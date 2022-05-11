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
#include "gprs_data.h"
// #include "funcs_gsm.h"

//helper modules
#include "tests_ok.h"
#include "tests_mock_usart.h"

#include <stdio.h>
#include <string.h>


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------

parameters_typedef mem_conf;

char cb_buff [5][200];
int cb_cnt = 0;


// For Test Module Private Functions -------------------------------------------
extern unsigned char VerifyAPNString (char * apn);


// Module Mocked Functions -----------------------------------------------------
// void ChangeLedActivate (unsigned char number);
void MyCb (char * buff);
void MyCbClean (void);
// void FuncsGSMSendSMS_Answer (unsigned char answer);


// Module Functions for testing ------------------------------------------------
void Test_Verify_APN (void);
// void Test_Verify_Number (void);
// void Test_Verify_And_Send_SMS (void);
    

// Module Functions ------------------------------------------------------------
int main(int argc, char *argv[])
{

    Test_Verify_APN ();

    // Test_Verify_Site ();

    // Test_Verify_And_Send_SMS ();
    
}


// void Test_Verify_And_Send_SMS (void)
// {
//     char serial_buff [200];
//     char buff [200];
//     sms_pckt_t sms_info;
//     unsigned char answer = 0;

//     // sms packet default
//     sms_info.alarm_input = 0;
//     sms_info.panel_input = 0;
//     sms_info.remote_number = 0;
//     sms_info.buff = buff;

//     strcpy(mem_conf.num_reportar, " ");
    
//     // test for alarm input
//     sms_info.alarm_input = 1;
//     Usart2Callback(&MyCb);
//     MyCbClean();
//     answer = VerifyAndSendSMS (&sms_info);

//     Usart2ReadBuffer(serial_buff, sizeof(serial_buff));

//     printf("Test send sms alarm input: ");
//     if (!(strncmp(&cb_buff[0][0], "External 12V: ", sizeof("External 12V: ") - 1)) &&
//         (!(strncmp(&cb_buff[1][0], "no phone number", sizeof("no phone number") - 1)) &&        
//          (answer == SMS_NOT_PROPER_DATA)))
//         PrintOK();
//     else
//         PrintERR();


//     // test for panel input
//     sms_info.alarm_input = 0;
//     sms_info.panel_input = 1;
//     MyCbClean();
//     answer = VerifyAndSendSMS (&sms_info);

//     Usart2ReadBuffer(serial_buff, sizeof(serial_buff));

//     printf("Test send sms panel input: ");
//     if (!(strncmp(&cb_buff[0][0], "Keypad ACT: ", sizeof("Keypad ACT: ") - 1)) &&
//         (!(strncmp(&cb_buff[1][0], "no phone number", sizeof("no phone number") - 1)) &&        
//          (answer == SMS_NOT_PROPER_DATA)))
//         PrintOK();
//     else
//         PrintERR();


//     // test for alarm input with phone
//     sms_info.alarm_input = 1;
//     sms_info.panel_input = 0;
//     strcpy(mem_conf.num_reportar, "+541122");
//     MyCbClean();
//     answer = VerifyAndSendSMS (&sms_info);

//     Usart2ReadBuffer(serial_buff, sizeof(serial_buff));

//     printf("Test send sms panel input phone %s: ", mem_conf.num_reportar);
//     if (!(strncmp(&cb_buff[0][0], "External 12V: ", sizeof("External 12V: ") - 1)) &&
//         (!(strncmp(&cb_buff[1][0], "no site saved", sizeof("no site saved") - 1)) &&
//          (answer == SMS_NOT_PROPER_DATA)))
//         PrintOK();
//     else
//         PrintERR();

//     // test for alarm input with phone and site
//     sms_info.alarm_input = 1;
//     sms_info.panel_input = 0;
//     strcpy(mem_conf.num_reportar, "+541122");
//     strcpy(mem_conf.sitio_propio, "quirno 457");
//     MyCbClean();
//     FuncsGSMSendSMS_Answer(resp_gsm_ok);
//     answer = VerifyAndSendSMS (&sms_info);

//     Usart2ReadBuffer(serial_buff, sizeof(serial_buff));

//     printf("Test send sms alarm input phone %s site %s: ",
//            mem_conf.num_reportar,
//            mem_conf.sitio_propio);
//     if (!(strncmp(&cb_buff[0][0], "External 12V: ", sizeof("External 12V: ") - 1)) &&
//         (answer == SMS_SENT))
//         PrintOK();
//     else
//         PrintERR();

//     // test for panel input with phone and site
//     sms_info.alarm_input = 0;
//     sms_info.panel_input = 1;
//     strcpy(mem_conf.num_reportar, "+541122");
//     strcpy(mem_conf.sitio_propio, "quirno 457");
//     MyCbClean();
//     FuncsGSMSendSMS_Answer(resp_gsm_ok);
//     answer = VerifyAndSendSMS (&sms_info);

