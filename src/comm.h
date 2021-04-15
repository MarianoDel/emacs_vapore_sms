//--------------------------------------------------
// #### VAPORE SMS COMMS PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### COMM.H #####################################
//--------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _COMM_H_
#define _COMM_H_


// Module Exported Types Constants and Macros ----------------------------------


// Module Exported Functions ---------------------------------------------------
void CommsProcessSMSPayload (char * orig_num, char * payload);
unsigned char VerifyNumberString (char * number);
unsigned char VerifySiteString (char * site);

#endif    /* _COMM_H_ */

//--- end of file ---//
