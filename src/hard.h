//----------------------------------------------------
// #### PROYECTO KIRNO REDONDA GSM - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### HARD.H #######################################
//----------------------------------------------------

// Define to prevent recursive inclusion ---------------------------------------
#ifndef HARD_H_
#define HARD_H_


//----------- Defines For Configuration -------------
// -- select the following only on HARDWARE_VER_1_1 or HARDWARE_VER_1_0
// #define WITH_PA1_V4V_SENSE    //normal mode
// #define WITH_PA1_TEST1_INPUT    //change in hardware test1 is re-routed to PA1 for 12V inputs signals
// -- select type of gsm_gateway function, in loop or as state machine
#define USE_GSM_GATEWAY_IN_LOOP
// #define USE_GSM_GATEWAY_SM

//----------- Hardware Board Version -------------
// #define HARDWARE_VER_2_0    // sim800l daughter board design
#define HARDWARE_VER_1_2    // all old hard changes included
// #define HARDWARE_VER_1_1    // test1 input modified to report alarms 4V analog to digital input
                               // pin5 on micro must be gnd (its disconnected)
// #define HARDWARE_VER_1_0    // original version

//----------- Firmware Version -------------------
#define FIRMWARE_VER_1_5    // fix gsm module bad answers and reboot
// #define FIRMWARE_VER_1_4    // can send battery reports by sms
// #define FIRMWARE_VER_1_3    // delay sms message
// #define FIRMWARE_VER_1_2    // all programs in one

// -- select the debug level
#define DEBUG_ON


//-------- Clock Frequency ------------------------------------
#define CLOCK_FREQ_64_MHZ    //ojo modificar a mano system_stm32g0xx.c SystemInit
// #define CLOCK_FREQ_16_MHZ    //ojo modificar a mano system_stm32g0xx.c SystemInit

//-------- End Of Defines For Configuration ------


#ifdef HARDWARE_VER_2_0
#define HARD    "Hardware version 2.0\n"
#endif
#ifdef HARDWARE_VER_1_2
#define HARD    "Hardware version 1.2\n"
#endif
#ifdef HARDWARE_VER_1_1
#define HARD    "Hardware version 1.1\n"
#endif
#ifdef HARDWARE_VER_1_0
#define HARD    "Hardware version 1.0\n"
#endif

#ifdef FIRMWARE_VER_1_5
#define SOFT    "Firmware version 1.5\n"
#endif

#ifdef FIRMWARE_VER_1_4
#define SOFT    "Firmware version 1.4\n"
#endif

#ifdef FIRMWARE_VER_1_3
#define SOFT    "Firmware version 1.3\n"
#endif

#ifdef FIRMWARE_VER_1_2
#define SOFT    "Firmware version 1.2\n"
#endif

//--------- Sanity Checks ----------
#if (!defined HARDWARE_VER_2_0) && \
    (!defined HARDWARE_VER_1_2) && \
    (!defined HARDWARE_VER_1_1) && \
    (!defined HARDWARE_VER_1_0)
#error "define hardware version on hard.h"
#endif

#if (!defined FIRMWARE_VER_1_5) && \
    (!defined FIRMWARE_VER_1_4) && \
    (!defined FIRMWARE_VER_1_3) && \
    (!defined FIRMWARE_VER_1_2)
#error "define firmware version on hard.h"
#endif


#if (defined HARDWARE_VER_1_1) || (defined HARDWARE_VER_1_0)

#if (defined WITH_PA1_TEST1_INPUT) && (defined WITH_PA1_V4V_SENSE)
#error "conflicting inputs selection on hard.h"
#endif

#if (!defined WITH_PA1_TEST1_INPUT) && (!defined WITH_PA1_V4V_SENSE)
#error "no inputs selection on hard.h"
#endif

#if (defined FIRMWARE_VER_1_5) || \
    (defined FIRMWARE_VER_1_4) || \
    (defined FIRMWARE_VER_1_3) || \
    (defined FIRMWARE_VER_1_2)
#if (!defined WITH_PA1_TEST1_INPUT)
#error "no input for 12V activation on hard.h"
#endif
#endif

#endif    // HARDWARE_VER_1_1 or HARDWARE_VER_1_0


