//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "contact_id.h"

//helper modules
#include "tests_ok.h"

#include <stdio.h>
#include <string.h>


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// For Test Module Private Functions -------------------------------------------


// Module Mocked Functions -----------------------------------------------------


// Module Functions for testing ------------------------------------------------
void Test_Verify_Contact_ID_Strings (void);
    

// Module Functions ------------------------------------------------------------
int main(int argc, char *argv[])
{

    Test_Verify_Contact_ID_Strings ();
    
}


void Test_Verify_Contact_ID_Strings (void)
{
    char msg [160];
    char orig_msg [160];
    unsigned char id = panic_alarm;

    // original message   
    char client_num [] = {"CB53"};
    char remote_number [] = {"999"};
    
    printf("Test panic alarm id: ");
    id = panic_alarm;
    sprintf(orig_msg, "18 %s 01 120 01 %s", client_num, remote_number);
    ContactIDString(id, client_num, remote_number, msg);

    if ((strlen(msg) == 21) &&
        (!strcmp(msg, orig_msg)))
        PrintOK();
    else
        PrintERR();

    printf("Test ac loss opening id: ");
    id = ac_loss_opening;
    sprintf(orig_msg, "18 %s 01 301 01 %s", client_num, remote_number);
    ContactIDString(id, client_num, remote_number, msg);

    if ((strlen(msg) == 21) &&
        (!strcmp(msg, orig_msg)))
        PrintOK();
    else
        PrintERR();

    printf("Test ac loss close id: ");
    id = ac_loss_close;
    sprintf(orig_msg, "18 %s 03 301 01 %s", client_num, remote_number);
    ContactIDString(id, client_num, remote_number, msg);

    if ((strlen(msg) == 21) &&
        (!strcmp(msg, orig_msg)))
        PrintOK();
    else
        PrintERR();

    printf("Test low system battery opening id: ");
    id = low_system_battery_opening;
    sprintf(orig_msg, "18 %s 01 302 01 %s", client_num, remote_number);
    ContactIDString(id, client_num, remote_number, msg);

    if ((strlen(msg) == 21) &&
        (!strcmp(msg, orig_msg)))
        PrintOK();
    else
        PrintERR();

    printf("Test low system battery close id: ");
    id = low_system_battery_close;
    sprintf(orig_msg, "18 %s 03 302 01 %s", client_num, remote_number);
    ContactIDString(id, client_num, remote_number, msg);

    if ((strlen(msg) == 21) &&
        (!strcmp(msg, orig_msg)))
        PrintOK();
    else
        PrintERR();

    printf("Test keep alive id: ");
    id = keep_alive;
    sprintf(orig_msg, "18 %s 01 602 01 %s", client_num, remote_number);
    ContactIDString(id, client_num, remote_number, msg);

    if ((strlen(msg) == 21) &&
        (!strcmp(msg, orig_msg)))
        PrintOK();
    else
        PrintERR();
    
}


// Module Mocked Functions -----------------------------------------------------


//--- end of file ---//


