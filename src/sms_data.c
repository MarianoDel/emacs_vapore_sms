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


// Module Functions ------------------------------------------------------------
unsigned char VerifyAndSendSMS (char * message)
{
    if (FuncsGSMSendSMS (message, num_tel_rep) == resp_gsm_ok)
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

