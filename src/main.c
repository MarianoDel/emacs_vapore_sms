//---------------------------------------------------------
// #### PROYECTO COMUNICADOR VAPORE SMS - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MAIN.C ############################################
//---------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "stm32g0xx.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gpio.h"
#include "tim.h"
#include "usart.h"
#include "hard.h"
#include "comm.h"

#include "core_cm0plus.h"
#include "adc.h"
#include "dma.h"
#include "flash_program.h"

#include "sim900_800.h"
#include "funcs_gsm.h"
#include "funcs_gsm_gateway.h"

#include "test_functions.h"



// Externals -------------------------------------------------------------------
// - Externals from ADC Converter -------
volatile unsigned short adc_ch [ADC_CHANNEL_QUANTITY];

// - Externals de la Memoria y los modos -------
parameters_typedef * pmem = (parameters_typedef *) (unsigned int *) FLASH_ADDRESS_FOR_BKP;	//en flash
parameters_typedef mem_conf;

// - Externals Funcs GSM
unsigned char register_status = 0;
unsigned char rssi_level = 0;


// Globals ---------------------------------------------------------------------
unsigned short show_power_index = 0;	//lo uso como timer sincronizado con la medicion, tick 2 secs.
unsigned short show_power_index_debug = 0;

// - Globals for GSM msjs -------
char gsmNUM [20];
char gsmMSG [180];

// - Globals from timers -------
volatile unsigned short timer_standby = 0;
volatile unsigned short timer_prender_ringing = 0;


// Module Private Functions ----------------------------------------------------
void TimingDelay_Decrement(void);
void ConfigurationChange (void);
void ConfigurationCheck (void);
void SysTickError (void);


//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
    //GPIO Configuration.
    GPIO_Config();

    //Start the SysTick Timer
#ifdef CLOCK_FREQ_64_MHZ
    if (SysTick_Config(64000))
        SysTickError();
#endif
#ifdef CLOCK_FREQ_16_MHZ
    if (SysTick_Config(16000))
        SysTickError();
#endif

    //--- Funciones de Test Hardware ---
    // TF_Led ();
    // TF_Led_Alarm_Input ();
    // TF_Alarm_Input_As_Output ();
    // TF_Led_Alarm_Input_Filtered ();
    // TF_Act_12V ();
    // TF_Led_Blinking();
    // TF_Usart2_TxRx ();
    // TF_Usart2_NetLight_and_Status ();
    // TF_Usart2_Adc_Dma ();
    // TF_Usart2_Flash_Empty_Page ();
    // TF_Usart2_Flash_Write_Data ();
    
    //--- Fin Funciones de Test Hardware ---    

    
    //--- Welcome code ---//
    LED_OFF;
    ACT_12V_OFF;

    Usart1Config();
    Usart2Config();

    EXTIOff();

    // Timers Start Functions
    // TIM_3_Init ();    //lo utilizo para 1 a 10V y para synchro ADC
    // TIM_16_Init();    //o utilizo para synchro de relay
    // TIM16Enable();

    WelcomeCode ();
    FuncsGSMReset ();

    // Backuped Memory Parameters
    memcpy(&mem_conf, pmem, sizeof(parameters_typedef));
    if (mem_conf.acumm_wh == 0xFFFFFFFF)
    {
        //memoria vacia --> Configuracion a Default
        mem_conf.acumm_wh = 0;
        mem_conf.acumm_w2s = 0;
        mem_conf.acumm_w2s_index = 0;
        timer_rep = 2;
        envios_ok = 0;
        prender_ring = 0;
        memset(num_tel_rep, '\0', sizeof(num_tel_rep));
        memset(num_tel_imei, '\0', sizeof(num_tel_imei));
        memset(num_tel_prop, '\0', sizeof(num_tel_prop));
        memset(sitio_prop, '\0', sizeof(sitio_prop));
        strcpy(num_tel_rep, "1141747063");
        strcpy(sitio_prop, "Prueba Kirno");
        //el timer a reportar esta n minutos, yo tengo tick cada 2 segundos
        // strcpy( mem_conf.num_reportar, "1149867843");	//segunda sim de claro
    
        Usart2Send("Memory Empty\n");
    }
    else
        Usart2Send("Memory Have Saved Data\n");

    // Wait_ms(500);
    // TF_ReadMemory ();

