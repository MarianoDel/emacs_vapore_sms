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

//----------- Hardware Board Version -------------
#define HARD    "Hardware version 1.0"
#define SOFT    "Firmware version 1.0"

//-------- Type of Program ----------------
//OJO --- los dos que siguen van juntos
#define USE_GSM
#define DEBUG_ON
#define USE_REDONDA_BASIC
// #define USE_GSM_GATEWAY

//-------- Clock Frequency ------------------------------------
#define CLOCK_FREQ_64_MHZ    //ojo modificar a mano system_stm32g0xx.c SystemInit
// #define CLOCK_FREQ_16_MHZ    //ojo modificar a mano system_stm32g0xx.c SystemInit

//-------- End Of Defines For Configuration ------

// Gpios Configuration ------------------------------
//GPIOB pin9    nc

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


// Exported Types Constants & Macros  ------------------------------------------
//Estados Externos de LED BLINKING
#define LED_NO_BLINKING               0
#define LED_STANDBY                   1
#define LED_GSM_NETWORK_LOW_RSSI      2
#define LED_GSM_NETWORK_HIGH_RSSI     3
#define LED_LOW_VOLTAGE               4

typedef enum
{
    main_init = 0,
    main_wait_for_gsm_network,
    main_ready,
    main_enable_output

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
void WelcomeCode (void);

#endif /* HARD_H_ */
