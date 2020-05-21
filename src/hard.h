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
#define VER_1_1		//esta es la placa kirno_redonda

//-------- Type of Program ----------------
//OJO --- los dos que siguen van juntos
#define USE_GSM
#define USE_REDONDA_BASIC
#define DEBUG_ON


//-------- End Of Defines For Configuration ------

//GPIOA pin0    LED_PWR
#define LED_PWR    ((GPIOA->ODR & 0x0001) != 0)
#define LED_PWR_ON    (GPIOA->BSRR = 0x00000001)
#define LED_PWR_OFF    (GPIOA->BSRR = 0x00010000)

//GPIOA pin1	LED_NET
#define LED_NET    ((GPIOA->ODR & 0x0002) != 0)
#define LED_NET_ON    (GPIOA->BSRR = 0x00000002)
#define LED_NET_OFF    (GPIOA->BSRR = 0x00020000)

//GPIOA pin2
//GPIOA pin3	usart2 tx rx (para debug)
#define PIN3_ON	GPIOA->BSRR = 0x00000008
#define PIN3_OFF GPIOA->BSRR = 0x00080000


//GPIOA pin4
#define NETLIGHT	((GPIOA->IDR & 0x0010) != 0)

//GPIOA pin5
#define STATUS		((GPIOA->IDR & 0x0020) != 0)

//GPIOA pin6	para PWM_CH1

//GPIOA pin7
#define PWRKEY ((GPIOA->ODR & 0x0080) != 0)
#define PWRKEY_ON	GPIOA->BSRR = 0x00000080
#define PWRKEY_OFF GPIOA->BSRR = 0x00800000

//GPIOB pin0 I_Sense

//GPIOB pin1

//GPIOA pin8

//GPIOA pin9
//GPIOA pin10	usart1 tx rx (para el SIM)

//GPIOA pin11
#define RELAY ((GPIOA->ODR & 0x0800) != 0)
#define RELAY_ON	GPIOA->BSRR = 0x00000800
#define RELAY_OFF GPIOA->BSRR = 0x08000000

//GPIOA pin12
#define LED ((GPIOA->ODR & 0x1000) != 0)
#define LED_ON	GPIOA->BSRR = 0x00001000
#define LED_OFF GPIOA->BSRR = 0x10000000

//GPIOA pin13
//GPIOA pin14
//GPIOA pin15

//GPIOB pin3
#define PPS ((GPIOB->IDR & 0x0008) == 0)

//GPIOB pin4
//GPIOB pin5

//GPIOB pin6
//GPIOB pin7	usart1 tx rx (para el GPS)



// Exported Constants  ---------------------------------------------------------
//Estados Externos de LED BLINKING
#define LED_NO_BLINKING               0
#define LED_STANDBY                   1
#define LED_GSM_NETWORK               2
#define LED_ENABLE_OUTPUT             3
#define LED_LOW_VOLTAGE               4


// Exported Types --------------------------------------------------------------
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
void WelcomeCodeFeatures (char *);

#endif /* HARD_H_ */
