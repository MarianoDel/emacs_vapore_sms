//------------------------------------------------
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEST_DEBUG.C #############################
//------------------------------------------------

// Includes --------------------------------------------------------------------
#include "test_debug.h"
#include "stm32g0xx.h"
#include "hard.h"
#include "tim.h"
#include "usart.h"
#include "tim.h"

#include <stdio.h>
#include <string.h>



// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Private Types & Macros -----------------------------------------------


// Module Private Functions ----------------------------------------------------
void TD_LedToggle (void);


// Module Functions ------------------------------------------------------------
void TD_Loop (void)
{
    Usart1Config_115200 ();
    Usart2Config();

    Usart2Send("GSM GATEWAY.. no module reboot\r\n");
    Wait_ms (100);
    Usart2Send("GSM GATEWAY working in loop - Ready\r\n");
    Wait_ms (100);
    Usart2Send("Usart2 -> PC at 9600\r\n");
    Wait_ms (100);
    Usart2Send("SIM <- Usart1 at 115200\r\n");
    Wait_ms (100);    

    char buff [256] = { 0 };
    while (1)
    {
        if (Usart2HaveData())
        {
            TD_LedToggle();
            Usart2HaveDataReset();
            Usart2ReadBuffer((unsigned char *)buff, sizeof(buff));

            // send it to gsm 115200
            Usart1Send(buff);
            Usart1Send("\r\n");
        }

        // send it to PC at 9600
        if (Usart1HaveData())
        {
            Usart1HaveDataReset();
            Usart1ReadBuffer((unsigned char *)buff, sizeof(buff));
            Usart2Send(buff);
        }
    }
}


void TD_LedToggle (void)
{
    if (Led_Status())
        Led_Off();
    else
        Led_On();
    
}

//--- end of file ---//
