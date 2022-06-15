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

// Exported Types Constants and Macros -----------------------------------------


#define GPRS_WORKING    0
#define GPRS_SENT    1
#define GPRS_NOT_SEND    2

#define IP1    1
#define IP2    2


// Module Exported Functions ---------------------------------------------------
// unsigned char VerifyAndSendGPRS (char * message);
unsigned char VerifyGPRSConfig (char * payload);
unsigned char VerifySocketData (unsigned char which_ip);

#endif    /* _GPRS_DATA_H_ */

//--- end of file ---//
