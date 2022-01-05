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

    battery_filter_out = 2618;
    printf("Test Battery Sense: %d\n", battery_filter_out);
    Battery_Voltage(&vint, &vdec);
    printf("int: %d dec: %d voltage: %02d.%02dV\n", vint, vdec, vint, vdec);

    printf("Test result: ");
    if ((vint == 13) &&
        (vdec == 93))
    {
        PrintOK();
    }
    else
        PrintERR();

    battery_filter_out = 0;
    printf("Test Battery Sense: %d\n", battery_filter_out);
    Battery_Voltage(&vint, &vdec);
    printf("int: %d dec: %d voltage: %02d.%02dV\n", vint, vdec, vint, vdec);

    printf("Test result: ");
    if ((vint == 0) &&
        (vdec == 70))
    {
        PrintOK();
    }
    else
        PrintERR();

    battery_filter_out = 4095;
    printf("Test Battery Sense: %d\n", battery_filter_out);
    Battery_Voltage(&vint, &vdec);
    printf("int: %d dec: %d voltage: %02d.%02dV\n", vint, vdec, vint, vdec);

    printf("Test result: ");
    if ((vint == 21) &&
        (vdec == 39))
    {
        PrintOK();
    }
    else
        PrintERR();
    
}





//--- end of file ---//


