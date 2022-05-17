//--------------------------------------------------
// #### VAPORE SMS COMMS PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### GPRS_DATA.H ################################
//--------------------------------------------------

// Define to prevent recursive inclusion ---------------------------------------
#ifndef _GPRS_DATA_H_
#define _GPRS_DATA_H_

#include "sms_data.h"
// Exported Types Constants and Macros -----------------------------------------
#define gprs_pckt_t    sms_pckt_t

// typedef struct {
//     unsigned char alarm_input;
//     unsigned char panel_input;
//     unsigned short remote_number;
//     char * buff;
    
// } sms_pckt_t;

#define GPRS_WORKING    0
#define GPRS_SENT    1
#define GPRS_NOT_SEND    2
#define GPRS_NOT_PROPER_DATA    3

#define APN_MAX_LEN    30

// Module Exported Functions ---------------------------------------------------
unsigned char VerifyAndSendGPRS (gprs_pckt_t * p_gprs);
unsigned char GPRS_Config (char * payload, unsigned char ipdomain);

#endif    /* _GPRS_DATA_H_ */

//--- end of file ---//
