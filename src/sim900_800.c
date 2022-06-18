//----------------------------------------------------
// #### PROYECTO KIRNO REDONDA GSM - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### SIM900_800.C #################################
//----------------------------------------------------

// Includes --------------------------------------------------------------------
#include "sim900_800.h"
#include "usart.h"
#include "tim.h"
#include "funcs_gsm.h"		//para string flags
#include "parameters.h"
#include "comm.h"


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Local Module Configs --------------------------------------------------------
#define USE_SIM800C    //start and stop sequence with SM (powerkey and status lines)
// #define USE_SIM800L    //no start nor stop sequence - always on

// Check and inform gsm module selection
#if defined USE_SIM800C
#pragma message "------------ USING SIM800C GSM MODULE ------------"
#elif defined USE_SIM800L
#pragma message "------------ USING SIM800L GSM MODULE ------------"
#else
#error "Not gsm module selected in sim900_800.c"
#endif

#if (defined HARDWARE_VER_2_0) && (!defined USE_SIM800L)
#error "hard 2.0 must be used with sim800l"
#endif

#if (defined HARDWARE_VER_1_2) || \
    (defined HARDWARE_VER_1_1) || \
    (defined HARDWARE_VER_1_0)
#ifdef USE_SIM800L
#error "hard 1.2 1.1 or 1.0 not have sim800l"
#endif
#endif


// Externals -------------------------------------------------------------------
extern parameters_typedef mem_conf;
extern unsigned short comms_global_flag;


// Globals ---------------------------------------------------------------------
//UART GSM.
//RX.
volatile char buffUARTGSMrx2[buffUARTGSMrx_dimension];
volatile unsigned char GSM_PacketReady = 0;
//TX.

//GSM Start Stop and Delay variables
unsigned char GSMStartState = 0;
volatile unsigned short GSMStartTime;

//GSM SendCommand.
char GSMSendCommandState = 0;
char GSMSendCommandFlag = 0;
char GSMbuffRtaCommand[buffUARTGSMrx_dimension];
volatile unsigned short GSMSendCommandTimeOut;

//GSM SendSMS
char GSMSendSMSState = 0;
char GSMSendSMSbuffAux[32];

//Recepcion de SMS.
unsigned char GSMCantSMS = 0;
char GSMReadSMSState = 0;
char GSMReadSMScommand[32];


// Module Functions ------------------------------------------------------------
//Procesa respuestas del modulo GSM esperando el final de linea por timeout
//carga el buffer buffUARTGSMrx2 y avisa con el flag GSM_PacketReady
void GSMProcess (void)
{
    if (Usart1HaveData())
    {
        Usart1HaveDataReset();
        Usart1ReadBuffer((unsigned char *) buffUARTGSMrx2, sizeof(buffUARTGSMrx2));
        GSM_PacketReady = 1;
    }
}


//GSM_Start() possible answers:
//0 trabajando
//1 terminado OK
//2 ERROR
//3 Timeout
typedef enum {
    START_CHECK_STATUS,
    START_STATUS_UP_WAIT_CONFIRM,
    START_STATUS_DWN,
    START_STATUS_DWN_1,
    START_STATUS_DWN_2,
    START_STATUS_DWN_3,
    START_STATUS_DWN_4

} gsm_start_state_e;
    

#ifdef USE_SIM800L
unsigned char GSM_Start (void)    // actually a reset
{
    GSM_PWRKEY_OFF;
    Wait_ms(200);
    GSM_PWRKEY_ON;
    Wait_ms(3000);    
    return 1;
}
#endif    //USE_SIM800L

