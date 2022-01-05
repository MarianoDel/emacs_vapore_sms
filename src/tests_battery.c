//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "battery.h"

//helper modules
#include "tests_ok.h"
#include "tests_utils.h"

#include <stdio.h>
#include <string.h>


// Externals -------------------------------------------------------------------
extern unsigned short battery_filter_out;


// Globals ---------------------------------------------------------------------
volatile unsigned short adc_ch [2] = { 0 };

// Module Auxialiary Functions -------------------------------------------------


// Module Functions for testing ------------------------------------------------
void Test_Battery (void);
    

// Module Functions ------------------------------------------------------------


int main(int argc, char *argv[])
{

    Test_Battery ();
    
}


void Test_Battery (void)
{
    unsigned char vint = 0;
    unsigned char vdec = 0;
    float volts_conversion = 0.0;
    float volts_expected = 0.0;
    int distance = 0;

    battery_filter_out = 2618;
    printf("Test Battery Sense: %d\n", battery_filter_out);
    Battery_Voltage(&vint, &vdec);
    printf("int: %d dec: %d voltage: %02d.%02dV\n", vint, vdec, vint, vdec);

    printf("Test result: ");

    volts_expected = 13.93;
    volts_conversion = vint + vdec / 100.0;
    distance = Utils_Distance_Floats(volts_expected, volts_conversion, 1);
    if (distance == 1)
        PrintOK();
    else
        PrintERR();

    battery_filter_out = 0;
    printf("Test Battery Sense: %d\n", battery_filter_out);
    Battery_Voltage(&vint, &vdec);
    printf("int: %d dec: %d voltage: %02d.%02dV\n", vint, vdec, vint, vdec);

    printf("Test result: ");
    volts_expected = 0.7;
    volts_conversion = vint + vdec / 100.0;
    distance = Utils_Distance_Floats(volts_expected, volts_conversion, 1);
    if (distance == 1)
        PrintOK();
    else
        PrintERR();

    battery_filter_out = 4095;
    printf("Test Battery Sense: %d\n", battery_filter_out);
    Battery_Voltage(&vint, &vdec);
    printf("int: %d dec: %d voltage: %02d.%02dV\n", vint, vdec, vint, vdec);

    printf("Test result: ");
    volts_expected = 21.39;
    volts_conversion = vint + vdec / 100.0;
    distance = Utils_Distance_Floats(volts_expected, volts_conversion, 1);
    if (distance == 1)
        PrintOK();
    else
        PrintERR();
}





//--- end of file ---//


