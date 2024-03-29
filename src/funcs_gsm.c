//--------------------------------------------------
// #### VAPORE SMS COMMS PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### FUNCS_GSM.C ################################
//--------------------------------------------------

// Includes --------------------------------------------------------------------
#include "funcs_gsm.h"
#include "sim900_800.h"
#include "usart.h"
// #include "flash_program.h"
#include "parameters.h"
#include "comm.h"
#include "sms_gprs_data.h"
#include "reports.h"

#include <string.h>
#include <stdio.h>

// Local Module Configs --------------------------------------------------------
// #define GSM_SEND_SMS_ON_START


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    cmd_continue = 1,
    cmd_ok,
    cmd_error,
    cmd_timeout

} resp_cmd_e;


#define MAX_STARTUP_ERRORS    10    // 10 error allowed, give space for register
#define MAX_COMMS_ERRORS    20    // 20 errors on common use
#define MAX_SMS_ERRORS    20    // 20 errors on common use

#define TT_CHECK_RSSI    20000
#define TT_CHECK_RSSI_ON_ERROR    3000


// Externals -------------------------------------------------------------------
extern parameters_typedef mem_conf;
extern unsigned char register_status;
extern unsigned char rssi_level;
extern volatile char buffUARTGSMrx2[];


// Globals ---------------------------------------------------------------------
t_GsmState gsm_state = gsm_state_reset;
unsigned char gsm_sms_error_counter = 0;
unsigned char gsm_error_counter = 0;
char * p_MSG;
char * p_NUM;
char * p_CMD;
char * p_RESP;

char s_msg [100];

//flags
unsigned short GSMFlags = 0;

//TODO: estas tres trabajan juntas, pasar a estructura
unsigned char enviar_sms = 0;
char enviar_sms_num [20] = { '\0' };
char enviar_sms_msg [160] = { '\0' };
unsigned char send_location = 0;

// Timeout Timer
volatile unsigned short funcs_gsm_timeout_timer = 0;

char s_latitude [20] = { 0 };
char s_longitude [20] = { 0 };

