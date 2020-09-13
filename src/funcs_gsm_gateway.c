//---------------------------------------------------------
// #### PROYECTO COMUNICADOR VAPORE SMS - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### FUNCS_GSM_GATEWAY.C ###############################
//---------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "funcs_gsm_gateway.h"
#include "sim900_800.h"
#include "usart.h"
#include "tim.h"
#include "hard.h"
#include "stm32g0xx.h"
#include <string.h>
#include <stdio.h>


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
volatile unsigned short timer_gsm_gw = 0;

// Constants -------------------------------------------------------------------

// Module Private Functions ----------------------------------------------------
unsigned char FuncsGSMG_ProcessCommands (char *);
void FuncsGSMG_SendStatus (char *);

// Module Functions ------------------------------------------------------------
//Procesa toda la pila del GSM (por lo menos para los SMS)
//los comandos que necesita el modulo se envian por otras funciones
void FuncsGSMGateway (void)
{

//---------- Pruebas con GSM GATEWAY --------//
    LED_OFF;
    for (unsigned char i = 0; i < 6; i++)
    {
        if (LED)
            LED_OFF;
        else
            LED_ON;

        Wait_ms (300);
    }

    Wait_ms (3000);
    Usart2Send("GSM GATEWAY.. Cambio a GSM\r\n");

    //mando start al gsm
    Usart2Send("Reset y Start GSM\r\n");
    //GPSStartResetSM ();
    timer_gsm_gw = 60000;		//doy 1 minuto para prender modulo
    unsigned char i = 0;
    while (timer_gsm_gw)
    {
        i = GSM_Start();
        if (i == 1)
        {
            Usart2Send("Start OK\r\n");
            timer_gsm_gw = 0;
        }

        if (i > 1)
        {
            Usart2Send("Start NOK\r\n");
            Usart2Send("Please reboot!\r\n");
        }
    }

    Usart2Send("GSM GATEWAY Listo para empezar\r\n");

    char buff [256] = { 0 };
    while (1)
    {
        if (Usart2HaveData())
        {
            Usart2HaveDataReset();
            Usart2ReadBuffer((unsigned char *)buff, sizeof(buff));
            //here process the commands from the PC
            if (!FuncsGSMG_ProcessCommands(buff))    //if not a cmd send to the gsm
                Usart1Send(buff);
        }

        if (Usart1HaveData())
        {
            Usart1HaveDataReset();
            Usart1ReadBuffer((unsigned char *)buff, sizeof(buff));
            Usart2Send(buff);
        }
    }
}


//contesta 1 si proceso, 0 si no era un comando
unsigned char FuncsGSMG_ProcessCommands (char * buff)
{
    unsigned char result = 0;

    char answers [100] = { 0 };
    if (!strncmp(buff, "gsm_status", sizeof ("gsm_status") -1))
    {
        FuncsGSMG_SendStatus(answers);
        result = 1;
    }

    if (!strncmp(buff, "gsm_power_off", sizeof ("gsm_power_off") -1))
    {
        unsigned char resp = 0;

        Usart2Send("Stopping GSM...\n");
        
        do {
            resp = GSM_Stop();
        } while (resp == 0);

        if (resp == 1)
            Usart2Send("Stop OK\n");            

        if (resp == 2)
            Usart2Send("Error\n");            

        if (resp == 3)
            Usart2Send("Timeout\n");            

        FuncsGSMG_SendStatus(answers);
        result = 1;
    }

    if (!strncmp(buff, "gsm_power_on", sizeof ("gsm_power_on") -1))
    {
        unsigned char resp = 0;

        Usart2Send("Starting GSM...\n");
        
        do {
            resp = GSM_Start();
        } while (resp == 0);

        if (resp == 1)
            Usart2Send("Start OK\n");            

        if (resp == 2)
            Usart2Send("Error\n");            

        if (resp == 3)
            Usart2Send("Timeout\n");            

        FuncsGSMG_SendStatus(answers);
        result = 1;
    }
    

    return result;
}


void  FuncsGSMG_SendStatus (char * buff)
{
    sprintf(buff, "STATUS: %d NETLIGHT: %d PWRKEY: %d\n",
            STATUS,
            NETLIGHT,
            PWRKEY);
        
    Usart2Send(buff);
}


void FuncsGSMG_Timeouts (void)
{
    if (timer_gsm_gw)
        timer_gsm_gw--;
}

// la llaman desde el modulo sim900_800 cuando existe un payload SMS valido
// debe interpretar el mensaje o descartar
// void FuncsGSMGetSMSPayloadCallback (char * orig_num, char * payload)
// {
//     unsigned char index = 0;

//     // Configurations
//     if (!strncmp(payload, "REPORTAR_OK:1", sizeof ("REPORTAR_OK:1") -1))
//     {
//         envios_ok = 1;
//         envios_ok_change_set;

//         enviar_sms = 1;
//         strcpy(enviar_sms_num, orig_num);
//         strcpy(enviar_sms_msg, "OK");
//     }

//     if (!strncmp(payload, "REPORTAR_OK:0", sizeof ("REPORTAR_OK:0") -1))
//     {
//         envios_ok = 0;
//         envios_ok_change_set;
//     }

//     if (!strncmp(payload, "PRENDER_RING:0", sizeof ("PRENDER_RING:0") -1))
//     {
//         prender_ring = 0;
//         prender_ring_change_set;

