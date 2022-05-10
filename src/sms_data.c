//--------------------------------------------------
// #### VAPORE SMS COMMS PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### SMS_DATA.C #################################
//--------------------------------------------------

// Includes --------------------------------------------------------------------
#include "sms_data.h"
#include "parameters.h"
#include "usart.h"
#include "hard.h"
#include "funcs_gsm.h"


#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------



// Externals -------------------------------------------------------------------
extern parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------
unsigned char VerifyNumberString (char * number);
unsigned char VerifySiteString (char * site);


// Module Functions ------------------------------------------------------------
unsigned char VerifyAndSendSMS (sms_pckt_t * p_sms)
{
    if (p_sms->alarm_input)
        Usart2Send("External 12V: ");    // 12V input alarm activate

    if (p_sms->panel_input)
        Usart2Send("Keypad ACT: ");
                    
    //check num_tel_rep before send sms
    if (!VerifyNumberString(num_tel_rep))
    {
        ChangeLedActivate(1);
        Usart2Send("no phone number\n");
        return SMS_NOT_PROPER_DATA;
    }

    //check sitio_prop before send sms
    if (!VerifySiteString(sitio_prop))
    {
        ChangeLedActivate(2);
        Usart2Send("no site saved\n");
        return SMS_NOT_PROPER_DATA;        
    }

    // data its good, try to send an sms
    Activation_12V_On();    // ACT_12V_ON;
    if (p_sms->alarm_input)
        strcpy(p_sms->buff, "Activacion en: ");

    if (p_sms->panel_input)
        sprintf(p_sms->buff, "Activo %03d en: ", p_sms->remote_number);
                        
    strcat(p_sms->buff, sitio_prop);
    if (FuncsGSMSendSMS (p_sms->buff, num_tel_rep) == resp_gsm_ok)
    {
        Usart2Send("OK\n");
        return SMS_SENT;
    }

    Usart2Send("delayed\n");    
    return SMS_NOT_SEND;
}


//answer 1 -> ok; 0 -> some error
unsigned char VerifyNumberString (char * number)
{
    unsigned char len = 0;
    len = strlen(number);

    if ((len > PHONE_NUMBER_MAX_LEN) || (len < 3))
        return 0;

    for (unsigned char i = 0; i < len; i++)
    {
        if (*(number + i) != '+')
        {
            if ((*(number + i) > '9') ||
                (*(number + i) < '0'))
                return 0;
        }
    }

    return 1;
}


//answer 1 -> ok; 0 -> some error
unsigned char VerifySiteString (char * site)
{
    unsigned char len = 0;
    len = strlen(site);

    if ((len > SITE_MAX_LEN) || (len < 3))
        return 0;

    for (unsigned char i = 0; i < len; i++)
    {
        if ((*(site + i) == 'á') ||
            (*(site + i) == 'é') ||
            (*(site + i) == 'í') ||
            (*(site + i) == 'ó') ||
            (*(site + i) == 'ú'))
        {
            // do nothing here
        }
        else if ((unsigned char) *(site + i) == 193)
            *(site + i) = 'A';
        else if ((unsigned char) *(site + i) == 201)
            *(site + i) = 'E';
        else if ((unsigned char) *(site + i) == 205)
            *(site + i) = 'I';
        else if ((unsigned char) *(site + i) == 211)
            *(site + i) = 'O';
        else if ((unsigned char) *(site + i) == 218)
            *(site + i) = 'U';
        else if ((*(site + i) > '~') ||
                 (*(site + i) < ' '))
            return 0;
    }

    return 1;
    
}

//--- end of file ---//