// Module Functions ------------------------------------------------------------
//Procesa toda la pila del GSM (por lo menos para los SMS)
//los comandos que necesita el modulo se envian por otras funciones
void FuncsGSM (void)
{
    t_RespGsm resp = resp_gsm_continue;
    unsigned short flags = 0;

    switch (gsm_state)
    {
    case gsm_state_reset:
        resp = GSM_Start();

        if (resp == resp_gsm_ok)
        {
            gsm_error_counter = 0;
            gsm_state = gsm_state_verify_at;
        }

        if ((resp == resp_gsm_error) || (resp == resp_gsm_timeout))
        {
            Usart2Debug("module not started\n", 2);
            gsm_state = gsm_state_shutdown;
        }
        break;

    case gsm_state_verify_at:
        resp = GSMSendCommand ("AT\r\n", 1000, 0, &s_msg[0]);

        if (resp == 2)
        {
            // reset the expected flags
            FuncsGSMMessageFlags(GSM_RESET_FLAG | GSM_SET_CPIN | GSM_SET_CALL);
            gsm_state = gsm_state_wait_cpin;
        }
        
        if (resp > 2)
        {
            if (gsm_error_counter < MAX_STARTUP_ERRORS)
                gsm_error_counter++;
            else
                gsm_state = gsm_state_shutdown;
        }
        break;

        //espero cpin o call ready
    case gsm_state_wait_cpin:
        flags = FuncsGSMMessageFlagsAsk();

        // if ((flags & GSM_SET_CPIN) || (flags & GSM_SET_CALL))
        // {
        //     // reset flags 
        //     FuncsGSMMessageFlags(GSM_RESET_FLAG | GSM_SET_CPIN | GSM_SET_CALL);
        //     gsm_state = gsm_state_echo_disable;
        // }

        if ((flags & GSM_SET_CPIN) || (flags & GSM_SET_CALL))
            gsm_state = gsm_state_echo_disable;
        
        resp = GSM_Delay (8000);	//8 segundos de espera

        if (resp == resp_gsm_ok)
            gsm_state = gsm_state_echo_disable;

        break;

    case gsm_state_echo_disable:
        resp = GSMSendCommand ("ATE0\r\n", 1000, 0, &s_msg[0]);

        if (resp == 2)
            gsm_state = gsm_state_sms_mode;

        if (resp > 2)
        {
            if (gsm_error_counter < MAX_STARTUP_ERRORS)
                gsm_error_counter++;
            else
                gsm_state = gsm_state_shutdown;
        }
        break;

    case gsm_state_sms_mode:
        resp = GSMSendCommand ("AT+CMGF=1\r\n", 1000, 0, &s_msg[0]);

        if (resp == 2)
            gsm_state = gsm_state_sms_character;

        if (resp > 2)
        {
            if (gsm_error_counter < MAX_STARTUP_ERRORS)
                gsm_error_counter++;
            else
                gsm_state = gsm_state_shutdown;
        }
        break;

    case gsm_state_sms_character:
        resp = GSMSendCommand ("AT+CSCS=\"GSM\"\r\n", 1000, 0, &s_msg[0]);

        if (resp == 2)
            gsm_state = gsm_state_verify_fully_func;

        if (resp > 2)
        {
            if (gsm_error_counter < MAX_STARTUP_ERRORS)
                gsm_error_counter++;
            else
                gsm_state = gsm_state_shutdown;
        }
        break;

    case gsm_state_verify_fully_func:
        resp = GSMSendCommand ("AT+CFUN=1\r\n", 10000, 0, &s_msg[0]);

        if (resp == 2)
        {
            GSM_Start_Stop_ResetSM ();
            gsm_state = gsm_state_wait_reg;
        }

        if (resp > 2)
        {
            if (gsm_error_counter < MAX_STARTUP_ERRORS)
                gsm_error_counter++;
            else
                gsm_state = gsm_state_shutdown;
        }
        break;

    case gsm_state_wait_reg:
        resp = GSM_Delay (8000);	//8 segundos de espera

        if (resp == resp_gsm_ok)
            gsm_state = gsm_state_verify_reg;

        break;

    case gsm_state_verify_reg:
        resp = GSMSendCommand ("AT+CREG?\r\n", 1000, 1, &s_msg[0]);

        if (resp == 2)
        {
            resp = 3;
            if (!strncmp(s_msg, "+CREG: 0,1", sizeof("+CREG: 0,1") - 1))
            {
                gsm_state = gsm_state_dell_all;		//equipo registrado
                resp = 2;
            }

            if (!strncmp(s_msg, "+CREG: 0,5", sizeof("+CREG: 0,5") - 1))
            {
                gsm_state = gsm_state_dell_all;		//equipo registrado con roaming
                resp = 2;
            }
        }

        if (resp > 2)
        {
            if (gsm_error_counter < MAX_STARTUP_ERRORS)
            {
                gsm_error_counter++;
                gsm_state = gsm_state_wait_reg;
            }
            else
                gsm_state = gsm_state_shutdown;

        }
        break;

    case gsm_state_dell_all:
        //TODO: Realmente tengo que borrar todos los mensajes cuando arranco???
        resp = GSMSendCommand ("AT+CMGDA=\"DEL ALL\"\r\n", 25000, 0, &s_msg[0]);

        if (resp == 2)
            gsm_state = gsm_state_get_imei;

        if (resp > 2)
        {
            if (gsm_error_counter < MAX_STARTUP_ERRORS)
                gsm_error_counter++;
            else
                gsm_state = gsm_state_shutdown;

        }
        break;

    case gsm_state_get_imei:
        resp = GSMSendCommand ("AT+GSN\r\n", 1000, 1, &s_msg[0]);

        if (resp == 2)
        {
#ifdef GSM_SEND_SMS_ON_START
            unsigned char i = 0;

            i = strlen(s_msg);
            strncpy(mem_conf.imei, s_msg, (i - 2));
            Usart2Debug("IMEI: ", 1);
            Usart2Debug(mem_conf.imei, 1);
            Usart2Debug("\r\n", 1);

            //mando SMS con mi info
            strcpy(s_msg, "IMEI: ");
            strcat(s_msg, mem_conf.imei);
            strcat(s_msg, ", ACTIVO");

            p_MSG = s_msg;
            p_NUM = mem_conf.num_reportar;
            gsm_state = gsm_state_sending_sms;
#else
            Usart2Debug("GSM is Ready!\n", 1);
            gsm_state = gsm_state_ready;
#endif
        }

        if (resp > 2)
        {
            if (gsm_error_counter < MAX_STARTUP_ERRORS)
                gsm_error_counter++;
            else
                gsm_state = gsm_state_shutdown;
        }
        break;

    case gsm_state_ready:
        if (enviar_sms)
        {
            enviar_sms = 0;
            FuncsGSMSendSMS (enviar_sms_msg, enviar_sms_num);
        }
        else if (send_location)
        {
            gsm_state = gsm_state_getting_location;
        }
        else if (SMSLeft())    //si tengo algun mensaje paso a leerlo
            gsm_state = gsm_state_reading_sms;
        
        else if (!funcs_gsm_timeout_timer)
            gsm_state = gsm_state_check_rssi;

        break;

    case gsm_state_check_rssi:
        resp = GSMSendCommand ("AT+CSQ\r\n", 2000, 1, &s_msg[0]);

        if (resp == 2)
        {
            resp = 3;    // error for default
            
            if (!strncmp(s_msg, "+CSQ: ", sizeof("+CSQ: ") - 1))
            {
                char * p_colon;
                
                p_colon = s_msg + (sizeof("+CSQ: ") - 1);

                if (*(p_colon + 1) == ',')
                {
                    rssi_level = *p_colon - '0';
                }
                else if (*(p_colon + 2) == ',')
                {
                    rssi_level = (*p_colon - '0') * 10 + (*(p_colon + 1) - '0');
                }

                char s_ser [20] = { 0 };
                sprintf(s_ser, "RSSI: %d\n", rssi_level);
                Usart2Debug(s_ser, 2);
                gsm_state = gsm_state_check_network;
                resp = 2;
            }
        }
        
        if (resp > 2)
        {
            if (gsm_error_counter < MAX_COMMS_ERRORS)
            {
                gsm_error_counter++;
                gsm_state = gsm_state_ready;
                rssi_level = 0xff;    //tell to main the cmd problem via rssi_level variable
                funcs_gsm_timeout_timer = TT_CHECK_RSSI_ON_ERROR;
            }
            else
                gsm_state = gsm_state_shutdown;

        }
        break;

    case gsm_state_check_network:
        resp = GSMSendCommand ("AT+CREG?\r\n", 2000, 1, &s_msg[0]);

        if (resp == 2)
        {
            resp = 3;    // error for default
            
            if (!strncmp(s_msg, "+CREG: ", sizeof("+CREG: ") - 1))
            {
                char * p_colon;
                
                p_colon = s_msg + (sizeof("+CREG: ") - 1);

                if (*(p_colon + 1) == ',')
                    register_status = *(p_colon + 2) - '0';

                char s_ser [20] = { 0 };
                sprintf(s_ser, "REG: %d\n", register_status);
                Usart2Debug(s_ser, 2);
                if ((register_status == 1) || (register_status == 5))
                {
                    gsm_error_counter = 0;
                    funcs_gsm_timeout_timer = TT_CHECK_RSSI;
                    gsm_state = gsm_state_ready;
                    resp = 2;
                }
            }
        }
        
        if (resp > 2)
        {
            if (gsm_error_counter < MAX_COMMS_ERRORS)
            {
                gsm_error_counter++;
                gsm_state = gsm_state_ready;
                rssi_level = 0xff;    //tell to main the cmd problem via rssi_level variable
                funcs_gsm_timeout_timer = TT_CHECK_RSSI_ON_ERROR;
            }
            else
                gsm_state = gsm_state_shutdown;

        }
        break;
        
    case gsm_state_reading_sms:

        resp = GSMReceivSMS();

        if (resp == resp_gsm_ok)
            gsm_state = gsm_state_ready;

        if ((resp == resp_gsm_timeout) ||
            (resp == resp_gsm_error))
            gsm_state = resp_gsm_ok;    // TODO: must erase all the sms?

        break;

    case gsm_state_sending_sms:
        resp = GSMSendSMS (p_MSG, p_NUM);

        if (resp == resp_gsm_ok)
        {
            if (gsm_sms_error_counter)
                gsm_sms_error_counter--;

            Usart2Debug("end send sms ok\n", 1);
            gsm_state = gsm_state_ready;
        }

        if ((resp == resp_gsm_error) ||
            (resp == resp_gsm_timeout))
        {
            Usart2Debug("end send sms with errors\n", 1);
            
            if (gsm_sms_error_counter < MAX_SMS_ERRORS)
            {
                gsm_sms_error_counter++;
                gsm_state = gsm_state_ready;
            }
            else
                gsm_state = gsm_state_shutdown;

        }
        break;

    case gsm_state_sending_gprs:
        break;

    case gsm_state_getting_location:
        resp = FuncsGSM_GetLocation ();

        if (resp == resp_gsm_ok)
        {
            send_location = 0;
            Usart2Debug("location getted, sending sms...\n", 1);
            enviar_sms = 1;
            // error de compatibilidad en android
            // sprintf(enviar_sms_msg, "https://www.google.com/maps/@%s,%s",
            //         s_latitude,
            //         s_longitude);

            sprintf(enviar_sms_msg, "https://www.google.com/maps/search/?api=1&query=%s,%s",
                    s_latitude,
                    s_longitude);
            gsm_state = gsm_state_ready;
        }

        if (resp == resp_gsm_error)
        {
            send_location = 0;            
            Usart2Debug("location error, check apn, sending sms error...\n", 1);
            enviar_sms = 1;
            strcpy(enviar_sms_msg, "error de localizacion, verificar apn");
            gsm_state = gsm_state_ready;
        }
        break;
        
    case gsm_state_command_answer:
        // almost all commands take less than 10 secs to complete tasks
        resp = GSMSendCommand (p_CMD, 10000, 1, p_RESP);

        if (resp != 1)
        {
            gsm_state = gsm_state_ready;
        }
        break;

    case gsm_state_shutdown:
        sprintf(s_msg, "shutting down... errors cmd: %d sms: %d\n",
                gsm_error_counter,
                gsm_sms_error_counter);
        
        Usart2Debug(s_msg, 2);
        GSM_Start_Stop_ResetSM ();
        gsm_state = gsm_state_shutdown_2;
        break;

    case gsm_state_shutdown_2:
        resp = GSM_Stop();

        if (resp != resp_gsm_continue)
        {
            if ((resp == resp_gsm_error) ||
                (resp == resp_gsm_timeout))
            {
                Usart2Debug("module not stopped\n", 2);
            }        
            
            GSM_Start_Stop_ResetSM ();
            gsm_state = gsm_state_stop_wait;
        }
        break;
        
    case gsm_state_stop_wait:
        resp = GSM_Delay (10000);	//10 segundos de espera antes de prender

        if (resp == resp_gsm_ok)
        {
            GSM_Start_Stop_ResetSM ();
            gsm_state = gsm_state_reset;
        }
        break;

    case gsm_state_shutdown_always:
        resp = GSM_Stop();

        if (resp != resp_gsm_continue)
        {
            if ((resp == resp_gsm_error) ||
                (resp == resp_gsm_timeout))
            {
                Usart2Debug("module not stopped\n", 2);
            }        
            
            GSM_Start_Stop_ResetSM ();
            gsm_state = gsm_state_stop_always;
        }
        break;

    case gsm_state_stop_always:
        break;

    default:
        GSM_Start_Stop_ResetSM ();
        gsm_state = gsm_state_reset;
        break;
    }

    GSMProcess ();    //lee bytes del puerto serie y avisa con flag la terminacion del msj
    GSMReceive ();    //usa el flag para analizar las respuestas

}


