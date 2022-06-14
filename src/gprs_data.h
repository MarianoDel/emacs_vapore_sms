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

// #define APN_MAX_LEN    30

// Module Exported Functions ---------------------------------------------------
unsigned char VerifyAndSendGPRS (char * message);
unsigned char GPRS_Config (char * payload);
unsigned char VerifySocketData (void);

#endif    /* _GPRS_DATA_H_ */

//--- end of file ---//