#ifdef USE_SIM800C    
unsigned char GSM_Start (void)
{    
    switch(GSMStartState)
    {
    case START_CHECK_STATUS:
        if (GSM_STATUS)
        {
            GSMStartTime = 600;    // wait 600ms to confirm
            GSMStartState = START_STATUS_UP_WAIT_CONFIRM;
        }
        else
            GSMStartState = START_STATUS_DWN;

        break;

    case START_STATUS_UP_WAIT_CONFIRM:
        if(!GSMStartTime)
        {
            if (GSM_STATUS)
            {
                // powerkey to on, almost sure it was already in on
                LED_NETLIGHT_ON;
                GSM_PWRKEY_ON;
                GSMStartState = START_CHECK_STATUS;
                return 1;
            }
            else
                GSMStartState = START_STATUS_DWN;
        }
        break;
        
    case START_STATUS_DWN:    
        // powerkey to ON
        LED_NETLIGHT_ON;
        GSMStartTime = 100;    // powerkey on 100ms
        GSM_PWRKEY_ON;
        GSMStartState++;
        break;
        
    case START_STATUS_DWN_1:
        // powerkey off
        if(!GSMStartTime)
        {
            LED_NETLIGHT_OFF;
            GSM_PWRKEY_OFF;
            GSMStartTime = 1500;    // 1.5 secs powerkey off
            GSMStartState++;
        }
        break;

    case START_STATUS_DWN_2:
        if(!GSMStartTime)
        {
            GSM_PWRKEY_ON;
            GSMStartTime = 2500;    // 2.5 secs powerkey on to check status again
            GSMStartState++;
        }
        break;

    case START_STATUS_DWN_3:
        if (GSM_STATUS)    // wait status up with powerkey on
        {
            GSMStartTime = 1000;
            GSMStartState++;
            LED_NETLIGHT_ON;
        }
        else if(!GSMStartTime)    // status never goes up, finish with timeout
        {
            GSMStartState = START_CHECK_STATUS;
            return 3;    //TimeOut
        }
        break;

    case START_STATUS_DWN_4:
        if(!GSMStartTime)    // last check if its up OK, otherwise NOK
        {
            GSMStartState = START_CHECK_STATUS;
            
            if (GSM_STATUS)
                return 1;    //OK
            else
                return 2;    //Error
        }
        break;

    default:
        GSMStartState = START_CHECK_STATUS;
        break;
    }
    
    return 0;    //trabajando
}
#endif    //USE_SIM800C
    
void GSM_Start_Stop_ResetSM (void)
{
    GSMStartState = 0;
}

//GSM_Stop() contesta:
//0 trabajando
//1 terminado OK
//2 ERROR
//3 Timeout
typedef enum {
    STOP_CHECK_STATUS,
    STOP_STATUS_DWN_WAIT_CONFIRM,
    STOP_STATUS_DWN_CHANGE,
    STOP_STATUS_UP,
    STOP_STATUS_UP_1,
    STOP_STATUS_UP_2

} gsm_stop_state_e;

#ifdef USE_SIM800L
unsigned char GSM_Stop(void)    // actually a reset
{    
    GSM_PWRKEY_OFF;
    Wait_ms(200);
    GSM_PWRKEY_ON;
    Wait_ms(3000);        
    return 1;
}
#endif    //USE_SIM800L
    
#ifdef USE_SIM800C    
unsigned char GSM_Stop(void)
{
    switch(GSMStartState)
    {
    case STOP_CHECK_STATUS:
        GSM_PWRKEY_ON;    // powerkey must be always on (for standby)
        
        if (!GSM_STATUS)
        {
            GSMStartTime = 3000;    // wait 3 secs to confirm
            GSMStartState = STOP_STATUS_DWN_WAIT_CONFIRM;
        }
        else
            GSMStartState = STOP_STATUS_UP;

        break;

    case STOP_STATUS_DWN_WAIT_CONFIRM:
        if (GSM_STATUS)    // anytime if status went up
        {
            GSMStartTime = 500;    // give module some reasonable time
            GSMStartState = STOP_STATUS_DWN_CHANGE;
        }
        else if(!GSMStartTime)
        {
            // module its sure off
            LED_NETLIGHT_OFF;
            GSMStartState = STOP_CHECK_STATUS;
            return 1;
        }        
        break;

    case STOP_STATUS_DWN_CHANGE:
        if(!GSMStartTime)
            GSMStartState = STOP_STATUS_UP;

        break;

    case STOP_STATUS_UP:
        GSMStartTime = 3000;
        GSM_PWRKEY_OFF;    // powerkey do a power cycle
        LED_NETLIGHT_OFF;
        GSMStartState++;
        break;

    case STOP_STATUS_UP_1:
        // status went down and powerkey was down at least 2secs
        if ((!GSM_STATUS) && (GSMStartTime < 1000))    
        {
            GSMStartTime = 1000;
            GSM_PWRKEY_ON;    // powerkey to on for standby
            GSMStartState++;
        }
        else if(!GSMStartTime) // wait up to 3secs in total
        {
            GSM_PWRKEY_ON;    // powerkey to on for standby
            GSMStartState = STOP_CHECK_STATUS;
            return 3;		//TimeOut
        }
        break;

    case STOP_STATUS_UP_2:
        if (!GSMStartTime)
        {            
            GSMStartState = STOP_CHECK_STATUS;
            
            if (!GSM_STATUS)    // module its sure off
                return 1;
            else
                return 2;
        }
        break;
    }

    return 0;
}
#endif    //USE_SIM800C    


