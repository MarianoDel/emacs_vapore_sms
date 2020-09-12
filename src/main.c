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

#include "core_cm0plus.h"
// #include "adc.h"
// #include "dma.h"
#include "flash_program.h"

#include "sim900_800.h"
#include "funcs_gsm.h"



// Externals -------------------------------------------------------------------
// - Externals del Puerto serie  -------
volatile unsigned char tx2buff[SIZEOF_DATA];
volatile unsigned char rx2buff[SIZEOF_DATA];

volatile unsigned char tx1buff[SIZEOF_DATA];
volatile unsigned char rx1buff[SIZEOF_DATA];


// - Externals del GPS & GSM -----------
volatile unsigned char usart1_mini_timeout;
volatile unsigned char usart1_pckt_ready;
volatile unsigned char usart1_have_data;
unsigned char usart1_pckt_bytes;

#define gps_mini_timeout	usart1_mini_timeout
#define gps_pckt_ready		usart1_pckt_ready
#define gps_have_data		usart1_have_data
#define gps_pckt_bytes		usart1_pckt_bytes


// ------- Externals del GSM -------
#if (defined USE_GSM) || (defined USE_GSM_GATEWAY)
#define gsm_mini_timeout	usart1_mini_timeout
#define gsm_pckt_ready		usart1_pckt_ready
#define gsm_have_data		usart1_have_data
#define gsm_pckt_bytes		usart1_pckt_bytes

volatile unsigned char usart2_mini_timeout;
volatile unsigned char usart2_pckt_ready;
volatile unsigned char usart2_have_data;
unsigned char usart2_pckt_bytes;

extern volatile char buffUARTGSMrx2[];
#endif

// ------- Externals de la Memoria y los modos -------
parameters_typedef * pmem = (parameters_typedef *) (unsigned int *) FLASH_PAGE_FOR_BKP;	//en flash
parameters_typedef mem_conf;

// ------- Externals para el LED state --------
volatile unsigned short timer_led;

// --- Externals Funcs GSM
unsigned char register_status = 0;
unsigned char rssi_level = 0;


// Globals ---------------------------------------------------------------------
unsigned short show_power_index = 0;	//lo uso como timer sincronizado con la medicion, tick 2 secs.
unsigned short show_power_index_debug = 0;

//para los msjs GSM
char gsmNUM [20];
char gsmMSG [180];


// ------- de los timers -------
volatile unsigned short wait_ms_var = 0;
volatile unsigned short timer_standby = 0;
volatile unsigned short timer_prender_ringing = 0;
volatile unsigned short tcp_kalive_timer;
//volatile unsigned char display_timer;
volatile unsigned char timer_meas;



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
    main_state_t main_state = main_init;
    char s_lcd [100];

    //GPIO Configuration.
    GPIO_Config();

    //ACTIVAR SYSTICK TIMER
#ifdef CLOCK_FREQ_64_MHZ
    if (SysTick_Config(64000))
        SysTickError();
#endif
#ifdef CLOCK_FREQ_16_MHZ
    if (SysTick_Config(16000))
        SysTickError();
#endif

    //--- Welcome code ---//
    LED_OFF;
    RELAY_OFF;

    Usart1Config();
    Usart2Config();

    EXTIOff();

    // Prendo todos los Timers
    TIM_3_Init ();    //lo utilizo para 1 a 10V y para synchro ADC
    // TIM_16_Init();    //o utilizo para synchro de relay
    // TIM16Enable();

    WelcomeCodeFeatures(s_lcd);
    
    FuncsGSMReset();

    //--- Leo los parametros de memoria ---//
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
        //el timer a reportar esta n minutos, yo tengo tick cada 2 segundos
        // strcpy( mem_conf.num_reportar, "1149867843");	//segunda sim de claro
    
        Usart2Send("Memory Empty\n");
    }
    else
        Usart2Send("Memory Have Saved Data\n");


