//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "parameters.h"
#include "sms_data.h"
#include "funcs_gsm.h"

//helper modules
#include "tests_ok.h"
#include "tests_mock_usart.h"

#include <stdio.h>
#include <string.h>


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
unsigned char enviar_sms = 0;
char enviar_sms_num [20] = { '\0' };
char enviar_sms_msg [160] = { '\0' };

parameters_typedef mem_conf;

char cb_buff [5][200];
int cb_cnt = 0;


// For Test Module Private Functions -------------------------------------------
extern unsigned char VerifyNumberString (char * number);
extern unsigned char VerifySiteString (char * site);


// Module Mocked Functions -----------------------------------------------------
void ChangeLedActivate (unsigned char number);
void MyCb (char * buff);
void MyCbClean (void);
void FuncsGSMSendSMS_Answer (unsigned char answer);


// Module Functions for testing ------------------------------------------------
void Test_Verify_Site (void);
void Test_Verify_Number (void);
void Test_Verify_And_Send_SMS (void);
    

// Module Functions ------------------------------------------------------------
int main(int argc, char *argv[])
{

    Test_Verify_Number ();

    Test_Verify_Site ();

    Test_Verify_And_Send_SMS ();
    
}




// Module Mocked Functions -----------------------------------------------------
void ChangeLedActivate (unsigned char number)
{
    if (number > 1)
        printf("led change to %d pulses\n", number);
    else
        printf("led change to %d pulse\n", number);
    
}


void Activation_12V_On (void)
{
    printf("12V ON!\n");
}


void Activation_12V_Off (void)
{
    printf("12V OFF!\n");
}


unsigned char funcs_gsm_answer = resp_gsm_continue;
unsigned char FuncsGSMSendSMS (char * msg, char * number)
{
    printf("  SMS  send to number: %s\n       this message: %s\n", number, msg);
    return funcs_gsm_answer;
}


void FuncsGSMSendSMS_Answer (unsigned char answer)
{
    funcs_gsm_answer = answer;
}


void MyCb (char * buff)
{
    printf("callback called %d\n", cb_cnt + 1);
    strcpy(&cb_buff[cb_cnt][0], buff);
    
    if (cb_cnt < 5)
        cb_cnt++;
    else
        cb_cnt = 0;
}


void MyCbClean (void)
{
    cb_cnt = 0;
    // memcpy(&cb_buff[0][0], '\0', 200);
    // memcpy(&cb_buff[1][0], '\0', 200);
    // memcpy(&cb_buff[2][0], '\0', 200);
    // memcpy(&cb_buff[3][0], '\0', 200);
    // memcpy(&cb_buff[4][0], '\0', 200);    
}
//--- end of file ---//


