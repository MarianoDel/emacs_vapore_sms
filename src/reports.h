//--------------------------------------------------
// #### VAPORE SMS COMMS PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### REPORTS.H ##################################
//--------------------------------------------------

// Define to prevent recursive inclusion ---------------------------------------
#ifndef _REPORTS_H_
#define _REPORTS_H_


// Exported Types Constants and Macros -----------------------------------------
typedef struct {
    char * buffer;
    unsigned char attempts;
    unsigned char media_flags;

} reports_st;


#define REPORT_SENDING    0
#define REPORT_SENT    1
#define REPORT_NOT_SENT    2

#define REPORT_BY_IP1    0x01
#define REPORT_BY_IP2    0x02
#define REPORT_BY_SMS    0x04


// Module Exported Functions ---------------------------------------------------
unsigned char ReportsVerifyAndSend (reports_st * report);
void ReportsTimeouts (void);


#endif    /* _REPORTS_H_ */

//--- end of file ---//
