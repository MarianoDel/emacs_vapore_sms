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
void FuncsGSMGateway (void);
void FuncsGSMG_SendStatus (char *);
unsigned char FuncsGSMG_ProcessCommands (char *);


// Module Functions ------------------------------------------------------------
void FuncsGSMG_Entering (void)
{
    char buff [40] = { 0 };

    // check if we need to enter in this mode
    if (Usart2HaveData())
    {
        Usart2HaveDataReset();
        Usart2ReadBuffer((unsigned char *)buff, sizeof(buff));
        if (!strncmp(buff, "gsm_gw_mode", sizeof ("gsm_gw_mode") -1))
        {
            FuncsGSMGateway ();
        }
    }
}


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
    unsigned char this_mode = 1;
    while (this_mode)
    {
        if (Usart2HaveData())
        {
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
            else if (cmd_type == 1)
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

    if (!strncmp(buff, "gsm_sms_mode", sizeof ("gsm_sms_mode") -1))
        result = 2;

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


//--- end of file ---//