//in: delay en ms
//out: 0 working; 1 finish
unsigned char GSM_Delay (unsigned short tim)
{
    unsigned char resp = 0;

    switch(GSMStartState)
    {
    case 0:
        GSMStartTime = tim;
        GSMStartState++;
        break;

    case 1:
        if (!GSMStartTime)
        {
            GSMStartState = 0;
            resp = 1;
        }
        break;

    default:
        GSMStartState = 0;
        break;
    }
    return resp;
}


//Procesa respuestas del modulo GSM ubicadas en el buffer buffUARTGSMrx2 con flag GSM_PacketReady
//revisa los flag de estados GSMConfigGPRSflag GSMSendCommandFlag y revisa respuestas no esperadas
//para respuestas no esperadas revisa SMS y avisa la cantidad con GSMCantSMS
//void GSMReceive (unsigned char * pAlertasReportar, char * puserCode, unsigned char * pclaveAct, unsigned char * pActDact)
void GSMReceive (void)
{
    //---- Comunicacion con modulo GSM ----//
    if (GSM_PacketReady)
    {
        FuncsGSMParser((unsigned char *)buffUARTGSMrx2, (unsigned char *)buffUARTGSMrx2);
        Usart2Debug((char *)buffUARTGSMrx2);
        Usart2Debug("\r\n");

        if (GSMSendCommandFlag)
        {
            if (GSMSendCommandFlag == 3)	// command no wait for answers
            {
                //reviso si existe un OK en el string
                for (unsigned short i = 0; i < (buffUARTGSMrx_dimension - 2); i++)
                {
                    if ((*(buffUARTGSMrx2 + i) != '\0') &&
                        (*(buffUARTGSMrx2 + i + 1) != '\0'))
                    {
                        if ((*(buffUARTGSMrx2 + i) == 'O') &&
                            (*(buffUARTGSMrx2 + i + 1) == 'K'))
                        {
                            GSMSendCommandFlag = 4;
                            break;
                        }
                    }
                    else
                        break;
                    
                }

                if (GSMSendCommandFlag != 4)    //todavía no vi el OK??, sera otra cosa?
                {
                    if(!strncmp((const char *)&buffUARTGSMrx2[0], (const char *)"> ", (sizeof("> ") - 1)))
                        GSMSendCommandFlag = 4;                    
                }
            }

            if(!strncmp((const char *)&buffUARTGSMrx2[0], (const char *) "ERROR", (sizeof("ERROR") - 1)))
                GSMSendCommandFlag = 5;

            if (GSMSendCommandFlag == 1)    // wait for answers
                GSMSendCommandFlag = 2;    // authorize the copy of answer always

        }

        // sometimes waited answers
        if (FuncsGSMGPRSFlagsAsk() & GPRS_ENABLE_FLAGS)
        {
            if(!strncmp((const char *)buffUARTGSMrx2, "CONNECT OK", sizeof("CONNECT OK") - 1))
                FuncsGSMGPRSFlags(GPRS_CONN_OK);

            if(!strncmp((const char *)buffUARTGSMrx2,
                        "STATE: TCP CLOSEDCONNECT FAIL",
                        sizeof("STATE: TCP CLOSEDCONNECT FAIL") - 1))
                FuncsGSMGPRSFlags(GPRS_CONN_FAIL);

        // if(!strncmp((const char *)&buffUARTGSMrx2[0], (const char *)"CLOSED", strlen((const char *)"CLOSED")))
        // {
        //     flagCloseIP = 1;
        // }

            
        }

        // no waited flags or answers
        // first check flags
        if(!strncmp((const char *)&buffUARTGSMrx2[0], (const char *)"+CPIN: ", (sizeof("+CPIN: ") - 1)))
        {
            FuncsGSMMessageFlags (GSM_SET_CPIN);
        }

        if(!strncmp((const char *)&buffUARTGSMrx2[0], (const char *)"Call Ready", (sizeof("Call Ready") - 1)))
            FuncsGSMMessageFlags (GSM_SET_CALL);

        if(!strncmp((const char *)&buffUARTGSMrx2[0], (const char *)"SMS Ready", (sizeof("SMS Ready") - 1)))
            FuncsGSMMessageFlags (GSM_SET_SMS);

        if(!strncmp((const char *)&buffUARTGSMrx2[0],
                    (const char *)"NORMAL POWER DOWN",
                    (sizeof("NORMAL POWER DOWN") - 1)))
        {
            FuncsGSMMessageFlags (GSM_SET_POWER_DOWN);
        }

        if (!strncmp((char *)&buffUARTGSMrx2[0], (const char *)"+CMTI: \"SM\",", sizeof ("+CMTI: \"SM\",") -1))
        {
            //TODO: modificar a dos bytes
            GSMCantSMS = buffUARTGSMrx2[12] - 48;
        }

        if (!strncmp((char *)&buffUARTGSMrx2[0], "RING", sizeof ("RING") -1))
        {
            comms_activate_ringing_flag_set;
        }
        
        GSM_PacketReady = 0;
    }
}