// Gpios Configuration ------------------------------
#ifdef HARDWARE_VER_2_0
//GPIOA pin0    nc

//GPIOA pin1	V_Sense_4V

//GPIOA pin2    usart2 tx
//GPIOA pin3	usart2 rx (para debug)

//GPIOA pin4
#define DIO0    ((GPIOA->IDR & 0x0010) != 0)

//GPIOA pin5    Spi1 SCK
//GPIOA pin6	Spi1 MISO 
//GPIOA pin7    Spi1 MOSI

//GPIOB pin0
#define DIO1    ((GPIOB->IDR & 0x0001) != 0)

//GPIOB pin1   
#define PWRKEY    ((GPIOB->ODR & 0x0002) != 0)
#define PWRKEY_ON    (GPIOB->BSRR = 0x00000002)
#define PWRKEY_OFF    (GPIOB->BSRR = 0x00020000)

//GPIOB pin2    V_Sense_12V

//GPIOA pin8    nc

//GPIOA pin9    usart1 tx (para el SIM)

//GPIOC pin6    nc

//GPIOA pin10	usart1 rx (para el SIM)

//GPIOA pin11
#define ACT_12V    ((GPIOA->ODR & 0x0800) != 0)
#define ACT_12V_ON    (GPIOA->BSRR = 0x00000800)
#define ACT_12V_OFF    (GPIOA->BSRR = 0x08000000)

//GPIOA pin12
#define LED    ((GPIOA->ODR & 0x1000) != 0)
#define LED_ON    (GPIOA->BSRR = 0x00001000)
#define LED_OFF    (GPIOA->BSRR = 0x10000000)

//GPIOA pin13    swdio
//GPIOA pin14    swclk
//GPIOA pin15    nc

//GPIOB pin3    
//GPIOB pin4
//GPIOB pin5    nc

//GPIOB pin6
#define ALARM_INPUT    ((GPIOB->IDR & 0x0040) != 0)

//GPIOB pin7    test2
//GPIOB pin8    test3
//GPIOB pin9    nc
#endif     //#ifdef HARDWARE_VER_2_0

#ifdef HARDWARE_VER_1_2
//GPIOA pin0    nc

//GPIOA pin1	V_Sense_4V

//GPIOA pin2    usart2 tx
//GPIOA pin3	usart2 rx (para debug)

//GPIOA pin4
//GPIOA pin5
//GPIOA pin6	nc

//GPIOA pin7
#define PWRKEY    ((GPIOA->ODR & 0x0080) != 0)
#define PWRKEY_ON    (GPIOA->BSRR = 0x00000080)
#define PWRKEY_OFF    (GPIOA->BSRR = 0x00800000)

//GPIOB pin0 
//GPIOB pin1    nc

//GPIOB pin2    V_Sense_12V

//GPIOA pin8    nc

//GPIOA pin9    usart1 tx (para el SIM)

//GPIOC pin6    nc

//GPIOA pin10	usart1 rx (para el SIM)

//GPIOA pin11
#define ACT_12V    ((GPIOA->ODR & 0x0800) != 0)
#define ACT_12V_ON    (GPIOA->BSRR = 0x00000800)
#define ACT_12V_OFF    (GPIOA->BSRR = 0x08000000)

//GPIOA pin12
#define LED    ((GPIOA->ODR & 0x1000) != 0)
#define LED_ON    (GPIOA->BSRR = 0x00001000)
#define LED_OFF    (GPIOA->BSRR = 0x10000000)

//GPIOA pin13    swdio
//GPIOA pin14    swclk
//GPIOA pin15    nc

//GPIOB pin3    nc

//GPIOB pin4
#define STATUS    ((GPIOB->IDR & 0x0010) != 0)

//GPIOB pin5
#define NETLIGHT    ((GPIOB->IDR & 0x0020) != 0)

//GPIOB pin6
#define ALARM_INPUT    ((GPIOB->IDR & 0x0040) != 0)

//GPIOB pin7    test2
//GPIOB pin8    test3
//GPIOB pin9    nc
#endif     //#ifdef HARDWARE_VER_1_2

#if (defined HARDWARE_VER_1_1) || (defined HARDWARE_VER_1_0)
//GPIOB pin9    nc

