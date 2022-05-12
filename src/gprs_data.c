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
#include "utils.h"


#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------



// Externals -------------------------------------------------------------------
extern parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------
unsigned char VerifyAPNString (char * apn);
unsigned char VerifyIPProtocol (char * ip_proto);
    

// Module Functions ------------------------------------------------------------
unsigned char VerifyAndSendGPRS (gprs_pckt_t * p_gprs)
{
    if (p_gprs->alarm_input)
        Usart2Send("External 12V: ");    // 12V input alarm activate

    if (p_gprs->panel_input)
        Usart2Send("Keypad ACT: ");
                    
    //check sitio_prop before send gprs
    if (!VerifySiteString(sitio_prop))
    {
        Usart2Send("no site saved\n");
        return GPRS_NOT_PROPER_DATA;
    }

    //check apn before send gprs
    if (!VerifyAPNString("gprs.personal.com"))
    {
        Usart2Send("no apn data\n");
        return GPRS_NOT_PROPER_DATA;
    }

    //check ip, port, protocol of server before send gprs
    // if (!VerifySocketData())
    // {
    //     Usart2Send("no socket data\n");
    //     return GPRS_NOT_PROPER_DATA;
    // }

    
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
    unsigned char answer = 0;

    answer = VerifySiteString (apn);

    if (answer)
    {
        unsigned char len = strlen(apn);
        for (unsigned char i = 0; i < len; i++)
        {
            if (*(apn+i) == ' ')
                answer = 0;
        }
    }
    
    return answer;
    
}


//answer 1 -> ok; 0 -> some error
unsigned char VerifyIPProtocol (char * ip_proto)
{
    unsigned char len = strlen(ip_proto);

    if (len != 3)
        return 0;

    if (!(strncmp(ip_proto, "TCP", sizeof("TCP") - 1)) ||
        !(strncmp(ip_proto, "UDP", sizeof("UDP") - 1)))
        return 1;
    
    return 0;
}

//--- end of file ---//