//--- Programa de Activacion SMS - Produccion ---
    main_state_t main_state = main_init;
    unsigned char led_rssi_high = 0;

    while (1)
    {
        switch (main_state)
        {
        case main_init:
            ChangeLed(LED_STANDBY);
            ACT_12V_OFF;
            main_state = main_wait_for_gsm_network;

            //reset de flags del gsm
            diag_prender_reset;
            diag_ringing_reset;

            //reset de configuraciones del gsm
            envios_ok_change_reset;
            timer_rep_change_reset;
            prender_ring_change_reset;

            //reset de comunicaciones
            sitio_prop_change_reset;
            num_tel_rep_change_reset;
            break;

        case main_wait_for_gsm_network:
            if (FuncsGSMStateAsk() >= gsm_state_ready)
            {
                main_state = main_ready;
                ChangeLed(LED_GSM_NETWORK_LOW_RSSI);
            }
            break;

        case main_ready:

#ifdef HARDWARE_VER_1_2
            // activate from SMS
            if (diag_prender)
            {
                diag_prender_reset;
                main_state = main_enable_output;
                ACT_12V_ON;
                timer_standby = timer_rep * 1000;
                Usart2Send("ACT_12V ACTIVO\n");
            }

            // activate from phone ringing
            else if ((diag_ringing) &&
                     (prender_ring) &&
                     (!timer_prender_ringing))
            {
                diag_ringing_reset;
                timer_prender_ringing = 12000;
                main_state = main_enable_output;
                ACT_12V_ON;
                timer_standby = timer_rep * 1000;
                Usart2Send("ACT_12V ACTIVO\n");
            }

            // activate from 12V on test1 input
            if (Check_Alarm_Input())
            {
                unsigned char sms_ready = 1;

                Usart2Send("ACT_12V ACTIVO: ");
                //check num_tel_rep before send sms
                if (sms_ready)
                {
                    sms_ready = VerifyNumberString(num_tel_rep);

                    if (!sms_ready)
                    {
                        ChangeLedActivate(1);
                        Usart2Send("sin numero\n");
                    }
                }

                //check sitio_prop before send sms
                if (sms_ready)
                {
                    sms_ready = VerifySiteString(sitio_prop);

                    if (!sms_ready)
                    {
                        ChangeLedActivate(2);
                        Usart2Send("sin sitio\n");
                    }
                }


                if (sms_ready)
                {
                    char buff [SITE_MAX_LEN + 20] = { 0 };

                    ACT_12V_ON;                
                    strcpy(buff, "Activacion en: ");
                    strcat(buff, sitio_prop);
                    if (FuncsGSMSendSMS (buff, num_tel_rep) == resp_gsm_ok)
                    {
                        main_state = main_enable_act_12V_input;
                        sms_ready = 1;
                        Usart2Send("OK\n");
                    }
                    else
                    {
                        sms_ready = 0;
                        ChangeLedActivate(3);
                        Usart2Send("sin red\n");
                    }
                }

                if (!sms_ready)
                {
                    main_state = main_sms_not_sended;
                    timer_standby = 6000;
                }
            }
            
#endif    //HARDWARE_VER_1_2
            
#if (defined HARDWARE_VER_1_0) || (defined HARDWARE_VER_1_1)
#ifdef ACTIVATION_BY_SMS
            // activate from SMS
            if (diag_prender)
            {
                diag_prender_reset;
                main_state = main_enable_output;
                ACT_12V_ON;
                timer_standby = timer_rep * 1000;
                Usart2Send("ACT_12V ACTIVO\n");
            }

            // activate from phone ringing
            else if ((diag_ringing) &&
                     (prender_ring) &&
                     (!timer_prender_ringing))
            {
                diag_ringing_reset;
                timer_prender_ringing = 12000;
                main_state = main_enable_output;
                ACT_12V_ON;
                timer_standby = timer_rep * 1000;
                Usart2Send("ACT_12V ACTIVO\n");
            }

#endif    //ACTIVATION_BY_SMS

#ifdef ACTIVATION_BY_12V_INPUT
#ifdef WITH_PA1_TEST1_INPUT
            // activate from 12V on test1 input
            if (Check_Alarm_Input())
            {
                unsigned char sms_ready = 1;

                Usart2Send("ACT_12V ACTIVO: ");
                //check num_tel_rep before send sms
                if (sms_ready)
                {
                    sms_ready = VerifyNumberString(num_tel_rep);

                    if (!sms_ready)
                    {
                        ChangeLedActivate(1);
                        Usart2Send("sin numero\n");
                    }
                }

                //check sitio_prop before send sms
                if (sms_ready)
                {
                    sms_ready = VerifySiteString(sitio_prop);

                    if (!sms_ready)
                    {
                        ChangeLedActivate(2);
                        Usart2Send("sin sitio\n");
                    }
                }


                if (sms_ready)
                {
                    char buff [80] = { 0 };

                    ACT_12V_ON;                
                    strcpy(buff, "Activacion en: ");
                    strcat(buff, sitio_prop);
                    if (FuncsGSMSendSMS (buff, num_tel_rep) == resp_gsm_ok)
                    {
                        main_state = main_enable_act_12V_input;
                        sms_ready = 1;
                        Usart2Send("OK\n");
                    }
                    else
                    {
                        sms_ready = 0;
                        ChangeLedActivate(3);
                        Usart2Send("sin red\n");
                    }
                }

                if (!sms_ready)
                {
                    main_state = main_sms_not_sended;
                    timer_standby = 6000;
                }
            }
#endif    //WITH_PA1_TEST1_INPUT
#endif    //ACTIVATION_BY_12V_INPUT
#endif    //#if (defined HARDWARE_VER_1_0) || (defined HARDWARE_VER_1_1)

            else if (FuncsGSMStateAsk() < gsm_state_ready)
            {
                main_state = main_wait_for_gsm_network;
                ChangeLed(LED_STANDBY);
            }

            if ((rssi_level > 10) && (!led_rssi_high))
            {
                ChangeLed(LED_GSM_NETWORK_HIGH_RSSI);
                led_rssi_high = 1;
            }

            if ((rssi_level <= 10) && (led_rssi_high))
            {
                ChangeLed(LED_GSM_NETWORK_LOW_RSSI);
                led_rssi_high = 0;
            }

            ConfigurationCheck();
            break;

        case main_enable_output:
            if (!timer_standby)
            {
                main_state = main_ready;
                ACT_12V_OFF;
            }

            ConfigurationCheck();
            break;

        case main_enable_act_12V_input:
            if (!Check_Alarm_Input())
            {
                main_state = main_ready;
                ACT_12V_OFF;
            }

            ConfigurationCheck();
            break;

        case main_sms_not_sended:
            UpdateLedActivate();

            ConfigurationCheck();

            if (!timer_standby)
            {
                ACT_12V_OFF;
                main_state = main_ready;
            }
            
            break;
            
        default:
            main_state = main_init;
            break;
        }

        // The things that do not depend on the program state
        UpdateLed ();
        FuncsGSM ();
        FuncsGSMG_Entering();
        
    }	//end while 1

