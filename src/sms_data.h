//--------------------------------------------------
// #### VAPORE SMS COMMS PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### SMS_DATA.H #################################
//--------------------------------------------------

// Define to prevent recursive inclusion ---------------------------------------
#ifndef _SMS_DATA_H_
#define _SMS_DATA_H_


// Exported Types Constants and Macros -----------------------------------------
typedef struct {
    unsigned char alarm_input;
    unsigned char panel_input;
    unsigned short remote_number;
    char * buff;
    
} sms_pckt_t;

#define SMS_NOT_SEND    0
#define SMS_SENT    1
#define SMS_NOT_PROPER_DATA    2


// Module Exported Functions ---------------------------------------------------
unsigned char VerifyAndSendSMS (sms_pckt_t * p_sms);

#endif    /* _SMS_DATA_H_ */

//--- end of file ---//
