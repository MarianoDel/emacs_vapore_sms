//---------------------------------------------------------
// #### PROYECTO COMUNICADOR VAPORE SMS - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### BATTERY.C #########################################
//---------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "battery.h"
#include "adc.h"
#include "dma.h"
#include "stm32g0xx.h"
#include "dsp.h"

#include <string.h>
#include <stdio.h>


// Externals -------------------------------------------------------------------
extern volatile unsigned short adc_ch [];


// Globals ---------------------------------------------------------------------
unsigned char cntr = 0;
unsigned short battery_filter_out = 0;
ma16_u16_data_obj_t battery_filter;


// Constants -------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------



// Module Functions ------------------------------------------------------------
void Battery_Check_Init (void)
{
    MA16_U16Circular_Reset (&battery_filter);
}
    

void Battery_Check (void)
{
    if (sequence_ready)
    {
        sequence_ready_reset;
        if (cntr < 100)
            cntr++;
        else
        {
            battery_filter_out = MA16_U16Circular (&battery_filter, V_Sense_12V);
            cntr = 0;
        }
    }
}


unsigned short Battery_Get_Voltage (void)
{
    return battery_filter_out;
}


// 13.94V minus diode 13.23V
// 13.23V -> 2618 pts
void Battery_Voltage (unsigned char * v_int, unsigned char * v_dec)
{
    // float calc = 0.0;

    // calc = battery_filter_out / 197.88;
    // calc = calc + 0.7;
    // *v_int = (unsigned char) calc;
    // calc = calc - *v_int;
    // calc = calc * 100;
    // *v_dec = (unsigned char) calc;

    unsigned int calc_int = 0;
    unsigned int calc_dec = 0;
    unsigned int calc = battery_filter_out;
    calc = calc * 505;
    calc = calc + 70000;

    calc_int = calc / 100000;
    *v_int = (unsigned char) calc_int;

    calc_dec = calc - calc_int * 100000;
    calc_dec = calc_dec / 1000;
    *v_dec = (unsigned char) calc_dec;
}

//--- end of file ---//

