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
#include "comms_from_panel.h"
#include "parameters.h"

#include "core_cm0plus.h"
#include "adc.h"
#include "dma.h"
#include "flash_program.h"

#include "sim900_800.h"
#include "funcs_gsm.h"
#include "funcs_gsm_gateway.h"

#include "test_functions.h"
#include "battery.h"
#include "sms_data.h"



// Externals -------------------------------------------------------------------
// - Externals from ADC Converter -------
volatile unsigned short adc_ch [ADC_CHANNEL_QUANTITY];

// - Externals de la Memoria y los modos -------
parameters_typedef * pmem = (parameters_typedef *) (unsigned int *) FLASH_ADDRESS_FOR_BKP;	//en flash
parameters_typedef mem_conf;

// - Externals Funcs GSM
unsigned char register_status = 0;
unsigned char rssi_level = 0;
#define LED_RSSI_LOW    0
#define LED_RSSI_HIGH    1
#define LED_RSSI_CMD_ERRORS    2            


// Globals ---------------------------------------------------------------------
unsigned short show_power_index = 0;	//lo uso como timer sincronizado con la medicion, tick 2 secs.
unsigned short show_power_index_debug = 0;

// - Globals for GSM msjs -------
char gsmNUM [20];
char gsmMSG [180];

// - Globals from timers -------
volatile unsigned short timer_standby = 0;
volatile unsigned short timer_prender_ringing = 0;

// - Globals for delay msgs -------
unsigned char message_delay = 0;

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
    // TF_Hardware_Tests ();
    
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

#if (defined FIRMWARE_VER_1_5) || \
    (defined FIRMWARE_VER_1_4)
    //-- ADC Init
    AdcConfig();

    //-- DMA configuration and Init
    DMAConfig();
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    ADC1->CR |= ADC_CR_ADSTART;
    Wait_ms(5);    // wait a few voltage samples
    Battery_Check_Init();
#endif

    WelcomeCode ();
    FuncsGSMReset ();

    // Backuped Memory Parameters
    memcpy(&mem_conf, pmem, sizeof(parameters_typedef));
    if (mem_conf.memory_saved_flag != 0)
    {
        //memoria vacia --> Configuracion a Default
        mem_conf.memory_saved_flag = 0;
        timer_rep = 2;
        envios_ok = 0;
        prender_ring = 0;
        battery_check = 0;
        memset(num_tel_rep, '\0', sizeof(num_tel_rep));
        memset(num_tel_imei, '\0', sizeof(num_tel_imei));
        memset(num_tel_prop, '\0', sizeof(num_tel_prop));
        memset(sitio_prop, '\0', sizeof(sitio_prop));
        // strcpy(num_tel_rep, "1141747063");
        strcpy(num_tel_rep, "");        
        strcpy(sitio_prop, "Prueba Kirno");
        //el timer a reportar esta n minutos, yo tengo tick cada 2 segundos
        // strcpy( mem_conf.num_reportar, "1149867843");	//segunda sim de claro
    
        Usart2Send("Memory Empty\n");
    }
    else
        Usart2Send("Memory Have Saved Data\n");

    
//--- Programa de Activacion SMS - Produccion ---
    main_state_t main_state = main_init;
    unsigned char led_rssi_status = 0;

    unsigned char alarm_input = 0;
    unsigned char panel_input = 0;
    unsigned short remote_number = 0;

    static char buff [SITE_MAX_LEN + 20] = { 0 };
    unsigned char sms_not_sent_cnt = 0;
    sms_pckt_t sms_info;
    unsigned char answer = 0;    //multi pourpose answer

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
            diag_battery_reset;
            diag_battery_low_voltage_reset;
            diag_battery_disconnect_voltage_reset;

            //reset de configuraciones del gsm
            // see ConfigurationCheck() for all the following
            timer_rep_change_reset;
            envios_ok_change_reset;
            prender_ring_change_reset;
            num_tel_rep_change_reset;
            sitio_prop_change_reset;
            battery_check_change_reset;
            
            break;

        case main_wait_for_gsm_network:
            if (FuncsGSMStateAsk() >= gsm_state_ready)
            {
                main_state = main_ready;
                ChangeLed(LED_GSM_NETWORK_LOW_RSSI);
            }
            break;

        case main_ready:

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

