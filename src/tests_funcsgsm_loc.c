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
#include "funcs_gsm.h"

//helper modules
#include "tests_ok.h"
#include "tests_mock_usart.h"

#include <stdio.h>
#include <string.h>


// Externals -------------------------------------------------------------------
extern unsigned char get_loc_state;

// Externals To Module Globals in Here -----------------------------------------
unsigned char rssi_level = 15;
unsigned char register_status = 1;

volatile char buffUARTGSMrx2[256];

// Globals ---------------------------------------------------------------------
parameters_typedef mem_conf;

char cb_buff [5][200];
int cb_cnt = 0;

char v_resp [4][40] = {"resp_gsm_continue",
                       "resp_gsm_ok",
                       "resp_gsm_error",
                       "resp_gsm_timeout"};


char v_bearer [][40] = {"bearer_init",
                      "bearer_close_last",
                      "bearer_param",
                      "bearer_context",
                      "bearer_context_activate",
                      "bearer_read_param",
                      "lbs_set_address",
                      "lbs_get_location",
                      "bearer_deactivate",
                      "bearer_deactivate_with_error"};
    

// For Test Module Private Functions -------------------------------------------


// Module Mocked Functions -----------------------------------------------------
// void ChangeLedActivate (unsigned char number);
void MyCb (char * buff);
void MyCbClean (void);
void FuncsGSMSendGPRS_Answer (unsigned char answer);


// Module Functions for testing ------------------------------------------------
void Test_Send_Location (void);


// Module Functions ------------------------------------------------------------
int main(int argc, char *argv[])
{
    Test_Send_Location ();
}


void Test_Send_Location (void)
{
    t_RespGsm resp = resp_gsm_continue;
    int end_loop = 0;

    printf("setting apn...\n");
    strcpy(mem_conf.apn, "apn.personal.com");
    do {
        resp = FuncsGSM_GetLocation ();
        end_loop++;
        
    } while ((resp == resp_gsm_continue) && (end_loop < 80000));

    printf("test end with %s on loop: %d\n", v_resp[resp], end_loop);
    printf(" with state: %s\n", v_bearer[get_loc_state]);
    
}


// Module Mocked Functions -----------------------------------------------------
void Usart2Debug (char * s, unsigned char debug_level)
{
    char buff [30];
    sprintf(buff, "debug lvl %d  ", debug_level);
    PrintCyan(buff);
    Usart2Send(s);
}


//-- mocked from sim900_800 module --
unsigned char GSM_Start (void)
{
    return 1;
}

unsigned char GSM_Delay (unsigned short tim)
{
    return 1;
}

void GSM_Start_Stop_ResetSM (void)
{
}

unsigned char SMSLeft (void)
{
    return 1;
}

t_RespGsm GSMReceivSMS (void)
{
    return resp_gsm_continue;
}

t_RespGsm GSMSendSMS (char *ptrMSG, char *ptrNUM)
{
    printf("  SMS  send to number: %s\n       this message: %s\n", ptrNUM, ptrMSG);
    return 1;
}

unsigned char GSM_Stop(void)
{    
    return 1;
}

void GSMProcess (void)
{
}


void GSMReceive (void)
{
}

void CommsProcessSMSPayload (char * orig_num, char * payload)
{
    printf("comms process payload from: %s\nmsg: %s\n", orig_num, payload);
}

// unsigned char FuncsGSMSendSMS (char * msg, char * number)
// {
//     printf("  SMS  send to number: %s\n       this message: %s\n", number, msg);
//     return funcs_gsm_answer;
// }


// typedef enum {
//     cmd_continue = 1,
//     cmd_ok,
//     cmd_error,
//     cmd_timeout

// } resp_cmd_e;
char GSMSendCommand (char *ptrCommand, unsigned short timeOut, unsigned char rta, char *ptrRta)
{
    if (!strncmp(ptrCommand, "AT+SAPBR=0,1\r\n", sizeof("AT+SAPBR=0,1\r\n") - 1))
        return 2;

    else if (!strncmp(ptrCommand, "AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n",
                      sizeof("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n") - 1))
    {
        return 2;
    }

    else if (!strncmp(ptrCommand, "AT+SAPBR=3,1,\"APN\",\"apn.personal.com\"\r\n",
                      sizeof("AT+SAPBR=3,1,\"APN\",\"apn.personal.com\"\r\n") - 1))
    {
        return 2;
    }

    else if (!strncmp(ptrCommand, "AT+SAPBR=1,1\r\n",
                      sizeof("AT+SAPBR=1,1\r\n") - 1))
    {
        return 2;
    }

    else if (!strncmp(ptrCommand, "AT+SAPBR=2,1\r\n",
                      sizeof("AT+SAPBR=2,1\r\n") - 1))
    {
        if (rta)
            strcpy(ptrRta, "+SAPBR: 1,1,");
        
        return 2;
    }

    else if (!strncmp(ptrCommand, "AT+CLBSCFG=1,3,\"lbs-simcom.com:3002\"\r\n",
                      sizeof("AT+CLBSCFG=1,3,\"lbs-simcom.com:3002\"\r\n") - 1))
    {
        return 2;
    }

    else if (!strncmp(ptrCommand, "AT+CLBS=1,1\r\n",
                      sizeof("AT+CLBS=1,1\r\n") - 1))
    {
        if (rta)
        {
            strcpy(ptrRta, "+CLBS: 0,-58.467664,-34.635832,550");
            // strcpy(ptrRta, "+CLBS: 3,-58.467664,-34.635832,550");    //error on answer
        }

        return 2;
    }
    



    return 1;
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


