//--------------------------------------------------
// #### VAPORE SMS COMMS PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### SMS_GPRS_DATA.H ############################
//--------------------------------------------------

// Define to prevent recursive inclusion ---------------------------------------
#ifndef _SMS_GPRS_DATA_H_
#define _SMS_GPRS_DATA_H_

// Exported Types Constants and Macros -----------------------------------------


#define GPRS_WORKING    0
#define GPRS_SENT    1
#define GPRS_NOT_SEND    2

#define IP1    1
#define IP2    2


// Module Exported Functions --------------------------------------------------
unsigned char VerifyGPRSConfig_IP1 (char * payload);
unsigned char VerifyGPRSConfig_IP2 (char * payload);
unsigned char VerifySocketData (unsigned char which_ip);

unsigned char VerifySMSData (void);
unsigned char VerifyNumberString (char * number);
unsigned char VerifySiteString (char * site);

#endif    /* _SMS_GPRS_DATA_H_ */

//--- end of file ---//
