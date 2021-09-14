//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "comms_from_panel.h"

//helper modules
#include "tests_ok.h"
#include "tests_mock_usart.h"

#include <stdio.h>
#include <string.h>


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------

// Module Auxialiary Functions -------------------------------------------------


// Module Functions for testing ------------------------------------------------
void Test_Comms_From_Panel (void);
    

// Module Functions ------------------------------------------------------------


int main(int argc, char *argv[])
{

    Test_Comms_From_Panel ();
    
}


void Test_Comms_From_Panel (void)
{
    unsigned char answer = 0;
    unsigned short number = 0;
    char payload [160] = { 0 };

    Usart2FillRxBuffer("Activo: ");
    printf("Test only Activo: ");
    answer = Panel_Check_Alarm (&number);

    if (answer)
        PrintERR();
    else
        PrintOK();

    Usart2FillRxBuffer("Activo: CCC B1");
    printf("Test only Activo: CCC B1: ");
    answer = Panel_Check_Alarm (&number);

    if (answer)
        PrintERR();
    else
        PrintOK();

    Usart2FillRxBuffer("Activo: 000 B2");
    printf("Test only Activo: 000 B2: ");
    answer = Panel_Check_Alarm (&number);

    if (answer)
        PrintERR();
    else
        PrintOK();


    Usart2FillRxBuffer("Activo: 000 B1");
    printf("Test only Activo: 000 B1: ");
    answer = Panel_Check_Alarm (&number);

    if ((answer) && (number == 0))
        PrintOK();
    else
        PrintERR();

    Usart2FillRxBuffer("Activo: 999 B1");
    printf("Test only Activo: 999 B1: ");
    answer = Panel_Check_Alarm (&number);

    if ((answer) && (number == 999))
        PrintOK();
    else
        PrintERR();
    
    
        
    
    
    // printf("Activar: ");    
    
    // CommsProcessSMSPayload (orig_num, payload);

    // if ((mem_conf.bkp_envios_ok == 1) &&
    //     (envios_ok_change) &&
    //     (enviar_sms == 1) &&
    //     (strcmp(enviar_sms_num, orig_num) == 0) &&
    //     (strcmp(enviar_sms_msg, "OK") == 0))
    // {
    //     PrintOK();
    // }
    // else
    //     PrintERR();

    // printf("Desactivar: ");    
    // strcpy(payload, "REPORTAR_OK:0");
    // CommsProcessSMSPayload (orig_num, payload);

    // if ((mem_conf.bkp_envios_ok == 0) &&
    //     (envios_ok_change))
    // {
    //     PrintOK();
    // }
    // else
    //     PrintERR();

    // char my_new_number [] = {"1145376762"};
    // printf("Test REPORTAR_NUM: %s\n", my_new_number);
    
    // strcpy(payload, "REPORTAR_NUM:1145376762OK");    //+trailing OK
    // CommsProcessSMSPayload (orig_num, payload);

    // if ((num_tel_rep_change) &&
    //     (strcmp(mem_conf.num_reportar, my_new_number) == 0))
    // {
    //     PrintOK();
    // }
    // else
    // {
    //     PrintERR();
    //     printf("el numero errado es: %s\n", mem_conf.num_reportar);
    // }

    // char my_new_site [] = {"Las Margaritas Prov. de Maximiliano"};
    // printf("Test REPORTAR_SITIO: %s\n", my_new_site);
    
    // strcpy(payload, "REPORTAR_SITIO:");
    // strcat(payload, my_new_site);
    // strcat(payload, "OK");        //+trailing OK
    // CommsProcessSMSPayload (orig_num, payload);

    // if ((sitio_prop_change) &&
    //     (strcmp(mem_conf.sitio_propio, my_new_site) == 0))
    // {
    //     PrintOK();
    // }
    // else
    // {
    //     PrintERR();
    //     printf("el sitio errado es: %s\n", mem_conf.sitio_propio);
    // }

    // // char my_new_site2 [] = {"ALARMA VECINAL QUIRNO 457 CIUDAD AUT”NOMA DE BUENOS AIRES ENTRE CALLE AV DIRECTORIO Y TANDIL CODIGO POSTAL 1406"};
    // char my_new_site2 [] = {"ALARMA VECINAL QUIRNO 457 CIUDAD AUTONOMA DE BUENOS AIRES ENTRE CALLE AV DIRECTORIO Y TANDIL CODIGO POSTAL 1406"};
    // // char my_new_site2 [] = {"ALARMA VECINAL QUIRNO 457 CIUDAD AUT”NOMA DE BUENOS AIRES ENTRE CALLE AV DIRECTORIO"};
    // // char my_new_site2 [] = {"AL¡RMA"};
    // printf("Test REPORTAR_SITIO: %s, largo: %d\n", my_new_site2, strlen(my_new_site2));
    
    // strcpy(payload, "REPORTAR_SITIO:");
    // strcat(payload, my_new_site2);
    // strcat(payload, "OK");        //+trailing OK
    // CommsProcessSMSPayload (orig_num, payload);

    // if ((sitio_prop_change) &&
    //     (strcmp(mem_conf.sitio_propio, my_new_site2) == 0))
    // {
    //     PrintOK();
    // }
    // else
    // {
    //     PrintERR();
    //     printf("el sitio errado es: %s\n", mem_conf.sitio_propio);
    // }
    
    // printf("\n");
    // char number_test [40] = { 0 };
    // strcpy(number_test, "11567");
    // printf("Test verify number: %s: ", number_test);
    // if (VerifyNumberString(number_test))
    //     PrintOK();
    // else
    //     PrintERR();
    
    // strcpy(number_test, "012345678901234567890");
    // printf("Test verify number: %s: ", number_test);
    // if (VerifyNumberString(number_test))
    //     PrintOK();
    // else
    //     PrintERR();

    // strcpy(number_test, "+110123456789");
    // printf("Test verify number: %s: ", number_test);
    // if (VerifyNumberString(number_test))
    //     PrintOK();
    // else
    //     PrintERR();

    // strcpy(number_test, "+11012 3456789");
    // printf("Test verify number: %s: ", number_test);
    // if (VerifyNumberString(number_test))
    //     PrintOK();
    // else
    //     PrintERR();

    // strcpy(number_test, "+11012a");
    // printf("Test verify number: %s: ", number_test);
    // if (VerifyNumberString(number_test))
    //     PrintOK();
    // else
    //     PrintERR();


    // printf("\n");
    // char site_test [80] = { 0 };
    // strcpy(site_test, "La esquina de los pibes");
    // printf("Test verify site: %s: ", site_test);
    // if (VerifySiteString(site_test))
    //     PrintOK();
    // else
    //     PrintERR();

    // strcpy(site_test, "La esquina..~ 345 de los pibes");
    // printf("Test verify site: %s: ", site_test);
    // if (VerifySiteString(site_test))
    //     PrintOK();
    // else
    //     PrintERR();

    // strcpy(site_test, my_new_site2);
    // printf("Test verify site: %s: ", site_test);
    // if (VerifySiteString(site_test))
    //     PrintOK();
    // else
    //     PrintERR();

    // printf("after verifing: %s\n", site_test);

    // // printf("Prueba de letras con acentos A E I O U\n");
    // // strcpy(site_test, "¡…Õ”⁄");
    // // for (int i = 0; i < strlen(site_test); i++)
    // //     printf("0x%x  dec: %d\n", (unsigned char) site_test[i], (unsigned char) site_test[i]);
}





//--- end of file ---//


