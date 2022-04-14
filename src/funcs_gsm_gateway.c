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
#include "adc.h"
#include "parameters.h"

#include <string.h>
#include <stdio.h>


// Externals -------------------------------------------------------------------
parameters_typedef * pmem;
parameters_typedef mem_conf;
extern volatile unsigned short adc_ch [ADC_CHANNEL_QUANTITY];

// Globals ---------------------------------------------------------------------
volatile unsigned short timer_gsm_gw = 0;    //TODO: not in use
unsigned char gw_mode = 0;


// Constants -------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------
void FuncsGSMGateway (void);
void FuncsGSMG_SendStatus (char *);
unsigned char FuncsGSMG_ProcessCommands (char *);
void FuncsGSMG_ShowMemory (parameters_typedef *);
void FuncsGSMG_ShowVoltage (void);
void LedToggle (void);

void FuncsGSMGateway_SM_Reset (void);
unsigned char FuncsGSMGateway_SM (void);



// Module Functions ------------------------------------------------------------
void FuncsGSMG_Entering (void)
{
    char buff [40] = { 0 };

    // check if we need to enter in this mode
    if (!gw_mode)
    {
        if (Usart2HaveData())
        {
            Usart2HaveDataReset();
            Usart2Send("new fucking data\n");
            Usart2ReadBuffer((unsigned char *)buff, sizeof(buff) - 1);
            if (!strncmp(buff, "gsm_gw_mode", sizeof ("gsm_gw_mode") -1))
            {
                gw_mode = 1;
                FuncsGSMGateway_SM_Reset();
            }
        }
    }
    
    if (gw_mode)
    {
        if (FuncsGSMGateway_SM () != 0)    //gw mode continue
            gw_mode = 0;
    }
}


//Procesa toda la pila del GSM (por lo menos para los SMS)
//los comandos que necesita el modulo se envian por otras funciones
#define INIT    0
#define LOOPING    1
unsigned char gw_mode_state = INIT;
void FuncsGSMGateway_SM_Reset (void)
{
    gw_mode_state = 0;
}


unsigned char FuncsGSMGateway_SM (void)
{
    unsigned char gw_out = 0;
    char buff [256] = { 0 };
    
//---------- Pruebas con GSM GATEWAY --------//
    switch (gw_mode_state)
    {
    case INIT:
        Led_Off();
        for (unsigned char i = 0; i < 6; i++)
        {
            if (Led_Status())
                Led_Off();
            else
                Led_On();

            Wait_ms (300);
        }

        Wait_ms (100);
        Usart2Send("GSM GATEWAY.. sin reboot al modulo\r\n");
        Wait_ms (100);
        Usart2Send("GSM GATEWAY Listo para empezar\r\n");
        Wait_ms (100);
        gw_mode_state = LOOPING;
        break;

    case LOOPING:
        if (Usart2HaveData())
        {
            LedToggle();
            Usart2HaveDataReset();
            Usart2ReadBuffer((unsigned char *)buff, sizeof(buff));
            
            //here process the commands from the PC
            unsigned char cmd_type = 0;
            cmd_type = FuncsGSMG_ProcessCommands(buff);
            
            if (cmd_type == 2)    //go out of this mode (gw_mode)
            {
                Usart2Send("Going to SMS Mode...\n");
                gw_out = 1;
            }
            else if (cmd_type == 1)    //own commands
            {
            }
            else if (cmd_type == 0)    //not a command, send it to gsm
            {
                Usart1Send(buff);
                Usart1Send("\r\n");
            }
        }

        if (Usart1HaveData())
        {
            Usart1HaveDataReset();
            Usart1ReadBuffer((unsigned char *)buff, sizeof(buff));
            Usart2Send(buff);
        }
        break;

    default:
        gw_mode_state = INIT;
        break;
    }

    return gw_out;
}