// la llaman desde el modulo sim900_800 cuando existe un payload SMS valido
// debe interpretar el mensaje o descartar
void FuncsGSMGetSMSPayloadCallback (char * orig_num, char * payload)
{
    CommsProcessSMSPayload(orig_num, payload);
}


void FuncsGSMReset (void)
{
    GSM_Start_Stop_ResetSM ();
    gsm_state = gsm_state_reset;
}


unsigned char FuncsGSMSendSMS (char *ptrMSG, char *ptrNUM)
{
    if (gsm_state != gsm_state_ready)
        return resp_gsm_error;

    gsm_state = gsm_state_sending_sms;
    p_MSG = ptrMSG;
    p_NUM = ptrNUM;

    return resp_gsm_ok;
}


typedef enum {
    gprs_init = 0,
    gprs_close_last,
    gprs_single_conn,
    gprs_attach_gprs_service,
    gprs_set_apn,
    gprs_bring_up_wireless,
    gprs_get_ip,
    gprs_start_tcp_udp,
    gprs_tcp_udp_wait_conn,
    gprs_send_msg,
    gprs_send_msg_wait_sign,
    gprs_wait_close,
    gprs_close_tcp_udp,
    gprs_close_end
    
} send_gprs_e;


send_gprs_e send_gprs_state;
unsigned char FuncsGSMSendGPRS (char * message, unsigned char which_ip)
{
    unsigned char resp = resp_gsm_continue;
    resp_cmd_e resp_cmd = cmd_continue;
    char sbuff [100];


    switch (send_gprs_state)
    {
    case gprs_init:
        if (gsm_state != gsm_state_ready)
            resp = resp_gsm_error;
        else
        {
            // blocks FuncsGSM SM
            gsm_state = gsm_state_sending_gprs;
            FuncsGSM_ServerAnswer_Reset ();
            send_gprs_state++;
        }
        break;

    case gprs_close_last:
        resp_cmd = GSMSendCommand ("AT+CIPSHUT\r\n", 65000, 1, &s_msg[0]);

        if (resp_cmd == cmd_ok)
        {
            if (!strncmp(s_msg, "SHUT OK", sizeof("SHUT OK") - 1))
                send_gprs_state++;
            else
                resp = resp_gsm_error;
        }

        if (resp_cmd > cmd_ok)
            resp = resp_gsm_error;
        
        break;

    case gprs_single_conn:
        resp_cmd = GSMSendCommand ("AT+CIPMUX=0\r\n", 1000, 0, &s_msg[0]);

        if (resp_cmd == cmd_ok)
            send_gprs_state++;
        
        if (resp_cmd > cmd_ok)
            resp = resp_gsm_error;
        
        break;

    case gprs_attach_gprs_service:
        resp_cmd = GSMSendCommand ("AT+CGATT=1\r\n", 10000, 0, &s_msg[0]);

        if (resp_cmd == cmd_ok)
            send_gprs_state++;
        
        if (resp_cmd > cmd_ok)
            resp = resp_gsm_error;
        
        break;

    case gprs_set_apn:
        // resp_cmd = GSMSendCommand ("AT+CSTT=\"gprs.personal.com\",\"\",\"\"\r\n", 1000, 0, &s_msg[0]);
        sprintf(sbuff, "AT+CSTT=\"%s\",\"\",\"\"\r\n", mem_conf.apn);
        resp_cmd = GSMSendCommand (sbuff, 1000, 0, &s_msg[0]);

        if (resp_cmd == cmd_ok)
            send_gprs_state++;
        
        if (resp_cmd > cmd_ok)
            resp = resp_gsm_error;
        
        break;

    case gprs_bring_up_wireless:
        resp_cmd = GSMSendCommand ("AT+CIICR\r\n", 65000, 0, &s_msg[0]);

        if (resp_cmd == cmd_ok)
            send_gprs_state++;
        
        if (resp_cmd > cmd_ok)
            resp = resp_gsm_error;
        
        break;

    case gprs_get_ip:
        resp_cmd = GSMSendCommand ("AT+CIFSR\r\n", 1000, 1, &s_msg[0]);

        if (resp_cmd == cmd_ok)
            send_gprs_state++;
        
        if (resp_cmd > cmd_ok)
            resp = resp_gsm_error;
        
        break;

    case gprs_start_tcp_udp:
        // resp_cmd = GSMSendCommand ("AT+CIPSTART=\"TCP\",\"echo.u-blox.com\",\"13\"\r\n", 65000, 1, &s_msg[0]);
        // resp_cmd = GSMSendCommand ("AT+CIPSTART=\"TCP\",\"186.18.4.68\",\"10000\"\r\n", 65000, 1, &s_msg[0]);
        if (which_ip == IP1)
        {
            sprintf(sbuff, "AT+CIPSTART=\"%s\",\"%s\",\"%s\"\r\n",
                    mem_conf.ip_proto,
                    mem_conf.ip1,
                    mem_conf.ip_port1);
            resp_cmd = GSMSendCommand (sbuff, 65000, 1, &s_msg[0]);
        }
        else
        {
            sprintf(sbuff, "AT+CIPSTART=\"%s\",\"%s\",\"%s\"\r\n",
                    mem_conf.ip_proto,
                    mem_conf.ip2,
                    mem_conf.ip_port2);
            resp_cmd = GSMSendCommand (sbuff, 65000, 1, &s_msg[0]);
        }

        if (resp_cmd == cmd_ok)
        {
            FuncsGSMGPRSFlags(GPRS_RESET_FLAG | GPRS_CONN_OK | GPRS_CONN_FAIL);
            FuncsGSMGPRSFlags(GPRS_ENABLE_FLAGS);
            funcs_gsm_timeout_timer = 65000;

            // check for UDP or TCP conn
            if (!strncmp(mem_conf.ip_proto, "UDP", sizeof("UDP") - 1))
                send_gprs_state = gprs_send_msg;
            else
                send_gprs_state++;
        }
        
        if (resp_cmd > cmd_ok)
            resp = resp_gsm_error;
        
        break;

    case gprs_tcp_udp_wait_conn:
        
        if (FuncsGSMGPRSFlagsAsk() & GPRS_CONN_OK)
            send_gprs_state++;

        if (FuncsGSMGPRSFlagsAsk() & GPRS_CONN_FAIL)
            resp = resp_gsm_error;
        
        if (!funcs_gsm_timeout_timer)
        {
            Usart2Debug("gprs conn timeout\n", 2);
            resp = resp_gsm_error;
        }
        break;
        
    case gprs_send_msg:
        resp_cmd = GSMSendCommand ("AT+CIPSEND\r\n", 65000, 0, &s_msg[0]);

        if (resp_cmd == cmd_ok)    // send message here
            send_gprs_state++;
        
        if (resp_cmd > cmd_ok)
            resp = resp_gsm_error;
        
        break;

    case gprs_send_msg_wait_sign:
        strcpy(sbuff, message);
        strcat(sbuff, "\032");
        resp_cmd = GSMSendCommand (sbuff, 65000, 0, &s_msg[0]);

        if (resp_cmd == cmd_ok)    // wait for SEND OK
        {
            funcs_gsm_timeout_timer = 20000;
            Usart2Debug("wait server answer for 20secs\n", 2);
            send_gprs_state++;
        }
        
        if (resp_cmd > cmd_ok)
            resp = resp_gsm_error;
        
        break;

    case gprs_wait_close:
        // continuosly check if sended is equal to answered
        if(!strncmp(message, (const char *) buffUARTGSMrx2, strlen(message)))
        {
            Usart2Debug("answer from server getted!!!", 1);
            FuncsGSM_ServerAnswer_Set();
            send_gprs_state++;
        }
        
        if (!funcs_gsm_timeout_timer)
        {
            FuncsGSMGPRSFlags(GPRS_RESET_FLAG | GPRS_ENABLE_FLAGS);
            send_gprs_state++;
        }
        break;
        
    case gprs_close_tcp_udp:
        resp_cmd = GSMSendCommand ("AT+CIPCLOSE\r\n", 1000, 0, &s_msg[0]);

        if (resp_cmd == cmd_ok)
            send_gprs_state++;
        
        if (resp_cmd > cmd_ok)
            resp = resp_gsm_error;
        
        break;

    case gprs_close_end:
        resp_cmd = GSMSendCommand ("AT+CIPSHUT\r\n", 65000, 1, &s_msg[0]);

        if (resp_cmd == cmd_ok)
        {
            if (!strncmp(s_msg, "SHUT OK", sizeof("SHUT OK") - 1))
            {
                if (FuncsGSM_ServerAnswer_Get())
                    resp = resp_gsm_ok;
                else
                    resp = resp_gsm_error;
                
                send_gprs_state = gprs_init;
                // unblock FuncsGSM SM
                gsm_state = gsm_state_ready;
            }
            else
                resp = resp_gsm_error;
        }

        if (resp_cmd > cmd_ok)
            resp = resp_gsm_error;
        
        break;
        
    default:
        send_gprs_state = gprs_init;
        break;
    }

    // if any error, reset the SM and free the funcsGSM
    if ((resp == resp_gsm_error) ||
        (resp == resp_gsm_timeout))
    {
        send_gprs_state = gprs_init;
        // unblock FuncsGSM SM
        gsm_state = gsm_state_ready;
    }
    
    return resp;
}


