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
#define SMS_NOT_SEND    0
#define SMS_SENT    1


// Module Exported Functions ---------------------------------------------------
unsigned char VerifyNumberString (char * number);
unsigned char VerifySiteString (char * site);
unsigned char VerifyAndSendSMS (char * message);

#endif    /* _SMS_DATA_H_ */

//--- end of file ---//
