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
#include "sms_gprs_data.h"
#include "contact_id.h"
#include "reports.h"


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

// - Externals for comms flags
unsigned short comms_global_flag = 0;
volatile unsigned short keepalive_cnt = 0;


// Globals ---------------------------------------------------------------------


// - Globals for GSM msjs -------
// char gsmNUM [20];
// char gsmMSG [180];
reports_st repo;

// - Globals from timers -------
volatile unsigned short timer_standby = 0;
volatile unsigned short timer_prender_ringing = 0;
volatile unsigned short secs_millis = 0;


// Module Private Functions ----------------------------------------------------
void TimingDelay_Decrement(void);
void ConfigurationChange (void);
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

#if (!defined HARDWARE_VER_1_1) && \
    (!defined HARDWARE_VER_1_0)
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
        timer_rep_conf = 2;
        envios_ok_conf = 0;
        prender_ring_conf = 0;
        battery_check_conf = 0;
        socket_use_enable_conf = 0;
        
        memset(num_tel_imei, '\0', sizeof(num_tel_imei));
        memset(num_tel_prop, '\0', sizeof(num_tel_prop));
        memset(num_tel_rep, '\0', sizeof(num_tel_rep));
        memset(sitio_prop, '\0', sizeof(sitio_prop));

        // strcpy(num_tel_rep, "1141747063");
        // strcpy(num_tel_rep, "");        
        // strcpy(sitio_prop, "Prueba Kirno");
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
    unsigned char answer = 0;    //multi pourpose answer

    // for memory alignment pourpose
    // sprintf(buff, "memory size: %d\n", sizeof(mem_conf));
    // Usart2Debug(buff);
    
    // set the first keepalive
    keepalive_cnt = mem_conf.keepalive;

    while (1)
    {
        switch (main_state)
        {
        case main_init:
            ChangeLed(LED_STANDBY);
            ACT_12V_OFF;
            main_state = main_wait_for_gsm_network;
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
            if (comms_activate_sms_flag)
            {
                comms_activate_sms_flag_reset;
                main_state = main_enable_output;
                Activation_12V_On();    // ACT_12V_ON;
                timer_standby = timer_rep_conf * 1000;
                Usart2Send("ACT_12V ACTIVO\n");
            }

            // activate from phone ringing
            else if ((comms_activate_ringing_flag) &&
                     (prender_ring_conf) &&
                     (!timer_prender_ringing))
            {
                comms_activate_ringing_flag_reset;
                timer_prender_ringing = 12000;
                main_state = main_enable_output;
                Activation_12V_On();    // ACT_12V_ON;
                timer_standby = timer_rep_conf * 1000;
                Usart2Send("ACT_12V ACTIVO\n");
            }

#if (!defined HARDWARE_VER_1_1) && \
    (!defined HARDWARE_VER_1_0)
            // reports from battery status
            if (FuncsGSMStateAsk () == gsm_state_ready)
            {
                // if we are in gprs mode check always
                if (socket_use_enable_conf)
                {                
                    if (comms_battery_low_flag)
                    {
                        ContactIDString(low_system_battery_opening,
                                        mem_conf.client_number,
                                        "000",
                                        buff);

                        comms_battery_low_flag_reset;
                                    
                        repo.buffer = buff;
                        repo.attempts = 3;
                        repo.media_flags = REPORT_BY_IP1 | REPORT_BY_IP2 | REPORT_BY_SMS;
                        main_state = main_report_buffer;
                    }
                    else if (comms_battery_good_flag)
                    {
                        ContactIDString(low_system_battery_close,
                                        mem_conf.client_number,
                                        "000",
                                        buff);
                                    
                        comms_battery_good_flag_reset;

                        repo.buffer = buff;
                        repo.attempts = 3;
                        repo.media_flags = REPORT_BY_IP1 | REPORT_BY_IP2 | REPORT_BY_SMS;
                        main_state = main_report_buffer;
                    }
                }
                // in sms mode check only if its configured
                else if ((battery_check_conf) &&
                         ((comms_battery_low_flag) ||
                          (comms_battery_disconnect_flag)))
                {
                    unsigned char volts_int = 0;
                    unsigned char volts_dec = 0;
                    Battery_Voltage(&volts_int, &volts_dec);
                    
                    if (comms_battery_low_flag)
                    {
                        sprintf(buff, "BAT_LOW: %02d.%02dV", volts_int, volts_dec);
                        comms_battery_low_flag_reset;
                    }
                        
                    if (comms_battery_disconnect_flag)
                    {
                        sprintf(buff, "BAT_DISC: %02d.%02dV", volts_int, volts_dec);
                        comms_battery_disconnect_flag_reset;
                    }

                    // send report by sms
                    repo.buffer = buff;
                    repo.attempts = 3;
                    repo.media_flags = REPORT_BY_SMS;
                    main_state = main_report_buffer;
                }
            }

            // battery measurement
            Battery_Check();
#endif

            // activate from 12V on test1 input or activation by panel
            alarm_input = Check_Alarm_Input();
            panel_input = Panel_Check_Alarm (&remote_number);
            if (alarm_input)
            {
                Activation_12V_On();    // ACT_12V_ON;
                Usart2Send("External 12V Activation!\n");
                main_state = main_report_alarm_input;
            }
            else if (panel_input)
            {
                Activation_12V_On();    // ACT_12V_ON;
                Usart2Send("Panel Internal Activation!\n");
                main_state = main_report_panel_input;
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


            // check if keepalive is enabled, check if gsm is free
            if ((socket_use_enable_conf) &&
                (mem_conf.keepalive) &&
                (!keepalive_cnt) &&
                (FuncsGSMStateAsk() == gsm_state_ready))
            {
                ContactIDString(keep_alive,
                                mem_conf.client_number,
                                "000",
                                buff);

                keepalive_cnt = mem_conf.keepalive;
                                    
                repo.buffer = buff;
                repo.attempts = 3;
                repo.media_flags = REPORT_BY_IP1 | REPORT_BY_IP2;
                main_state = main_report_buffer;
                Usart2Debug("send keepalive msg!!!\n");
            }
            break;

        case main_enable_output:
            if (!timer_standby)
            {
                main_state = main_ready;
                ACT_12V_OFF;
            }
            break;

        case main_report_alarm_input:
            // check if we are going to use gprs mode or sms mode
            if (socket_use_enable_conf)
            {
                // gprs mode, assembly the buffer
                ContactIDString(panic_alarm,
                                mem_conf.client_number,
                                "000",
                                buff);
                                    
                repo.buffer = buff;
                repo.attempts = 3;
                repo.media_flags = REPORT_BY_IP1 | REPORT_BY_IP2 | REPORT_BY_SMS;
            }
            else
            {
                // use only sms mode, assembly buffer
                strcpy(buff, "Activacion en: ");
                strcat(buff, sitio_prop);

                repo.buffer = buff;
                repo.attempts = 3;
                repo.media_flags = REPORT_BY_SMS;
            }

            main_state = main_report_buffer;
            break;

        case main_report_panel_input:
            // check if we are going to use gprs mode or sms mode
            if (socket_use_enable_conf)
            {
                // gprs mode, assembly the buffer
                char remote_number_str [4] = { 0 };                
                sprintf(remote_number_str, "%03d", remote_number);

                ContactIDString(panic_alarm,
                                mem_conf.client_number,
                                remote_number_str,
                                buff);

                repo.buffer = buff;
                repo.attempts = 3;
                repo.media_flags = REPORT_BY_IP1 | REPORT_BY_IP2 | REPORT_BY_SMS;
            }
            else
            {
                // use only sms mode, assembly buffer
                sprintf(buff, "Activo %03d en: ", remote_number);
                strcat(buff, sitio_prop);                

                repo.buffer = buff;
                repo.attempts = 3;
                repo.media_flags = REPORT_BY_SMS;
            }

            main_state = main_report_buffer;
            break;
            

        case main_report_buffer:
            answer = ReportsVerifyAndSend(&repo);
            
            if (answer == REPORT_NOT_SENT)
            {
                main_state = main_report_buffer_not_sended;
                Usart2Send("report not sended\n");
                timer_standby = 6000;    // six seconds to show the error
            }

            if (answer == REPORT_SENT)
            {
                main_state = main_report_buffer_sended;
                Usart2Send("report delivered\n");
                timer_standby = 2000;    // two seconds for show led cycle
            }
            break;

        case main_report_buffer_not_sended:
            UpdateLedActivate();
            if (!timer_standby)
            {
                ACT_12V_OFF;
                main_state = main_ready;
            }            
            break;

        case main_report_buffer_sended:
            ToggleLedActivate();    // sended ok show led cycle
            if (!Check_Alarm_Input() && (!timer_standby))    // sended ok
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
            ConfigurationChange();
        
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
void ConfigurationChange (void)
{
    unsigned char saved_ok = 0;

    if (comms_memory_save_flag)
    {
        comms_memory_save_flag_reset;
        while (!Usart2SendVerifyEmpty());
    
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

    ReportsTimeouts ();

    if (secs_millis < 1000)
        secs_millis++;
    else
    {
        secs_millis = 0;
        if (keepalive_cnt)
            keepalive_cnt--;
    }
    
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