typedef enum {
    bearer_init = 0,
    bearer_param,
    bearer_context,
    bearer_context_activate,
    bearer_read_param,
    lbs_set_address,
    lbs_get_location,
    bearer_deactivate,
    bearer_deactivate_with_errors
    
} get_loc_e;


get_loc_e get_loc_state;
unsigned char FuncsGSM_GetLocation (void)
{
    unsigned char resp = resp_gsm_continue;
    resp_cmd_e resp_cmd = cmd_continue;
    char sbuff [100];


    switch (get_loc_state)
    {
    case bearer_init:
        Usart2Debug("ask gsm for location\r\n", 1);
        get_loc_state++;
        break;

    case bearer_param:
        resp_cmd = GSMSendCommand ("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n", 1000, 0, &s_msg[0]);

        if (resp_cmd == cmd_ok)
            get_loc_state++;
        
        if (resp_cmd > cmd_ok)
            resp = resp_gsm_error;
        
        break;

    case bearer_context:
        sprintf(sbuff, "AT+SAPBR=3,1,\"APN\",\"%s\"\r\n", mem_conf.apn);
        resp_cmd = GSMSendCommand (sbuff, 1000, 0, &s_msg[0]);

        if (resp_cmd == cmd_ok)
            get_loc_state++;
        
        if (resp_cmd > cmd_ok)
            resp = resp_gsm_error;
        
        break;

    case bearer_context_activate:
        resp_cmd = GSMSendCommand ("AT+SAPBR=1,1\r\n", 65000, 0, &s_msg[0]);

        if (resp_cmd == cmd_ok)
            get_loc_state++;
        
        if (resp_cmd > cmd_ok)
            resp = resp_gsm_error;
        
        break;

    case bearer_read_param:
        resp_cmd = GSMSendCommand ("AT+SAPBR=2,1\r\n", 1000, 1, &s_msg[0]);

        if (resp_cmd == cmd_ok)
        {
            if (!strncmp(s_msg, "+SAPBR: 1,1,", sizeof("+SAPBR: 1,1,") - 1))
                get_loc_state++;
        }

        if (resp_cmd > cmd_ok)
        {
            get_loc_state = bearer_deactivate_with_errors;
            resp = resp_gsm_continue;
        }
        break;

    case lbs_set_address:
        resp_cmd = GSMSendCommand ("AT+CLBSCFG=1,3,\"lbs-simcom.com:3002\"\r\n", 1000, 0, &s_msg[0]);

        if (resp_cmd == cmd_ok)
            get_loc_state++;
        
        if (resp_cmd > cmd_ok)
        {
            get_loc_state = bearer_deactivate_with_errors;
            resp = resp_gsm_continue;
        }
        break;

    case lbs_get_location:
        resp_cmd = GSMSendCommand ("AT+CLBS=1,1\r\n", 10000, 1, &s_msg[0]);

        if (resp_cmd == cmd_ok)
        {
            if (!strncmp(s_msg, "+CLBS: 0,", sizeof("+CLBS: 0,") - 1))
            {
                unsigned char offset = sizeof("+CLBS: 0,") - 1;
                unsigned char offset_latitude = 0;
                unsigned char offset_longitude = 0;
                char c = 0;

                // get longitude
                for (int i = 0; i < 20; i++)
                {
                    c = *(s_msg + offset + i);
                    
                    if (c != ',')
                    {
                        s_longitude[i] = c;
                    }
                    else
                    {
                        s_longitude[i] = 0;
                        offset_longitude = i;
                        break;
                    }
                }
                // printf(" end longitude: %s offset: %d\n", s_longitude, offset_longitude);

                // get latitude
                for (int i = 0; i < 20; i++)
                {
                    c = *(s_msg + offset + offset_longitude + 1 + i);    //+1 for ',' offset
                    
                    if (c != ',')
                    {
                        s_latitude[i] = c;
                    }
                    else
                    {
                        s_latitude[i] = 0;
                        offset_latitude = i;
                        break;
                    }
                }
                // printf(" end latitude: %s offset: %d\n", s_latitude, offset_latitude);

                if ((offset_latitude < 7) ||
                    (offset_longitude < 7))
                {
                    Usart2Debug("latitude or longitude with errors\r\n", 1);
                    get_loc_state = bearer_deactivate_with_errors;
                }
                else
                {
                    get_loc_state++;
                }
            }
            else
            {
                Usart2Debug("not location getted\r\n", 1);
                get_loc_state = bearer_deactivate_with_errors;
            }
        }

        if (resp_cmd > cmd_ok)
        {
            get_loc_state = bearer_deactivate_with_errors;
            resp = resp_gsm_continue;
        }
        break;

    case bearer_deactivate:
        resp_cmd = GSMSendCommand ("AT+SAPBR=0,1\r\n", 65000, 0, &s_msg[0]);

        if (resp_cmd == cmd_ok)
        {
            get_loc_state = bearer_init;
            resp = resp_gsm_ok;
        }

        if (resp_cmd > cmd_ok)
            resp = resp_gsm_error;
        
        break;

    case bearer_deactivate_with_errors:
        resp_cmd = GSMSendCommand ("AT+SAPBR=0,1\r\n", 65000, 0, &s_msg[0]);

        if (resp_cmd != cmd_continue)
            resp = resp_gsm_error;
        
        break;
        
    default:
        get_loc_state = bearer_init;
        break;
    }

    // if any error, reset the SM and free the funcsGSM
    if ((resp == resp_gsm_error) ||
        (resp == resp_gsm_timeout))
    {
        get_loc_state = bearer_init;
    }
    
    return resp;
}