//--- Programa de Redonda Basic - Produccion ---
    unsigned char led_rssi_high = 0;

    while (1)
    {
        switch (main_state)
        {
        case main_init:
            ChangeLed(LED_STANDBY);
            RELAY_OFF;
            main_state = main_wait_for_gsm_network;

            //reset de flags del gsm
            diag_prender_reset;
            diag_ringing_reset;

            //reset de configuraciones del gsm
            envios_ok_change_reset;
            timer_rep_change_reset;
            prender_ring_change_reset;
            break;

        case main_wait_for_gsm_network:
            if (FuncsGSMStateAsk() >= gsm_state_ready)
            {
                main_state = main_ready;
                ChangeLed(LED_GSM_NETWORK_LOW_RSSI);
            }
            break;

        case main_ready:
            if (diag_prender)
            {
                diag_prender_reset;
                main_state = main_enable_output;
                RELAY_ON;
                timer_standby = timer_rep * 1000;
                Usart2Send("RELAY ACTIVO\n");
            }

            if ((diag_ringing) &&
                (prender_ring) &&
                (!timer_prender_ringing))
            {
                diag_ringing_reset;
                timer_prender_ringing = 12000;
                main_state = main_enable_output;
                RELAY_ON;
                timer_standby = timer_rep * 1000;
                Usart2Send("RELAY ACTIVO\n");
            }

            if (FuncsGSMStateAsk() < gsm_state_ready)
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
                RELAY_OFF;
            }

            ConfigurationCheck();
            break;
            
        // case LAMP_OFF:
        //     Usart2Send("PRENDIDO\r\n");
        //     FuncsGSMSendSMS("PRENDIDO", mem_conf.num_reportar);
        //     LED_ON;
        //     break;


        default:
            main_state = main_init;
            break;
        }

        //Cosas que no dependen del estado del programa
        UpdateLed();
        // UpdateRelay ();
        // UpdatePhotoTransistor();
        FuncsGSM();
    }	//end while 1

//---------- Fin Programa de Produccion Redonda Basic--------//


//---------- Pruebas con GSM GATEWAY --------//
#ifdef USE_GSM_GATEWAY
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
    timer_standby = 60000;		//doy 1 minuto para prender modulo
    unsigned char i = 0;
    while (timer_standby)
    {
        i = GSM_Start();
        if (i == 1)
        {
            Usart2Send("Start OK\r\n");
            timer_standby = 0;
        }

        if (i > 1)
        {
            Usart2Send("Start NOK\r\n");
            Usart2Send("Please reboot!\r\n");
        }
    }

    Usart2Send("GSM GATEWAY Listo para empezar\r\n");

    while (1)
    {
        GSMProcess();

        if (usart2_pckt_ready)	//deja paquete en buffUARTGSMrx2
        {
            usart2_pckt_ready = 0;
            Usart1SendUnsigned((unsigned char *) buffUARTGSMrx2, usart2_pckt_bytes);
        }

        if (gsm_pckt_ready)		//deja paquete en buffUARTGSMrx2
        {
            gsm_pckt_ready = 0;
            Usart2SendUnsigned((unsigned char *) buffUARTGSMrx2, gsm_pckt_bytes);
        }

    }
#endif
//---------- Fin Prueba con GSM GATEWAY --------//

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
}
    
void ConfigurationChange (void)
{
    unsigned char saved_ok = 0;

    __disable_irq();
    saved_ok = Flash_WriteConfigurations((uint32_t *)&mem_conf, sizeof(mem_conf));
    __enable_irq();                
#ifdef DEBUG_ON
    if (saved_ok)
        Usart2Send("Memory Saved OK!\n");
    else
        Usart2Send("Memory Error!!!\n");
#endif
}

void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;

    if (timer_prender_ringing)
        timer_prender_ringing--;
    
    if (usart1_mini_timeout)
        usart1_mini_timeout--;
    
    if (usart2_mini_timeout)
        usart2_mini_timeout--;

    if (timer_led)
        timer_led--;
    
    GSMTimeoutCounters ();

    FuncsGSMTimeoutCounters ();
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

