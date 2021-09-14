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
#include "flash_program.h"
#include "comm.h"

#include <string.h>
#include <stdio.h>


// Externals -------------------------------------------------------------------
extern parameters_typedef mem_conf;
extern unsigned char register_status;
extern unsigned char rssi_level;


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

// Timeout Timer
volatile unsigned short funcs_gsm_timeout_timer = 0;

// Constants -------------------------------------------------------------------
// #define MAX_STARTUP_ERRORS		5		//a veces tarda mas en registrar
#define MAX_STARTUP_ERRORS		10		//lo paso a 10


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
            GSM_Start_Stop_ResetSM ();
            gsm_state = gsm_state_shutdown;
        }
        break;

    case gsm_state_verify_at:
        resp = GSMSendCommand ("AT\r\n", 1000, 0, &s_msg[0]);

        if (resp == 2)
            gsm_state = gsm_state_wait_cpin;

        if (resp > 2)
        {
            if (gsm_error_counter < MAX_STARTUP_ERRORS)
                gsm_error_counter++;
            else
            {
                GSM_Start_Stop_ResetSM ();
                gsm_state = gsm_state_shutdown;
            }
        }
        break;

        //espero cpin o call ready
    case gsm_state_wait_cpin:
        flags = FuncsGSMMessageFlagsAsk();

        if ((flags == GSM_SET_CPIN) || (flags == GSM_SET_CALL))
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
            {
                GSM_Start_Stop_ResetSM ();
                gsm_state = gsm_state_shutdown;
            }
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
            {
                GSM_Start_Stop_ResetSM ();
                gsm_state = gsm_state_shutdown;
            }
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
            {
                GSM_Start_Stop_ResetSM ();
                gsm_state = gsm_state_shutdown;
            }
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
            {
                GSM_Start_Stop_ResetSM ();
                gsm_state = gsm_state_shutdown;
            }
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


            //ya tengo la resp = 3 por default
            // if (!strncmp(s_msg, "+CREG: 0,2", sizeof("+CREG: 0,2") - 1))
            // 	resp = 3;		//equipo buscando nueva empresa
            // if (!strncmp(s_msg, "+CREG: 0,3", sizeof("+CREG: 0,3") - 1))
            // 	resp = 3;		//equipo mal o no registrado

        }

        if (resp > 2)
        {
            if (gsm_error_counter < MAX_STARTUP_ERRORS)
            {
                gsm_error_counter++;
                gsm_state = gsm_state_wait_reg;
            }
            else
            {
                gsm_state = gsm_state_shutdown;
            }
            GSM_Start_Stop_ResetSM ();
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
            {
                GSM_Start_Stop_ResetSM ();
                gsm_state = gsm_state_shutdown;
            }
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
            Usart2Send("IMEI: ");
            Usart2Send(mem_conf.imei);
            Usart2Send("\r\n");

            //mando SMS con mi info
            strcpy(s_msg, "IMEI: ");
            strcat(s_msg, mem_conf.imei);
            strcat(s_msg, ", ACTIVO");

            p_MSG = s_msg;
            p_NUM = mem_conf.num_reportar;
            gsm_state = gsm_state_sending_sms;
#else
            gsm_state = gsm_state_ready;
#endif
        }

        if (resp > 2)
        {
            if (gsm_error_counter < MAX_STARTUP_ERRORS)
                gsm_error_counter++;
            else
            {
                GSM_Start_Stop_ResetSM ();
                gsm_state = gsm_state_shutdown;
            }
        }
        break;

    case gsm_state_ready:
        //TODO: reviar aca contador de errores

        if (enviar_sms)
        {
            enviar_sms = 0;
            FuncsGSMSendSMS (enviar_sms_msg, enviar_sms_num);
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
            // Usart2Send("csq\n");
            // Usart2Send(s_msg);
            
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
                Usart2Send(s_ser);
            }
            
            gsm_state = gsm_state_check_network;
        }
        else if (resp > 2)
            gsm_state = gsm_state_check_network;

        break;

    case gsm_state_check_network:
        resp = GSMSendCommand ("AT+CREG?\r\n", 2000, 1, &s_msg[0]);

        if (resp == 2)
        {
            if (!strncmp(s_msg, "+CREG: ", sizeof("+CREG: ") - 1))
            {
                char * p_colon;
                
                p_colon = s_msg + (sizeof("+CREG: ") - 1);

                if (*(p_colon + 1) == ',')
                    register_status = *(p_colon + 2) - '0';

                char s_ser [20] = { 0 };
                sprintf(s_ser, "REG: %d\n", register_status);
                Usart2Send(s_ser);
                if ((register_status == 1) || (register_status == 5))
                {
                    gsm_error_counter = 0;
                    funcs_gsm_timeout_timer = 20000;
                    gsm_state = gsm_state_ready;
                }
                else if (gsm_error_counter > 1)
                {
                    GSM_Start_Stop_ResetSM ();
                    gsm_state = gsm_state_shutdown;
                }
                else
                {
                    gsm_error_counter++;
                    gsm_state = gsm_state_ready;
                    funcs_gsm_timeout_timer = 10000;
                }
            }
        }
        else if (resp > 2)
        {
            gsm_state = gsm_state_ready;
            funcs_gsm_timeout_timer = 20000;
        }        
        break;
        
    case gsm_state_reading_sms:

        resp = GSMReceivSMS();

        if (resp == resp_gsm_ok)
            gsm_state = gsm_state_ready;

        // if (resp == resp_gsm_timeout)    //TODO: agregado 23-06-2021 problemas con buffers grandes
        //     gsm_state = resp_gsm_ok;    //tengo que borrar los sms?
            

        break;

    case gsm_state_sending_sms:
        resp = GSMSendSMS (p_MSG, p_NUM, 60000);

        if (resp == resp_gsm_ok)
        {
            if (gsm_sms_error_counter)
                gsm_sms_error_counter--;

#ifdef DEBUG_ON
            Usart2Send("end send sms ok\n");
#endif
            gsm_state = gsm_state_ready;
        }

        if ((resp == resp_gsm_error) || (resp == resp_gsm_timeout))
        {
            gsm_sms_error_counter++;
#ifdef DEBUG_ON
            Usart2Send("end send sms with errors\n");
#endif            
            gsm_state = gsm_state_ready;
        }
        break;

    case gsm_state_command_answer:
        resp = GSMSendCommand (p_CMD, 10000, 1, p_RESP);	//la mayoria de los comandos no tarda mas de 10 secs

        if (resp != 1)
        {
            gsm_state = gsm_state_ready;
        }
        break;

    case gsm_state_shutdown:
        resp = GSM_Stop();

        if (resp == resp_gsm_ok)
        {
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

        if (resp == resp_gsm_ok)
        {
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

    GSMProcess ();		//lee bytes del puerto serie y avisa con flag la terminacion del msj
    GSMReceive ();		//usa el flag para analizar las respuestas

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
    //veo si es un reset flag
    if (flag & GSM_RESET_FLAG)
        GSMFlags &= flag;
    else			//set flags
        GSMFlags |= flag;

}


unsigned short FuncsGSMMessageFlagsAsk (void)
{
    return GSMFlags;
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

//--- end of file ---//