void FuncsGSMShutdown (void)
{
    GSM_Start_Stop_ResetSM ();
    gsm_state = gsm_state_shutdown;
}


void FuncsGSMShutdownAlways (void)
{
    GSM_Start_Stop_ResetSM ();
    gsm_state = gsm_state_shutdown_always;
}


unsigned char FuncsGSMReady (void)
{
    if (gsm_state == gsm_state_ready)
        return resp_gsm_ok;
    else
        return resp_gsm_error;
}


unsigned char FuncsGSMStateAsk (void)
{
    return gsm_state;
}


void FuncsGSMMessageFlags (unsigned short flag)
{
    // check if its a reset flag
    if (flag & GSM_RESET_FLAG)
        GSMFlags &= ~(flag);
    else			//set flags
        GSMFlags |= flag;

}


unsigned short FuncsGSMMessageFlagsAsk (void)
{
    return GSMFlags;
}


unsigned char GPRSFlags = 0;
void FuncsGSMGPRSFlags (unsigned char flag)
{
    // check if its a reset flag
    if (flag & GPRS_RESET_FLAG)
        GPRSFlags &= ~(flag);
    else			//set flags
        GPRSFlags |= flag;

}


unsigned short FuncsGSMGPRSFlagsAsk (void)
{
    return GPRSFlags;
}


unsigned char FuncsGSMCommandAnswer (char * pCMD, char * pIMEI)
{
    if (gsm_state != gsm_state_ready)
        return resp_gsm_error;

    gsm_state = gsm_state_command_answer;
    p_RESP = pIMEI;
    p_CMD = pCMD;

    return resp_gsm_ok;
}


//achica el buffer recibido a todos los codigos numeros letras y espacios
void FuncsGSMParser (unsigned char * orig, unsigned char * dest)
{
    while (*orig != '\0')
    {
        //todos los codigos numeros y letras        
        if ((*orig > 31) && (*orig < 127))
        {
            *dest = *orig;
            dest++;
        }
        orig++;
    }

    *dest = '\0';
}


void FuncsGSMTimeoutCounters (void)
{
    
    if (funcs_gsm_timeout_timer)
        funcs_gsm_timeout_timer--;
    
}

unsigned char server_answer = 0;
void FuncsGSM_ServerAnswer_Set (void)
{
    server_answer = 1;
}


void FuncsGSM_ServerAnswer_Reset (void)
{
    server_answer = 0;
}


unsigned char FuncsGSM_ServerAnswer_Get (void)
{
    return server_answer;
}

//--- end of file ---//