//-----------------------------------------------------//
//char GSMSendCommand (char *ptrCommand, unsigned char timeOut, unsigned char rta, char *ptrRta)
//----------------------------------------------------//
//ptrCommand: Comando a enviar.
//ptrRta: Respuesta al comando desde sim900.
//rta: 0: No espera rta. 1: Espera respuesta antes de OK.
//TO: time out. en ms
//----------------------------------------------------//
//Wait: 	1
//OK o RTA:	2
//ERR:		3
//TimeOut:      4
//----------------------------------------------------//
char GSMSendCommand (char *ptrCommand, unsigned short timeOut, unsigned char rta, char *ptrRta)
{
    switch(GSMSendCommandState)
    {
    case 0:
        GSMSendCommandFlag = 0;
        GSMSendCommandTimeOut = timeOut;
        GSMSendCommandState++;
        break;

    case 1:
        UARTGSMSend(&ptrCommand[0]);

        if (rta)
        {
            GSMSendCommandState = 2;
            GSMSendCommandFlag = 1;
        }
        else
        {
            GSMSendCommandState = 3;
            GSMSendCommandFlag = 3;
        }
        break;

    case 2:
        //Espera rta.
        if (GSMSendCommandFlag == 2)
        {
            //Rta obtenida.
            strcpy((char *)ptrRta, (const char *)&buffUARTGSMrx2[0]);
            //OK pegado
            GSMSendCommandFlag = 0;
            GSMSendCommandState = 0;
            return 2;
        }
        break;

    case 3:
        //Espera OK.
        if (GSMSendCommandFlag == 4)
        {
            //OK recibido.
            GSMSendCommandFlag = 0;
            GSMSendCommandState = 0;
            return 2;
        }
        break;

    default:
        GSMSendCommandState = 0;
        break;
    }

    if (GSMSendCommandFlag == 5)	//"ERROR"
    {
        GSMSendCommandFlag = 0;
        GSMSendCommandState = 0;
        return 3;
    }

    if (!GSMSendCommandTimeOut)
    {
        GSMSendCommandFlag = 0;
        GSMSendCommandState = 0;
        return 4;
    }

    return 1;
}


//-----------------------------------------------------------------------//
//char GSMSendSMS (char *ptrMSG, char *ptrNUM, unsigned char timeOut)
//-----------------------------------------------------------------------//
//String de mensaje: 	"..."
//timeOut en mS
//String Numero: 		"----------"
//Wait: 	0
//OK:		1
//ERR:	2
//TO:		3
//-----------------------------------------------------------------------//
t_RespGsm GSMSendSMS (char *ptrMSG, char *ptrNUM)
{
    unsigned char i;

    switch(GSMSendSMSState)
    {
    case 0:
        strcpy(&GSMSendSMSbuffAux[0], "AT+CMGS=\"");
        strcat(&GSMSendSMSbuffAux[0], ptrNUM);
        strcat(&GSMSendSMSbuffAux[0], "\"\r\n");
        GSMSendSMSState++;
        Usart2Debug("send sms\n");
        break;

    case 1:
        i = GSMSendCommand(&GSMSendSMSbuffAux[0], 60000, 0, &GSMbuffRtaCommand[0]);

        if (i == 2)
        {
            strcpy(&GSMSendSMSbuffAux[0], ptrMSG);
            strcat(&GSMSendSMSbuffAux[0], "\032");
            GSMSendSMSState++;
        }

        if (i > 2)
        {
            GSMSendSMSState = 0;
            return i-1;
        }
        break;

    case 2:
        i = GSMSendCommand(&GSMSendSMSbuffAux[0], 60000, 1, &GSMbuffRtaCommand[0]);

        if (i == 2)
        {
            GSMSendSMSState = 0;

            if (!strncmp(GSMbuffRtaCommand, "+CMGS: ", sizeof("+CMGS: ") - 1))
                return 1;
            else
                return 2;
        }

        if (i > 2)
        {
            GSMSendSMSState = 0;
            return i-1;
        }
        break;

    default:
        GSMSendSMSState = 0;
        break;
    }

    return 0;
}