void FuncsGSMGateway (void)
{
//---------- Pruebas con GSM GATEWAY --------//
    Led_Off();
    for (unsigned char i = 0; i < 6; i++)
    {
        if (Led_Status())
            Led_Off();
        else
            Led_On();

        Wait_ms (300);
    }

    Wait_ms (100);
    Usart2Send("GSM GATEWAY.. sin reboot al modulo\r\n");
    Wait_ms (100);
    Usart2Send("GSM GATEWAY Listo para empezar\r\n");
    Wait_ms (100);

    char buff [256] = { 0 };
    unsigned char this_mode = 1;
    while (this_mode)
    {
        if (Usart2HaveData())
        {
            LedToggle();
            Usart2HaveDataReset();
            Usart2ReadBuffer((unsigned char *)buff, sizeof(buff));
            
            //here process the commands from the PC
            unsigned char cmd_type = 0;
            cmd_type = FuncsGSMG_ProcessCommands(buff);
            
            if (cmd_type == 2)    //go out of this mode (gw_mode)
            {
                Usart2Send("Going to SMS Mode...\n");
                this_mode = 0;
            }
            else if (cmd_type == 1)    //own commands
            {
            }
            else if (cmd_type == 0)    //not a command, send it to gsm
            {
                Usart1Send(buff);
            }
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
    sprintf(answers, "getted: %s\n", buff);
    Usart2Send(answers);
    Wait_ms(100);
    
    if (!strncmp(buff, "gsm_status", sizeof ("gsm_status") -1))
    {
        FuncsGSMG_SendStatus(answers);
        result = 1;
    }

    if (!strncmp(buff, "gsm_pwrkey_on", sizeof ("gsm_pwrkey_on") -1))
    {
        Usart2Send("done\n");
        GSM_PWRKEY_ON;
        result = 1;
    }

    if (!strncmp(buff, "gsm_pwrkey_off", sizeof ("gsm_pwrkey_off") -1))
    {
        Usart2Send("done\n");
        GSM_PWRKEY_OFF;
        result = 1;        
    }
    
    if (!strncmp(buff, "gsm_power_off", sizeof ("gsm_power_off") -1))
    {
        unsigned char resp = 0;

        Usart2Send("Stopping GSM...\n");
        GSM_Start_Stop_ResetSM ();
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
        GSM_Start_Stop_ResetSM ();
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

    if (!strncmp(buff, "gsm_bkp_mem", sizeof ("gsm_bkp_mem") -1))
    {
        FuncsGSMG_ShowMemory(pmem);
        result = 1;
    }

    if (!strncmp(buff, "gsm_current_mem", sizeof ("gsm_current_mem") -1))
    {
        FuncsGSMG_ShowMemory(&mem_conf);
        result = 1;
    }

    if (!strncmp(buff, "gsm_current_voltage", sizeof ("gsm_current_voltage") -1))
    {
        FuncsGSMG_ShowVoltage();
        result = 1;
    }
    
    if (!strncmp(buff, "gsm_sms_mode", sizeof ("gsm_sms_mode") -1))
        result = 2;

    return result;
}


void FuncsGSMG_SendStatus (char * buff)
{
    sprintf(buff, "STATUS: %d NETLIGHT: %d PWRKEY: %d\n",
            Status_Status(),
            NetLight_Status(),
            PwrKey_Status());
        
    Usart2Send(buff);
}


void FuncsGSMG_Timeouts (void)
{
    if (timer_gsm_gw)
        timer_gsm_gw--;
}


void FuncsGSMG_ShowMemory (parameters_typedef * pmemory)
{
    char s_send [40] = { 0 };

    // sprintf(s_send, "num_reportar: %s\n", pmemory->num_reportar);
    // Usart2Send(s_send);
    // Wait_ms(100);

    // sprintf(s_send, "imei: %s\n", pmemory->imei);
    // Usart2Send(s_send);
    // Wait_ms(100);

    // sprintf(s_send, "num_propio: %s\n", pmemory->num_propio);
    // Usart2Send(s_send);
    // Wait_ms(100);

    sprintf(s_send, "envios_ok: %d\n", pmemory->bkp_envios_ok);
    Usart2Send(s_send);
    Wait_ms(100);

    sprintf(s_send, "timer_reportar: %d\n", pmemory->bkp_timer_reportar);
    Usart2Send(s_send);
    Wait_ms(100);

    sprintf(s_send, "prender_ring: %d\n", pmemory->bkp_prender_ring);
    Usart2Send(s_send);
    Wait_ms(100);

}


#define K4V    560
#define K12V    183
void FuncsGSMG_ShowVoltage (void)
{
    // char s_send [40] = { 0 };

    // unsigned short vint = V_Sense_4V;
    // sprintf(s_send, "V_Sense4V: %d\n", pmemory->bkp_envios_ok);
    // Usart2Send(s_send);
    // Wait_ms(100);

    // sprintf(s_send, "V_Sense12V: %d\n", pmemory->bkp_timer_reportar);
    // Usart2Send(s_send);
    // Wait_ms(100);
}


void LedToggle (void)
{
    if (Led_Status())
        Led_Off();
    else
        Led_On();
    
}

//--- end of file ---//

