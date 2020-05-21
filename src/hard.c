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
#include "stm32f0xx.h"
#include "uart.h"

#include <stdio.h>


// Externals -------------------------------------------------------------------
extern volatile unsigned short timer_led;


// Private Types ---------------------------------------------------------------
//ESTADOS DEL LED
typedef enum
{    
    START_BLINKING = 0,
    WAIT_TO_OFF,
    WAIT_TO_ON,
    WAIT_NEW_CYCLE
    
} led_state_t;


// Globals ---------------------------------------------------------------------
// para el led
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
                LED_PWR_ON;
                timer_led = 200;
                led_state++;
                blink--;
            }
            break;

        case WAIT_TO_OFF:
            if (!timer_led)
            {
                LED_PWR_OFF;
                timer_led = 200;
                led_state++;
            }
            break;

        case WAIT_TO_ON:
            if (!timer_led)
            {
                if (blink)
                {
                    blink--;
                    timer_led = 200;
                    led_state = WAIT_TO_OFF;
                    LED_PWR_ON;
                }
                else
                {
                    led_state = WAIT_NEW_CYCLE;
                    timer_led = 2000;
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

void WelcomeCodeFeatures (char * str)
{
    Usart2Send("\r\nKirno Placa Redonda - Basic V1.1\r\n");
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
