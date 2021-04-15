//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------

#include <stdio.h>
#include <string.h>
// #include <math.h>

// Types Constants and Macros --------------------------------------------------
typedef enum {
    CHECKING_SWITCHES = 0,
    SEND_DATA,
    SENDED_DATA_WAIT_ACK
    
} main_state_e;


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
unsigned char usart1_have_data = 0;
char new_uart_msg [200] = { 0 };
char last_uart_sended [200] = { 0 };


// Module Functions to Test ----------------------------------------------------
void Test_Functions (void);
main_state_e main_loop (main_state_e main_state, int sw_red_on, int timer_standby);

// Module Auxiliary Functions --------------------------------------------------
void Usart1Send (char * msg);
void CommFlushAck (void);
unsigned char CommCheckOK (void);
unsigned char CommCheckNOK (void);
    
void PrintOK (void);
void PrintERR (void);


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    Test_Functions ();

    return 0;
}


unsigned char acknowledge = 0;
void Test_Functions (void)
{
    printf("Testing main logic...\n");
    
    main_state_e main_state = CHECKING_SWITCHES;
    int sw_red_on = 0;
    int timer_standby = 0;

    printf("test no change: ");
    for (int i = 0; i < 50; i++)
    {
        main_state = main_loop(main_state, sw_red_on, timer_standby);
    }

    if (main_state == CHECKING_SWITCHES)
        PrintOK();
    else
        PrintERR();

    printf("look for five strings sended\n");
    sw_red_on = 1;
    for (int i = 0; i < 50; i++)
    {
        main_state = main_loop(main_state, sw_red_on, timer_standby);
        sw_red_on = 0;
    }

    printf("test sending no answer (timed out): ");
    if (main_state == CHECKING_SWITCHES)
        PrintOK();
    else
        PrintERR();

    printf("look for only one string sended\n");
    sw_red_on = 1;
    timer_standby = 10;
    for (int i = 0; i < 50; i++)
    {
        main_state = main_loop(main_state, sw_red_on, timer_standby);

        if (main_state == SENDED_DATA_WAIT_ACK)
            acknowledge |= 0x01;
            
        sw_red_on = 0;
    }

    printf("test sending with ack: ");
    if (main_state == CHECKING_SWITCHES)
        PrintOK();
    else
        PrintERR();

    printf("look for five strings sended\n");
    sw_red_on = 1;
    timer_standby = 10;
    for (int i = 0; i < 50; i++)
    {
        main_state = main_loop(main_state, sw_red_on, timer_standby);

        if (main_state == SENDED_DATA_WAIT_ACK)
            acknowledge |= 0x02;
        
        sw_red_on = 0;
    }

    printf("test sending no answer (nok answer): ");
    if (main_state == CHECKING_SWITCHES)
        PrintOK();
    else
        PrintERR();
    
}


char color_send [20] = { 0 };
int sended_cntr = 0;
main_state_e main_loop (main_state_e main_state, int sw_red_on, int timer_standby)
{
    switch (main_state)
    {
    case CHECKING_SWITCHES:
        if (sw_red_on)
        {
            strcpy(color_send, "red\n");
            sended_cntr = 5;
            main_state = SEND_DATA;
        }
        break;

    case SEND_DATA:
        if (sended_cntr)
        {
            sended_cntr--;
            timer_standby = 50;
            CommFlushAck();    //flush ok and nok
            Usart1Send(color_send);
            main_state = SENDED_DATA_WAIT_ACK;
        }
        else
            main_state = CHECKING_SWITCHES;
            
        break;

    case SENDED_DATA_WAIT_ACK:
        if (CommCheckOK())
        {
            //sended ok!
            main_state = CHECKING_SWITCHES;
        }

        if ((CommCheckNOK()) || (!timer_standby))
        {
            //error in comms or timed out
            main_state = SEND_DATA;
        }
            
        break;
            
    default:
        break;
    }
    
    return main_state;
}


void Usart1Send (char * msg)
{
    strcpy(last_uart_sended, msg);
    printf("%s",msg);
}


void CommFlushAck (void)
{
    acknowledge = 0;
}


unsigned char CommCheckOK (void)
{
    if (acknowledge & 0x01)
        return 1;
    else
        return 0;
}


unsigned char CommCheckNOK (void)
{
    if (acknowledge & 0x02)
        return 1;
    else
        return 0;
}


void PrintOK (void)
{
    printf("\033[0;32m");    //green
    printf("OK\n");
    printf("\033[0m");    //reset
}


void PrintERR (void)
{
    printf("\033[0;31m");    //red
    printf("ERR\n");
    printf("\033[0m");    //reset
}
//--- end of file ---//


