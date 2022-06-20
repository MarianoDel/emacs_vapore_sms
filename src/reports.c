//--------------------------------------------------
// #### VAPORE SMS COMMS PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### REPORTS.C ##################################
//--------------------------------------------------

// Includes --------------------------------------------------------------------
#include "reports.h"
#include "sms_gprs_data.h"
#include "parameters.h"
#include "usart.h"
#include "hard.h"
#include "funcs_gsm.h"


#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    CHECK_GPRS_IP1,
    SEND_IP1,
    CHECK_GPRS_IP2,    
    SEND_IP2,
    CHECK_SMS,
    SEND_SMS

} report_state_e;

#define ATTEMPTS_TIMER    3000


// Externals -------------------------------------------------------------------
extern parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------
report_state_e report_state = CHECK_GPRS_IP1;
unsigned char report_repeat_cnt = 0;
volatile unsigned short report_timer = 0;


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
unsigned char ReportsVerifyAndSend (reports_st * report)
{
    unsigned char answer = REPORT_SENDING;
    
    switch (report_state)
    {
    case CHECK_GPRS_IP1:
        if (!(report->media_flags & REPORT_BY_IP1))
        {
            report_state = CHECK_SMS;
            break;
        }

        if (VerifySocketData(IP1))
        {
            report_repeat_cnt = report->attempts;
            report_state++;
        }
        else
            report_state = CHECK_SMS;
        
        break;

    case SEND_IP1:
        if (report_timer)
            break;

        answer = FuncsGSMSendGPRS (report->buffer, IP1);

        if (answer == resp_gsm_ok)
        {
            Usart2Debug("report sent by gprs ip1\n", 1);
            answer = REPORT_SENT;
        }
        else if (answer > resp_gsm_ok)
        {
            if (report_repeat_cnt)
            {
                report_repeat_cnt--;
                report_timer = ATTEMPTS_TIMER;
            }
            else
            {
                Usart2Debug("gprs ip1 connection fail, go to ip2\n", 1);
                report_state++;
            }
            answer = REPORT_SENDING;
        }
        else
            answer = REPORT_SENDING;
        
        break;

    case CHECK_GPRS_IP2:
        if (!(report->media_flags & REPORT_BY_IP2))
        {
            report_state = CHECK_SMS;
            break;
        }

        if (VerifySocketData(IP2))
        {
            report_repeat_cnt = report->attempts;
            report_state++;
        }
        else
            report_state = CHECK_SMS;
        
        break;

    case SEND_IP2:
        if (report_timer)
            break;

        answer = FuncsGSMSendGPRS (report->buffer, IP2);

        if (answer == resp_gsm_ok)
        {
            Usart2Debug("report sent by gprs ip2\n", 1);
            answer = REPORT_SENT;
        }
        else if (answer > resp_gsm_ok)
        {
            if (report_repeat_cnt)
            {
                report_repeat_cnt--;
                report_timer = ATTEMPTS_TIMER;
            }
            else
            {
                Usart2Debug("gprs ip2 connection fail, go to sms\n", 1);
                report_state++;
            }
            answer = REPORT_SENDING;
        }
        else
            answer = REPORT_SENDING;

        break;

    case CHECK_SMS:
        if(!(report->media_flags & REPORT_BY_SMS))
        {
            answer = REPORT_NOT_SENT;
            break;
        }
        
        if (VerifySMSData())
        {
            report_repeat_cnt = report->attempts;
            report_state++;
        }
        break;

    case SEND_SMS:
        if (report_timer)
            break;

        if (FuncsGSMSendSMS (report->buffer, mem_conf.num_reportar) == resp_gsm_ok)
        {
            Usart2Debug("report sent by sms\n", 1);
            answer = REPORT_SENT;
        }
        else
        {
            if (report_repeat_cnt)
            {
                report_repeat_cnt--;
                report_timer = ATTEMPTS_TIMER;
            }
            else
                answer = REPORT_NOT_SENT;

        }
        break;
        
    default:
        report_state = CHECK_GPRS_IP1;
        break;
    }

    if (answer != REPORT_SENDING)
        report_state = CHECK_GPRS_IP1;

    return answer;
}


void ReportsTimeouts (void)
{
    if (report_timer)
        report_timer--;
    
}
//--- end of file ---//