//         if (envios_ok)
//         {
//             enviar_sms = 1;
//             strcpy(enviar_sms_num, orig_num);
//             strcpy(enviar_sms_msg, "OK");
//         }
//     }

//     if (!strncmp(payload, "PRENDER_RING:1", sizeof ("PRENDER_RING:1") -1))
//     {
//         prender_ring = 1;
//         prender_ring_change_set;

//         if (envios_ok)
//         {
//             enviar_sms = 1;
//             strcpy(enviar_sms_num, orig_num);
//             strcpy(enviar_sms_msg, "OK");
//         }        
//     }
    
//     if (!strncmp(payload, (const char *)"TIMER:", sizeof ("TIMER:") -1))
//     {
//         index = 0;
//         index += (*(payload + 6) - 48) * 10;
//         index += *(payload + 7) - 48;

//         if ((*(payload + 6) == 'F') && (*(payload + 7) == 'F'))
//         {
//             timer_rep = 0;
//         }
//         else if ((index > 1) && (index <= 60))
//         {
// #ifdef DEBUG_ON
//             char debug [60] = {'\0'};
//             sprintf(debug, "nuevo timer %d", index);
//             Usart2Send(debug);
// #endif
//             timer_rep = index;
//             timer_rep_change_set;
//         }

//         if (envios_ok)
//         {
//             enviar_sms = 1;
//             strcpy(enviar_sms_num, orig_num);
//             strcpy(enviar_sms_msg, "OK");
//         }
//     }

//     // if (!strncmp(payload, (const char *)"TIMERD:", sizeof ("TIMERD:") -1))
//     // {
//     //     index = 0;
//     //     index += (*(payload + 7) - 48) * 10;
//     //     index += *(payload + 8) - 48;

//     //     if ((*(payload + 7) == 'F') && (*(payload + 8) == 'F'))
//     //     {
//     //         timer_debug = 0;
//     //         send_sms_ok_set;
//     //     }
//     //     else if ((index > 1) && (index <= 60))
//     //     {
//     //         timer_debug = index;
//     //         send_sms_ok_set;
//     //     }
//     // }

//     if (!strncmp(payload, (const char *)"PRENDER:", sizeof ("PRENDER:") -1))
//     {
//         diag_prender_set;
//         if (envios_ok)
//         {
//             enviar_sms = 1;
//             strcpy(enviar_sms_num, orig_num);
//             strcpy(enviar_sms_msg, "OK");
//         }
//     }

//     // if (!strncmp(payload, (const char *)"APAGAR:", sizeof ("APAGAR:") -1))
//     //     diag_apagar_set;

//     // if (!strncmp(payload, (const char *)"ENERGIA:", sizeof ("ENERGIA:") -1))
//     //     send_energy_set;

// }


// void FuncsGSMReset (void)
// {
//     GSM_Start_Stop_ResetSM ();
//     gsm_state = gsm_state_reset;
// }


// unsigned char FuncsGSMSendSMS (char *ptrMSG, char *ptrNUM)
// {
//     if (gsm_state != gsm_state_ready)
//         return resp_gsm_error;

//     gsm_state = gsm_state_sending_sms;
//     p_MSG = ptrMSG;
//     p_NUM = ptrNUM;

//     return resp_gsm_ok;
// }


// void FuncsGSMShutdown (void)
// {
//     GSM_Start_Stop_ResetSM ();
//     gsm_state = gsm_state_shutdown;
// }


// void FuncsGSMShutdownAlways (void)
// {
//     GSM_Start_Stop_ResetSM ();
//     gsm_state = gsm_state_shutdown_always;
// }


// unsigned char FuncsGSMReady (void)
// {
//     if (gsm_state == gsm_state_ready)
//         return resp_gsm_ok;
//     else
//         return resp_gsm_error;
// }


// unsigned char FuncsGSMStateAsk (void)
// {
//     return gsm_state;
// }


// void FuncsGSMMessageFlags (unsigned short flag)
// {
//     //veo si es un reset flag
//     if (flag & GSM_RESET_FLAG)
//         GSMFlags &= flag;
//     else			//set flags
//         GSMFlags |= flag;

// }


// unsigned short FuncsGSMMessageFlagsAsk (void)
// {
//     return GSMFlags;
// }


// unsigned char FuncsGSMCommandAnswer (char * pCMD, char * pIMEI)
// {
//     if (gsm_state != gsm_state_ready)
//         return resp_gsm_error;

//     gsm_state = gsm_state_command_answer;
//     p_RESP = pIMEI;
//     p_CMD = pCMD;

//     return resp_gsm_ok;
// }


// //achica el buffer recibido a todos los codigos numeros letras y espacios
// void FuncsGSMParser (unsigned char * orig, unsigned char * dest)
// {
//     while (*orig != '\0')
//     {
//         //todos los codigos numeros y letras        
//         if ((*orig > 31) && (*orig < 127))
//         {
//             *dest = *orig;
//             dest++;
//         }
//         orig++;
//     }

//     *dest = '\0';
// }


// void FuncsGSMTimeoutCounters (void)
// {
    
//     if (funcs_gsm_timeout_timer)
//         funcs_gsm_timeout_timer--;
    
// }

//--- end of file ---//