//     Usart2ReadBuffer(serial_buff, sizeof(serial_buff));

//     printf("Test send sms panel input phone %s site %s: ",
//            mem_conf.num_reportar,
//            mem_conf.sitio_propio);
//     if (!(strncmp(&cb_buff[0][0], "Keypad ACT: ", sizeof("Keypad ACT: ") - 1)) &&    
//         (answer == SMS_SENT))
//         PrintOK();
//     else
//         PrintERR();


//     // test for panel input with phone and site not send
//     sms_info.alarm_input = 0;
//     sms_info.panel_input = 1;
//     strcpy(mem_conf.num_reportar, "+541122");
//     strcpy(mem_conf.sitio_propio, "quirno 457");
//     MyCbClean();
//     FuncsGSMSendSMS_Answer(resp_gsm_error);
//     answer = VerifyAndSendSMS (&sms_info);

//     Usart2ReadBuffer(serial_buff, sizeof(serial_buff));

//     printf("Test send sms panel input phone %s site %s: ",
//            mem_conf.num_reportar,
//            mem_conf.sitio_propio);
//     if (!(strncmp(&cb_buff[0][0], "Keypad ACT: ", sizeof("Keypad ACT: ") - 1)) &&    
//         (answer == SMS_NOT_SEND))
//         PrintOK();
//     else
//         PrintERR();
    
    
// }


// void Test_Verify_Number (void)
// {
//     char number_test [40] = { 0 };
//     strcpy(number_test, "11567");
//     printf("Test verify number %d: %s: ", strlen(number_test), number_test);
//     if (VerifyNumberString(number_test))
//         PrintOK();
//     else
//         PrintERR();
    
//     strcpy(number_test, "012345678901234567890");
//     printf("Test verify long number %d: %s: ", strlen(number_test), number_test);
//     if (!VerifyNumberString(number_test))
//         PrintOK();
//     else
//         PrintERR();

//     strcpy(number_test, "+110123456789");
//     printf("Test verify number plus signed %d: %s: ", strlen(number_test), number_test);
//     if (VerifyNumberString(number_test))
//         PrintOK();
//     else
//         PrintERR();

//     strcpy(number_test, "+11012 3456789");
//     printf("Test verify broken number: %s: ", number_test);
//     if (!VerifyNumberString(number_test))
//         PrintOK();
//     else
//         PrintERR();

//     strcpy(number_test, "+11012a");
//     printf("Test verify mixed number: %s: ", number_test);
//     if (!VerifyNumberString(number_test))
//         PrintOK();
//     else
//         PrintERR();
    
// }


void Test_Verify_APN (void)
{
    char apn_test [80] = { 0 };
    char modif_apn_test [80] = { 0 };    
    
    strcpy(apn_test, "gprs.personal.com");
    strcpy(modif_apn_test, apn_test);    
    printf("Test verify apn: %s: ", apn_test);
    if ((VerifyAPNString(modif_apn_test)) &&
        (!strcmp(modif_apn_test, apn_test)))
        PrintOK();
    else
        PrintERR();

    strcpy(apn_test, "igprs.claro.com.ar");
    strcpy(modif_apn_test, apn_test);    
    printf("Test verify apn: %s: ", apn_test);
    if ((VerifyAPNString(modif_apn_test)) &&
        (!strcmp(modif_apn_test, apn_test)))
        PrintOK();
    else
        PrintERR();

    strcpy(apn_test, "wap.gprs.unifon.com.ar");
    strcpy(modif_apn_test, apn_test);    
    printf("Test verify apn: %s: ", apn_test);
    if ((VerifyAPNString(modif_apn_test)) &&
        (!strcmp(modif_apn_test, apn_test)))
        PrintOK();
    else
        PrintERR();

    strcpy(apn_test, "datos.personal.com");
    strcpy(modif_apn_test, apn_test);    
    printf("Test verify apn: %s: ", apn_test);
    if ((VerifyAPNString(modif_apn_test)) &&
        (!strcmp(modif_apn_test, apn_test)))
        PrintOK();
    else
        PrintERR();
    
    
    printf("\n");
    
}


// Module Mocked Functions -----------------------------------------------------
// void ChangeLedActivate (unsigned char number)
// {
//     if (number > 1)
//         printf("led change to %d pulses\n", number);
//     else
//         printf("led change to %d pulse\n", number);
    
// }


void Activation_12V_On (void)
{
    printf("12V ON!\n");
}


void Activation_12V_Off (void)
{
    printf("12V OFF!\n");
}


// unsigned char funcs_gsm_answer = resp_gsm_continue;
// unsigned char FuncsGSMSendSMS (char * msg, char * number)
// {
//     printf("  SMS  send to number: %s\n       this message: %s\n", number, msg);
//     return funcs_gsm_answer;
// }


// void FuncsGSMSendSMS_Answer (unsigned char answer)
// {
//     funcs_gsm_answer = answer;
// }


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

