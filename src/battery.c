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
#include "parameters.h"

#include <string.h>
#include <stdio.h>

// Module Types Constants and Macros -------------------------------------------
// batt voltage to adc (batt - 0.7) * 197.88
#define BAT_VOLTAGE_HYSTERESIS    138    // 0.7 hyst
// #define BAT_LOW_VOLTAGE    1978    // 10V + 0.7 = 10.7
// #define BAT_LOW_VOLTAGE    1741    // 8.8 + 0.7 = 9.5
#define BAT_LOW_VOLTAGE    1840    // 9.3 + 0.7 = 10
#define BAT_LOW_VOLTAGE_RECONNECT    (BAT_LOW_VOLTAGE + BAT_VOLTAGE_HYSTERESIS)
// #define BAT_DISC_VOLTAGE    1780    // 9V + 0.7 = 9.7
#define BAT_DISC_VOLTAGE    1642    // 8.3 + 0.7 = 9.0
#define BAT_DISC_VOLTAGE_RECONNECT    (BAT_DISC_VOLTAGE + BAT_VOLTAGE_HYSTERESIS)

typedef enum {
    BATTERY_GOOD,
    BATTERY_LOW_VOLTAGE,
    BATTERY_DISCONNECT

} batt_status_e;

// Externals -------------------------------------------------------------------
extern volatile unsigned short adc_ch [];
extern parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------
unsigned char cntr = 0;
unsigned short battery_filter_out = 0;
ma16_u16_data_obj_t battery_filter;
batt_status_e battery_status = BATTERY_GOOD;


// Module Private Functions ----------------------------------------------------



// Module Functions ------------------------------------------------------------
void Battery_Check_Init (void)
{
    MA16_U16Circular_Reset (&battery_filter);

    // charge the filter
    for (unsigned char i = 0; i < 16; i++)
        battery_filter_out = MA16_U16Circular (&battery_filter, V_Sense_12V);
    
}
    

void Battery_Check (void)
{
    switch (battery_status)
    {
    case BATTERY_GOOD:
        if (battery_filter_out < BAT_LOW_VOLTAGE)
        {
            battery_status = BATTERY_LOW_VOLTAGE;
            diag_battery_low_voltage_set;
        }
        break;
        
    case BATTERY_LOW_VOLTAGE:
        if (battery_filter_out < BAT_DISC_VOLTAGE)
        {
            battery_status = BATTERY_DISCONNECT;
            diag_battery_disconnect_voltage_set;
        }

        if (battery_filter_out > BAT_LOW_VOLTAGE_RECONNECT)
        {
            battery_status = BATTERY_GOOD;
            // diag_battery_good_voltage_set;
        }
        break;
        
    case BATTERY_DISCONNECT:
        if (battery_filter_out > BAT_DISC_VOLTAGE_RECONNECT)
        {
            battery_status = BATTERY_LOW_VOLTAGE;
        }
        break;
        
    default:
        battery_status = BATTERY_GOOD;
        break;
    }
    
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
