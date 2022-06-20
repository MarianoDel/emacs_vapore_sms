//----------------------------------------------------
// #### PROYECTO KIRNO REDONDA GSM - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### HARD.C #######################################
//----------------------------------------------------

// Includes --------------------------------------------------------------------
#include "hard.h"
#include "tim.h"
#include "stm32g0xx.h"
#include "usart.h"

#include <stdio.h>

// Module Private Types Constants and Macros -----------------------------------
// led states
typedef enum
{    
    START_BLINKING = 0,
    WAIT_TO_OFF,
    WAIT_TO_ON,
    WAIT_NEW_CYCLE
    
} led_state_t;

// Configs
#define LED_BLINK_ON    LED_ON
#define LED_BLINK_OFF    LED_OFF
#define TIMER_BLINK    200
#define TIMER_BLINK_NEXT_CYCLE    2000


#define SWITCHES_ROOF    80
#define SWITCHES_THRESHOLD_MIN	50


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
// for the led
volatile unsigned short timer_led;
led_state_t led_state = START_BLINKING;
unsigned char blink = 0;
unsigned char how_many_blinks = 0;

// for the led_activate
volatile unsigned short timer_led_activate;
led_state_t led_activate_state = START_BLINKING;
unsigned char blink_activate = 0;
unsigned char how_many_blinks_activate = 0;

volatile unsigned char s_alarm_input_cntr = 0;



// Module Functions ------------------------------------------------------------
//cambia configuracion de bips del LED
void ChangeLed (unsigned char how_many)
{
    how_many_blinks = how_many;
    led_state = START_BLINKING;
}

//mueve el LED segun el estado del Pote
void UpdateLed (void)
{
    switch (led_state)
    {
        case START_BLINKING:
            blink = how_many_blinks;
            
            if (blink)
            {
                LED_BLINK_ON;
                timer_led = TIMER_BLINK;
                led_state++;
                blink--;
            }
            break;

        case WAIT_TO_OFF:
            if (!timer_led)
            {
                LED_BLINK_OFF;
                timer_led = TIMER_BLINK;
                led_state++;
            }
            break;

        case WAIT_TO_ON:
            if (!timer_led)
            {
                if (blink)
                {
                    blink--;
                    timer_led = TIMER_BLINK;
                    led_state = WAIT_TO_OFF;
                    LED_BLINK_ON;
                }
                else
                {
                    led_state = WAIT_NEW_CYCLE;
                    timer_led = TIMER_BLINK_NEXT_CYCLE;
                }
            }
            break;

        case WAIT_NEW_CYCLE:
            if (!timer_led)
                led_state = START_BLINKING;

            break;

        default:
            led_state = START_BLINKING;
            break;
    }
}


void ChangeLedActivate (unsigned char how_many)
{
    how_many_blinks_activate = how_many;
    led_activate_state = START_BLINKING;
}


void UpdateLedActivate (void)
{
    switch (led_activate_state)
    {
        case START_BLINKING:
            blink_activate = how_many_blinks_activate;
            
            if (blink_activate)
            {
                ACT_12V_ON;
                timer_led_activate = TIMER_BLINK;
                led_activate_state++;
                blink_activate--;
            }
            break;

        case WAIT_TO_OFF:
            if (!timer_led_activate)
            {
                ACT_12V_OFF;
                timer_led_activate = TIMER_BLINK;
                led_activate_state++;
            }
            break;

        case WAIT_TO_ON:
            if (!timer_led_activate)
            {
                if (blink_activate)
                {
                    blink_activate--;
                    timer_led_activate = TIMER_BLINK;
                    led_activate_state = WAIT_TO_OFF;
                    ACT_12V_ON;
                }
                else
                {
                    led_activate_state = WAIT_NEW_CYCLE;
                    timer_led_activate = TIMER_BLINK_NEXT_CYCLE;
                }
            }
            break;

        case WAIT_NEW_CYCLE:
            if (!timer_led_activate)
                led_activate_state = START_BLINKING;

            break;

        default:
            led_activate_state = START_BLINKING;
            break;
    }
}


void ToggleLedActivate (void)
{
    if (!timer_led_activate)
    {
        timer_led_activate = 125;
        if (ACT_12V)
            ACT_12V_OFF;
        else
            ACT_12V_ON;
    }
}


void WelcomeCode (void)
{
    char str [128] = { 0 };
    
    Usart2Debug("\r\nKirno -- Comunicador Vapore SMS --\r\n", 0);

#ifdef HARD
    Usart2Debug(HARD, 0);
    Wait_ms(100);    
#else
#error	"No Hardware defined in hard.h file"
#endif

#ifdef SOFT
    Usart2Debug(SOFT, 0);
    Wait_ms(100);    
#else
#error	"No Soft Version defined in hard.h file"
#endif
    
    Usart2Debug("\r\nFeatures:\r\n", 0);
    
    // Main Program Type
#ifdef DEFAULT_DEBUG_LVL
    sprintf(str,"[%s] %s: %d\n", __FILE__, str_macro(DEFAULT_DEBUG_LVL), DEFAULT_DEBUG_LVL);
    Usart2Debug(str, 0);
    Wait_ms(30);
#endif

#ifdef USE_GSM_GATEWAY_SM
    sprintf(str,"[%s] %s\n", __FILE__, str_macro(USE_GSM_GATEWAY_SM));
    Usart2Debug(str, 0);
    Wait_ms(30);    
#endif

#ifdef USE_GSM_GATEWAY_IN_LOOP
    sprintf(str,"[%s] %s\n", __FILE__, str_macro(USE_GSM_GATEWAY_IN_LOOP));
    Usart2Debug(str, 0);
    Wait_ms(30);    
#endif
    
}


void HARD_Timeouts (void)
{
    if (timer_led)
        timer_led--;

    if (timer_led_activate)
        timer_led_activate--;
    
    if (ALARM_INPUT)
    {
        if (s_alarm_input_cntr < SWITCHES_ROOF)
            s_alarm_input_cntr++;
    }
    else if (s_alarm_input_cntr)
    {
        if (s_alarm_input_cntr > 10)
            s_alarm_input_cntr -= 5;
        else
            s_alarm_input_cntr--;
    }
}



unsigned char Check_Alarm_Input (void)
{
    if (s_alarm_input_cntr > SWITCHES_THRESHOLD_MIN)
        return 1;
    else
        return 0;
}


void Led_Off (void)
{
    LED_OFF;
}


void Led_On (void)
{
    LED_ON;
}


unsigned char Led_Status (void)
{
    return LED;
}


void PwrKey_Off (void)
{
    PWRKEY_OFF;
}


void PwrKey_On (void)
{
    PWRKEY_ON;
}


void Activation_12V_On (void)
{
    ACT_12V_ON;
}


void Activation_12V_Off (void)
{
    ACT_12V_OFF;
}


unsigned char PwrKey_Status (void)
{
    return PWRKEY;
}


unsigned char Status_Status (void)
{
#ifdef HARDWARE_VER_2_0
    return 1;
#else
    return STATUS;
#endif
}


unsigned char NetLight_Status (void)
{
#ifdef HARDWARE_VER_2_0
    return 1;
#else
    return NETLIGHT;
#endif
}


//--- end of file ---//