//--- Fin Programa de Produccion Alarma SMS ---



    return 0;
}

//--- End of Main ---//


void ConfigurationCheck (void)
{
    if (timer_rep_change)
    {
        timer_rep_change_reset;
        ConfigurationChange();
    }            

    if (envios_ok_change)
    {
        envios_ok_change_reset;
        ConfigurationChange();
    }

    if (prender_ring_change)
    {
        prender_ring_change_reset;
        ConfigurationChange();
    }

    if (num_tel_rep_change)
    {
        num_tel_rep_change_reset;
        ConfigurationChange();
    }

    if (sitio_prop_change)
    {
        sitio_prop_change_reset;
        ConfigurationChange();
    }
}


void ConfigurationChange (void)
{
    unsigned char saved_ok = 0;

    __disable_irq();
    saved_ok = Flash_WriteConfigurations((uint32_t *)&mem_conf, sizeof(mem_conf));
    __enable_irq();                
#ifdef DEBUG_ON
    if (saved_ok == FLASH_COMPLETE)
        Usart2Send("Memory Saved OK!\n");
    else
        Usart2Send("Memory Error!!!\n");
#endif
}


void TimingDelay_Decrement(void)
{
    TIM_Timeouts ();

    if (timer_standby)
        timer_standby--;

    if (timer_prender_ringing)
        timer_prender_ringing--;

    USART_Timeouts();

    HARD_Timeouts();
    
    GSMTimeoutCounters ();

    FuncsGSMTimeoutCounters ();

    FuncsGSMG_Timeouts ();
}

void SysTickError (void)
{
    //Capture systick error...
    while (1)
    {
        if (LED)
            LED_OFF;
        else
            LED_ON;

        for (unsigned char i = 0; i < 255; i++)
        {
            asm ("nop \n\t"
                 "nop \n\t"
                 "nop \n\t" );
        }
    }
}

//--- end of file ---//

