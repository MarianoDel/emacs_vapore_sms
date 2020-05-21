//----------------------------------------------------
// #### PROYECTO KIRNO REDONDA GSM - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MAIN.C #######################################
//----------------------------------------------------

// Includes --------------------------------------------------------------------
#include "stm32f0xx.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gpio.h"
#include "tim.h"
#include "uart.h"
#include "hard.h"

#include "core_cm0.h"
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


//--- VARIABLES GLOBALES ---//
unsigned short show_power_index = 0;	//lo uso como timer sincronizado con la medicion, tick 2 secs.
unsigned short show_power_index_debug = 0;

//para los msjs GSM
char gsmNUM [20];
char gsmMSG [180];


// ------- de los timers -------
volatile unsigned short wait_ms_var = 0;
volatile unsigned short timer_standby;
volatile unsigned short tcp_kalive_timer;
//volatile unsigned char display_timer;
volatile unsigned char timer_meas;

#ifdef WITH_HYST
volatile unsigned short secs = 0;
volatile unsigned char hours = 0;
volatile unsigned char minutes = 0;
#endif

#ifdef POWER_MEAS_WITH_SAMPLES

#ifdef POWER_MEAS_WITH_SAMPLES_MA32
#define SIZEOF_POWER_VECT		32			//aunque en realidad es MA32
#define SIZEOF_POWER_VECT_INDEX		10		//este lo dejo igual porque quiere mediciones cada 2 segundos
#endif

#ifdef POWER_MEAS_WITH_SAMPLES_BESTOF_10
#define SIZEOF_POWER_VECT		10
#define SIZEOF_POWER_VECT_INDEX		10
#endif

unsigned short power_vect [SIZEOF_POWER_VECT];
#endif

#ifdef POWER_MEAS_PEAK_TO_PEAK
#define SIZEOF_POWER_VECT		10
unsigned short power_vect [SIZEOF_POWER_VECT];
#endif



// Module Private Functions ----------------------------------------------------
void TimingDelay_Decrement(void);



//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
    unsigned char resp = resp_continue;
    unsigned char need_to_save = 0;
    // unsigned short wh_int, wh_dec;
    // float fcalc = 1.0;
    unsigned short power, last_power;
    unsigned int zero_current_loc = 0;

    unsigned short acum_secs_index;
    unsigned int acum_secs, acum_hours;

    main_state_t main_state = main_init;
    char s_lcd [100];

    //GPIO Configuration.
    GPIO_Config();


    //ACTIVAR SYSTICK TIMER
    if (SysTick_Config(48000))
    {
        while (1)	/* Capture error */
        {
            if (LED)
                LED_OFF;
            else
                LED_ON;

            for (unsigned char i = 0; i < 255; i++)
            {
                asm (	"nop \n\t"
                        "nop \n\t"
                        "nop \n\t" );
            }
        }
    }

    HARD_Initial_Setup();

    //--- Leo los parametros de memoria ---//
#ifdef USE_REDONDA_BASIC
    memcpy(&mem_conf, pmem, sizeof(parameters_typedef));
    if (mem_conf.acumm_wh == 0xFFFFFFFF)
    {
        //memoria vacia
        mem_conf.acumm_wh = 0;
        mem_conf.acumm_w2s = 0;
        mem_conf.acumm_w2s_index = 0;
        mem_conf.timer_reportar = 2;
        mem_conf.timer_pruebas = 10;
        //el timer a reportar esta n minutos, yo tengo tick cada 2 segundos

        strcpy( mem_conf.num_reportar, "1149867843");	//segunda sim de claro
    }
#endif


    //--- Welcome code ---//
    LED_OFF;
//	EN_GPS_OFF;
    EN_GPS_ON;
    //RELAY_ON;
    RELAY_OFF;

    USART1Config();
    USART2Config();

    EXTIOff();



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

        if (LIGHT)
            LED_ON;
        else
            LED_OFF;
    }
#endif
//---------- Fin Prueba con GSM GATEWAY --------//





#ifdef USE_REDONDA_BASIC
//---------- Inicio Programa de Produccion Redonda Basic --------//
    // USART1Config();

    // Prendo todos los Timers
    TIM_3_Init ();					//lo utilizo para 1 a 10V y para synchro ADC
    TIM_16_Init();					//o utilizo para synchro de relay
    TIM16Enable();
    
    Usart2Send("\r\nKirno Placa Redonda - Basic V1.0\r\n");
    Usart2Send("  Features:\r\n");
#ifdef USE_GSM
    Usart2Send("  Uses GSM for SMS data\r\n");
#endif



    for (unsigned char i = 0; i < 8; i++)
    {
        if (LED)
            LED_OFF;
        else
            LED_ON;

        Wait_ms (250);
    }


    timer_standby = 2000;
    FuncsGSMReset();


//--- Programa de Redonda Basic - Produccion - -----

    while (1)
    {
        switch (main_state)
        {
        case main_init:
            break;

        case main_wait_for_gsm_network:
            break;

        case main_ready:
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
        // UpdateRelay ();
        // UpdatePhotoTransistor();
        FuncsGSM();
    }	//end while 1

//---------- Fin Programa de Produccion Redonda Basic--------//
#endif	//USE_REDONDA_BASIC

    return 0;
}

//--- End of Main ---//


void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;

    if (usart1_mini_timeout)
        usart1_mini_timeout--;
    
    if (usart2_mini_timeout)
        usart2_mini_timeout--;

    GSMTimeoutCounters ();
}

//--- end of file ---//