#if (defined FIRMWARE_VER_1_5) || \
    (defined FIRMWARE_VER_1_4)
            // reports from battery status
            if ((battery_check) &&
                (FuncsGSMStateAsk () == gsm_state_ready))
            {
                if ((diag_battery_low_voltage) ||
                    (diag_battery_disconnect_voltage))
                {
                    if (VerifyNumberString(num_tel_rep))
                    {
                        unsigned char volts_int = 0;
                        unsigned char volts_dec = 0;
                        Battery_Voltage(&volts_int, &volts_dec);
                        if (diag_battery_low_voltage)
                        {
                            sprintf(buff, "BAT_LOW: %02d.%02dV", volts_int, volts_dec);
                            diag_battery_low_voltage_reset;
                        }
                        
                        if (diag_battery_disconnect_voltage)
                        {
                            sprintf(buff, "BAT_DISC: %02d.%02dV", volts_int, volts_dec);
                            diag_battery_disconnect_voltage_reset;
                        }
                        
                        if (FuncsGSMSendSMS (buff, num_tel_rep) == resp_gsm_ok)
                        {
                            Usart2Debug(buff);
                            Usart2Debug(" -> Sended OK!\n");
                        }
                        else    // cant send
                            Usart2Debug("Battery report not sended!\n");
                    }
                    else
                        Usart2Debug("sin numero grabado para reportes\n");
                }
            }

            // battery measurement
            Battery_Check();
#endif

            // activate from 12V on test1 input or activation by panel
            alarm_input = Check_Alarm_Input();
            panel_input = Panel_Check_Alarm (&remote_number);
            if ((alarm_input) || (panel_input))
            {
                main_state = main_report_alarm_input_or_panel;
            }
            else if (FuncsGSMStateAsk() < gsm_state_ready)
            {
                main_state = main_wait_for_gsm_network;
                ChangeLed(LED_STANDBY);
            }
            else if (rssi_level == 0xff)    // some cmd problems!
            {
                // show commands errors on led
                if (led_rssi_status != LED_RSSI_CMD_ERRORS)
                {
                    ChangeLed(LED_GSM_CMD_ERRORS);
                    led_rssi_status = LED_RSSI_CMD_ERRORS;
                }
            }
            else
            {
                if ((rssi_level > 10) && (led_rssi_status != LED_RSSI_HIGH))
                {
                    ChangeLed(LED_GSM_NETWORK_HIGH_RSSI);
                    led_rssi_status = LED_RSSI_HIGH;
                }

                if ((rssi_level <= 10) && (led_rssi_status != LED_RSSI_LOW))
                {
                    ChangeLed(LED_GSM_NETWORK_LOW_RSSI);
                    led_rssi_status = LED_RSSI_LOW;
                }
            }
            break;

        case main_enable_output:
            if (!timer_standby)
            {
                main_state = main_ready;
                ACT_12V_OFF;
            }
            break;

        case main_report_alarm_input_or_panel:
            // answer = FuncsGSMSendGPRS();

            // if (answer == resp_gsm_ok)
            // {
            //     Usart2Send("connection ok\n");
            //     main_state = main_enable_act_12V_input;                
            // }
            // else if (answer > resp_gsm_ok)
            // {
            //     Usart2Send("connection fail!!!\n");
            //     main_state = main_sms_not_sended;
            //     timer_standby = 6000;
            // }
                
                

            // check if gprs is needed

            // or go directly to sms
            main_state = main_report_alarm_by_sms;
            sms_not_sent_cnt = 5;

            // sms assemble packet
            sms_info.alarm_input = alarm_input;
            sms_info.panel_input = panel_input;
            sms_info.remote_number = remote_number;
            sms_info.buff = buff;

            timer_standby = 0;

            break;

        case main_report_alarm_by_gprs:            
            // check data to send a GPRS packet
            // answer = VerifyAndSendGPRS();

            // if (answer == GPRS_NOT_SENDED)
            // {
            //     //TODO: another attempt here?
            //     main_state = main_report_alarm_by_sms;
            // }
            break;

        case main_report_alarm_by_sms:
            if (timer_standby)
                break;
            
            // check data and send a GSM packet
            answer = VerifyAndSendSMS (&sms_info);

            if (answer == SMS_NOT_SEND)
            {
                if (sms_not_sent_cnt)
                {
                    sms_not_sent_cnt--;
                    timer_standby = 1000;
                }
                else
                {
                    main_state = main_sms_not_sended;
                    timer_standby = 6000;
                }
            }
            else if (SMS_NOT_PROPER_DATA)
            {
                main_state = main_sms_not_sended;
                timer_standby = 6000;
            }
            else    //SMS_SENT
            {
                main_state = main_enable_act_12V_input;
                Usart2Send("OK\n");
                if (panel_input)
                    timer_standby = 1000;
            }
            break;

        case main_enable_act_12V_input:
            if (!Check_Alarm_Input() && (!timer_standby))
            {
                ACT_12V_OFF;
                main_state = main_ready;
            }
            break;

        case main_sms_not_sended:
            UpdateLedActivate();

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

        // check configuration changes and the need for a memory save
        if (main_state >= main_ready)
            ConfigurationCheck();
        
        // The things that do not depend on the program state
        UpdateLed ();
        FuncsGSM ();
        FuncsGSMG_Entering();    //check config entry and clean the usart2 buffer
        
    }	//end while 1

//--- Fin Programa de Produccion Alarma SMS ---



    return 0;
}

//--- End of Main ---//


// Module Functions ------------------------------------------------------------
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
    
    if (battery_check_change)
    {
        battery_check_change_reset;
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

    // FuncsGSMG_Timeouts ();
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

