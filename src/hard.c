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

// Configs. --------------------------------------------------------------------
#define LED_BLINK_ON    LED_ON
#define LED_BLINK_OFF    LED_OFF
#define TIMER_BLINK    200
#define TIMER_BLINK_NEXT_CYCLE    2000


// Externals -------------------------------------------------------------------
extern volatile unsigned short timer_led;


// Private Types ---------------------------------------------------------------
// led states
typedef enum
{    
    START_BLINKING = 0,
    WAIT_TO_OFF,
    WAIT_TO_ON,
    WAIT_NEW_CYCLE
    
} led_state_t;


// Globals ---------------------------------------------------------------------
// for the led
led_state_t led_state = START_BLINKING;
unsigned char blink = 0;
unsigned char how_many_blinks = 0;



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


void WelcomeCode (void)
{
    char str [128] = { 0 };
    
    Usart2Send("\r\nKirno -- Comunicador Vapore SMS --\r\n");

#ifdef HARD
    Usart2Send(HARD);
    Wait_ms(100);    
#else
#error	"No Hardware defined in hard.h file"
#endif

#ifdef SOFT
    Usart2Send(SOFT);
    Wait_ms(100);    
#else
#error	"No Soft Version defined in hard.h file"
#endif
    
    Usart2Send("Features:\r\n");
    
    // Main Program Type
#ifdef USE_GSM
    sprintf(str,"[%s] %s\n", __FILE__, str_macro(USE_GSM));
    Usart2Send(str);
    Wait_ms(30);
#endif

#ifdef USE_REDONDA_BASIC
    sprintf(str,"[%s] %s\n", __FILE__, str_macro(USE_REDONDA_BASIC));
    Usart2Send(str);
    Wait_ms(30);    
#endif

#ifdef DEBUG_ON
    sprintf(str,"[%s] %s\n", __FILE__, str_macro(DEBUG_ON));
    Usart2Send(str);
    Wait_ms(30);    
#endif
    
}



//--- end of file ---//
