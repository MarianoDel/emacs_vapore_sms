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
#include "parameters.h"

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

// Module Auxialiary Functions -------------------------------------------------


// Module Functions for testing ------------------------------------------------
void Test_Verify_Site (void);
void Test_Verify_Number (void);
    

// Module Functions ------------------------------------------------------------


int main(int argc, char *argv[])
{

    Test_Verify_Number ();

    Test_Verify_Site ();
    
}


void Test_Verify_Number (void)
{
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
    
}


void Test_Verify_Site (void)
{
    char my_new_site2 [] = {"ALARMA VECINAL QUIRNO 457 CIUDAD AUTONOMA DE BUENOS AIRES ENTRE CALLE AV DIRECTORIO Y TANDIL CODIGO POSTAL 1406"};
    
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

    strcpy(site_test, my_new_site2);
    printf("Test verify site: %s: ", site_test);
    if (VerifySiteString(site_test))
        PrintOK();
    else
        PrintERR();

    printf("after verifing: %s\n", site_test);

    char too_long_site [200];
    char too_long_s [] = {"00 ALARMA 0123456789"};

    sprintf(too_long_site, "%s %s %s %s %s %s %s %s",
           too_long_s,
           too_long_s,
           too_long_s,
           too_long_s,
           too_long_s,
           too_long_s,
           too_long_s,
           too_long_s);

    printf("Verify site too long: \n%s\n", too_long_site);

    printf("Test site too long: ");    
    if (!VerifySiteString(too_long_site))
        PrintOK();
    else
        PrintERR();

    char too_short_site [] = {"ho"};

    printf("Verify site too short: \n%s\n", too_short_site);

    printf("Test site too short: ");    
    if (!VerifySiteString(too_short_site))
        PrintOK();
    else
        PrintERR();
    
    printf("\n");
    // printf("Prueba de letras con acentos A E I O U\n");
    // strcpy(site_test, "аимсз");
    // for (int i = 0; i < strlen(site_test); i++)
    //     printf("0x%x  dec: %d\n", (unsigned char) site_test[i], (unsigned char) site_test[i]);
    
}

//--- end of file ---//


