//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "comm.h"
#include "tests_ok.h"
#include "flash_program.h"

#include <stdio.h>
// #include <stdlib.h>
#include <string.h>


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
unsigned char enviar_sms = 0;
char enviar_sms_num [20] = { '\0' };
char enviar_sms_msg [160] = { '\0' };

parameters_typedef mem_conf;

// Module Auxialiary Functions -------------------------------------------------


// Module Functions for testing ------------------------------------------------
void Test_Comms (void);
    

// Module Functions ------------------------------------------------------------


int main(int argc, char *argv[])
{

    Test_Comms ();
    
}


void Test_Comms (void)
{
    char orig_num [22] = { 0 };
    char payload [160] = { 0 };


    strcpy(orig_num, "111115");
    strcpy(payload, "REPORTAR_OK:1");
    printf("Test REPORTAR_OK num: %s\n", orig_num);
    
    printf("Activar: ");    
    
    CommsProcessSMSPayload (orig_num, payload);

    if ((mem_conf.bkp_envios_ok == 1) &&
        (envios_ok_change) &&
        (enviar_sms == 1) &&
        (strcmp(enviar_sms_num, orig_num) == 0) &&
        (strcmp(enviar_sms_msg, "OK") == 0))
    {
        PrintOK();
    }
    else
        PrintERR();

    printf("Desactivar: ");    
    strcpy(payload, "REPORTAR_OK:0");
    CommsProcessSMSPayload (orig_num, payload);

    if ((mem_conf.bkp_envios_ok == 0) &&
        (envios_ok_change))
    {
        PrintOK();
    }
    else
        PrintERR();

    char my_new_number [] = {"1145376762"};
    printf("Test REPORTAR_NUM: %s\n", my_new_number);
    
    strcpy(payload, "REPORTAR_NUM:1145376762");    
    CommsProcessSMSPayload (orig_num, payload);

    if ((num_tel_rep_change) &&
        (strcmp(mem_conf.num_reportar, my_new_number) == 0))
    {
        PrintOK();
    }
    else
    {
        PrintERR();
        printf("el numero errado es: %s\n", mem_conf.num_reportar);
    }

    char my_new_site [] = {"Las Margaritas Prov. de Maximiliano"};
    printf("Test REPORTAR_SITIO: %s\n", my_new_site);
    
    strcpy(payload, "REPORTAR_SITIO:");
    strcat(payload, my_new_site);        
    CommsProcessSMSPayload (orig_num, payload);

    if ((sitio_prop_change) &&
        (strcmp(mem_conf.sitio_propio, my_new_site) == 0))
    {
        PrintOK();
    }
    else
    {
        PrintERR();
        printf("el sitio errado es: %s\n", mem_conf.sitio_propio);
    }

    printf("\n");
    char number_test [40] = { 0 };
    strcpy(number_test, "11567");
    printf("Test verify number: %s: ", number_test);
    if (VerifyNumberString(number_test))
        PrintOK();
    else
        PrintERR();
    
    strcpy(number_test, "012345678901234567890");
    printf("Test verify number: %s: ", number_test);
    if (VerifyNumberString(number_test))
        PrintOK();
    else
        PrintERR();

    strcpy(number_test, "+110123456789");
    printf("Test verify number: %s: ", number_test);
    if (VerifyNumberString(number_test))
        PrintOK();
    else
        PrintERR();

    strcpy(number_test, "+11012 3456789");
    printf("Test verify number: %s: ", number_test);
    if (VerifyNumberString(number_test))
        PrintOK();
    else
        PrintERR();

    strcpy(number_test, "+11012a");
    printf("Test verify number: %s: ", number_test);
    if (VerifyNumberString(number_test))
        PrintOK();
    else
        PrintERR();


    printf("\n");
    char site_test [80] = { 0 };
    strcpy(site_test, "La esquina de los pibes");
    printf("Test verify site: %s: ", site_test);
    if (VerifySiteString(site_test))
        PrintOK();
    else
        PrintERR();

    strcpy(site_test, "La esquina..~ 345 de los pibes");
    printf("Test verify site: %s: ", site_test);
    if (VerifySiteString(site_test))
        PrintOK();
    else
        PrintERR();
    
}





//--- end of file ---//