char last_num [20] = { '\0' };
t_RespGsm GSMReceivSMS (void)
{
    t_RespGsm resp = resp_gsm_continue;
    unsigned char i, j, colon_index;
    char * pToAnswer;

    switch(GSMReadSMSState)
    {
    case 0:
        if (GSMCantSMS)    //avanzo solo si tengo algo que leer
        {
            sprintf(&GSMReadSMScommand[0], (const char *)"AT+CMGR=%d\r\n", GSMCantSMS);
            Usart2Debug("receiv sms\n");
            GSMReadSMSState++;
        }
        break;

    case 1:
        i = GSMSendCommand (&GSMReadSMScommand[0], 6000, 1, &GSMbuffRtaCommand[0]);

        if (i == 2)
        {
            if (!strncmp((char *)&GSMbuffRtaCommand[0], (const char *)"+CMGR:", sizeof("+CMGR:") - 1))
            {
                unsigned char num_start = 0;
                unsigned char num_end = 0;

                memset(last_num, '\0', sizeof(last_num));
                    
                //mensajes al modulo
                //+CMGR: "REC UNREAD","+5491145376762","","20/05/21,20:25:22-12"PRENDER:OK 
                colon_index = 0;
                for (j = 0; j < 222; j++)		//222 	160bytes para SMS + 62 bytes header
                {
                    if (GSMbuffRtaCommand[j] == '"')
                    {
                        colon_index++;
                        if (colon_index == 3)
                            num_start = j + 1;

                        if (colon_index == 4)
                            num_end = j;
                            
                        if (colon_index == 8)
                        {
                            pToAnswer = (char *) &GSMbuffRtaCommand [j+1];
                            j = 222;
                        }
                    }
                }
                    
                //fin del for
                // copio el numero de origen del mensaje
                char s_debug [60];
                strncpy(last_num, &GSMbuffRtaCommand[num_start], num_end - num_start);
                sprintf(s_debug, "el numero origen: %s\n", last_num);
                Usart2Debug(s_debug);
                    
                //en pToAnswer debo tener la respuesta (payload del SMS)
                if (colon_index == 8)
                {
                    //Son todos payloads correctos REVISO RESPUESTAS
                    FuncsGSMGetSMSPayloadCallback(last_num, pToAnswer);
                }

                //me fijo si tengo mas SMS, quito el index de la lista
                // y paso a borrar los leidos
                GSMCantSMS--;
                GSMReadSMSState++;
            }
            else    //es una respuesta pero no parece un sms
            {
                //TODO: debiera contar errores antes de borrar este sms
                GSMCantSMS--;
                GSMReadSMSState++;
            }
        }

        if (i > 2)
        {
            resp = i;
        }
        break;

    case 2:
        if (GSM_Delay(200))
            GSMReadSMSState++;

        break;
            
    case 3:
        i = GSMSendCommand ("AT+CMGDA=\"DEL READ\"\r\n", 25000, 0, &GSMbuffRtaCommand[0]);

        //modificacion 19-12-17, si no logre borrar igual dejo vacios sms
        //ademas uso GSMCantSMS como flag para seguir trabajando
        if (i != 1)
        {
            char s_debug [40] = { 0 };
            sprintf(s_debug, "del read answer: %d\n", i);
            Usart2Debug(s_debug);
            GSMReadSMSState = 0;
            resp = resp_gsm_ok;
        }
        break;

    default:
        GSMReadSMSState = 0;
        break;
    }

    return resp;
}


unsigned char SMSLeft (void)
{
    return GSMCantSMS;
}


void GSMTimeoutCounters (void)
{
    // Start Stop and Delay timeout
    if (GSMStartTime)
        GSMStartTime--;

    if(GSMSendCommandTimeOut)
        GSMSendCommandTimeOut--;

}


//--- end of file ---//
