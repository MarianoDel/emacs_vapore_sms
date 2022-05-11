//--------------------------------------------------
// #### VAPORE SMS COMMS PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### GPRS_DATA.C ################################
//--------------------------------------------------

// Includes --------------------------------------------------------------------
#include "gprs_data.h"
// #include "sms_data.h"
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
unsigned char VerifyAPNString (char * apn);


// Module Functions ------------------------------------------------------------
unsigned char VerifyAndSendGPRS (gprs_pckt_t * p_gprs)
{
    if (p_gprs->alarm_input)
        Usart2Send("External 12V: ");    // 12V input alarm activate

    if (p_gprs->panel_input)
        Usart2Send("Keypad ACT: ");
                    
    //check sitio_prop before send gprs
    // if (!VerifySiteString(sitio_prop))
    // {
    //     Usart2Send("no site saved\n");
    //     return GPRS_NOT_PROPER_DATA;
    // }

    // //check ip, port, protocol of server before send gprs
    // if (!VerifySocketData())
    // {
    //     Usart2Send("no socket data\n");
    //     return GPRS_NOT_PROPER_DATA;
    // }

    //check apn before send gprs
    if (!VerifyAPNString("gprs.personal.com"))
    {
        Usart2Send("no apn data\n");
        return GPRS_NOT_PROPER_DATA;
    }
    
    // data is good, try to send an sms
    // Activation_12V_On();    // ACT_12V_ON;
    // if (p_gprs->alarm_input)
    //     strcpy(p_gprs->buff, "Activacion en: ");

    // if (p_gprs->panel_input)
    //     sprintf(p_gprs->buff, "Activo %03d en: ", p_gprs->remote_number);
                        
    // strcat(p_gprs->buff, sitio_prop);
    // if (FuncsGSMSendSMS (p_gprs->buff, num_tel_rep) == resp_gsm_ok)
    // {
    //     Usart2Send("OK\n");
    //     return SMS_SENT;
    // }

    // Usart2Send("delayed\n");    
    return SMS_NOT_SEND;
}



//answer 1 -> ok; 0 -> some error
unsigned char VerifyAPNString (char * apn)
{
    unsigned char len = 0;
    len = strlen(apn);

    if ((len > APN_MAX_LEN) || (len < 3))
        return 0;

    for (unsigned char i = 0; i < len; i++)
    {
        if ((*(apn + i) == 'á') ||
            (*(apn + i) == 'é') ||
            (*(apn + i) == 'í') ||
            (*(apn + i) == 'ó') ||
            (*(apn + i) == 'ú'))
        {
            // do nothing here
        }
        else if ((unsigned char) *(apn + i) == 193)
            *(apn + i) = 'A';
        else if ((unsigned char) *(apn + i) == 201)
            *(apn + i) = 'E';
        else if ((unsigned char) *(apn + i) == 205)
            *(apn + i) = 'I';
        else if ((unsigned char) *(apn + i) == 211)
            *(apn + i) = 'O';
        else if ((unsigned char) *(apn + i) == 218)
            *(apn + i) = 'U';
        else if ((*(apn + i) > '~') ||
                 (*(apn + i) < ' '))
            return 0;
    }

    return 1;
    
}

//--- end of file ---//