//GPIOA pin0    nc

#ifdef WITH_PA1_V4V_SENSE
//GPIOA pin1	V_Sense_4V
#endif
#ifdef WITH_PA1_TEST1_INPUT
//GPIOA pin1	re-routed from test1
#define ALARM_INPUT    ((GPIOA->IDR & 0x0002) != 0)
// #define ALARM_ON    (GPIOA->BSRR = 0x00000002)
// #define ALARM_OFF    (GPIOA->BSRR = 0x00020000)

#endif

//GPIOA pin2    usart2 tx
//GPIOA pin3	usart2 rx (para debug)

//GPIOA pin4
//GPIOA pin5
//GPIOA pin6	nc

//GPIOA pin7
#define PWRKEY    ((GPIOA->ODR & 0x0080) != 0)
#define PWRKEY_ON    (GPIOA->BSRR = 0x00000080)
#define PWRKEY_OFF    (GPIOA->BSRR = 0x00800000)

//GPIOB pin0 
//GPIOB pin1    nc

//GPIOB pin1    V_Sense_12V

//GPIOA pin8    nc

//GPIOA pin9    usart1 tx (para el SIM)

//GPIOC pin6    nc

//GPIOA pin10	usart1 rx (para el SIM)

//GPIOA pin11
#define ACT_12V    ((GPIOA->ODR & 0x0800) != 0)
#define ACT_12V_ON    (GPIOA->BSRR = 0x00000800)
#define ACT_12V_OFF    (GPIOA->BSRR = 0x08000000)

//GPIOA pin12
#define LED    ((GPIOA->ODR & 0x1000) != 0)
#define LED_ON    (GPIOA->BSRR = 0x00001000)
#define LED_OFF    (GPIOA->BSRR = 0x10000000)

//GPIOA pin13    swdio
//GPIOA pin14    swclk
//GPIOA pin15    nc

//GPIOB pin3    nc

//GPIOB pin4
#define STATUS    ((GPIOB->IDR & 0x0010) != 0)

//GPIOB pin5
#define NETLIGHT    ((GPIOB->IDR & 0x0020) != 0)

//GPIOB pin6    test1
//GPIOB pin7    test2
//GPIOB pin8    test3
#endif     //#if (defined HARDWARE_VER_1_0) || (defined HARDWARE_VER_1_1)


// Exported Types Constants & Macros  ------------------------------------------
//Estados Externos de LED BLINKING
#define LED_NO_BLINKING               0
#define LED_STANDBY                   1
#define LED_GSM_NETWORK_LOW_RSSI      2
#define LED_GSM_NETWORK_HIGH_RSSI     3
#define LED_LOW_VOLTAGE               4
#define LED_GSM_CMD_ERRORS            20

typedef enum
{
    main_init = 0,
    main_wait_for_gsm_network,
    main_ready,
    main_report_alarm_input_or_panel,
    main_report_alarm_by_gprs,
    main_report_alarm_by_sms,
    main_enable_output,
    main_enable_act_12V_input,
    main_sms_not_sended

} main_state_t;

typedef enum {
    resp_ok = 0,
    resp_continue,
    resp_selected,
    resp_change,
    resp_change_all_up,
    resp_working,
    resp_error,
    resp_need_to_save,
    resp_finish,
    resp_nok,
    resp_timeout,
    resp_ready,
    resp_no_answer,
    resp_save

} resp_t;


//--- Stringtify Utils -----------------------
#define str_macro(s) #s


// Module Exported Functions ---------------------------------------------------
void ChangeLed (unsigned char);
void UpdateLed (void);

void ChangeLedActivate (unsigned char how_many);
void UpdateLedActivate (void);
void ToggleLedActivate (void);

void Activation_12V_On (void);
void Activation_12V_Off (void);

void WelcomeCode (void);
void HARD_Timeouts (void);
unsigned char Check_Alarm_Input (void);

void Led_Off (void);
void Led_On (void);
unsigned char Led_Status (void);

unsigned char Status_Status (void);
void PwrKey_On (void);
void PwrKey_Off (void);
unsigned char PwrKey_Status (void);
unsigned char NetLight_Status (void);

#endif /* HARD_H_ */
