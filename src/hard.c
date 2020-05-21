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
#include "adc.h"
#include "dsp.h"
#include "uart.h"

#include <stdio.h>


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
ma8_u16_data_obj_t mains_filter;




// Module Functions ------------------------------------------------------------
// Initial setup for filters and pointers
void HARD_Initial_Setup (void)
{
    MA8_U16Circular_Reset(&mains_filter);
}




//--- end of file ---//
