//---------------------------------------------------------
// #### PROYECTO COMUNICADOR VAPORE SMS - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### FUNCS_GSM.H #######################################
//---------------------------------------------------------

// Define to prevent recursive inclusion ---------------------------------------
#ifndef _FUNCS_GSM_H_
#define _FUNCS_GSM_H_


// Exported Types Constants and Macros -----------------------------------------
typedef enum {
	gsm_state_reset = 0,
	gsm_state_verify_at,
	gsm_state_wait_cpin,
	gsm_state_echo_disable,
	gsm_state_sms_mode,
	gsm_state_sms_character,
	gsm_state_verify_fully_func,
	gsm_state_wait_reg,
	gsm_state_verify_reg,
	gsm_state_dell_all,
	gsm_state_get_imei,
	gsm_state_shutdown,
	gsm_state_shutdown_2,        
	gsm_state_stop_wait,
	gsm_state_shutdown_always,
	gsm_state_stop_always,
	gsm_state_ready,
	gsm_state_sending_conf,
	gsm_state_sending_sms,
	gsm_state_sending_gprs,
        gsm_state_getting_location,
	gsm_state_command_answer,
	gsm_state_idle,
	gsm_state_connecting,
	gsm_state_connected,
        gsm_state_check_rssi,
        gsm_state_check_network,        
	gsm_state_reading_sms,
	gsm_state_disconnected

} t_GsmState;

typedef enum {
	resp_gsm_continue = 0,
	resp_gsm_ok,
	resp_gsm_error,
	resp_gsm_timeout

} t_RespGsm;


// Exported Constants ----------------------------------------------------------
//FLAGS de string del gsm
#define GSM_RESET_FLAG    0x8000
#define GSM_SET_CALL    0x0001
#define GSM_SET_SMS    0x0002
#define GSM_SET_CPIN    0x0004
#define GSM_SET_POWER_DOWN    0x4000
#define GSM_RESET_ALL    (GSM_RESET_FLAG|GSM_SET_CALL|GSM_SET_SMS|GSM_SET_CPIN|GSM_SET_POWER_DOWN)


// FLAGS strings flags from gprs
#define GPRS_RESET_FLAG    0x80
#define GPRS_ENABLE_FLAGS    0x40
#define GPRS_CONN_OK    0x01
#define GPRS_CONN_FAIL    0x02


// Exported Functions ----------------------------------------------------------
void FuncsGSM (void);
void FuncsGSMReset (void);
unsigned char FuncsGSMReady (void);
unsigned char FuncsGSMStateAsk (void);
void FuncsGSMShutdown (void);
void FuncsGSMShutdownAlways (void);
unsigned char FuncsGSMSendSMS (char *, char *);
unsigned char FuncsGSMSendGPRS (char *, unsigned char which_ip);

void FuncsGSMMessageFlags (unsigned short);
unsigned short FuncsGSMMessageFlagsAsk (void);
unsigned char FuncsGSMCommandAnswer (char * , char * );
void FuncsGSMParser (unsigned char *, unsigned char *);
void FuncsGSMGetSMSPayloadCallback (char *, char *);
void FuncsGSMTimeoutCounters (void);

void FuncsGSMGPRSFlags (unsigned char flag);
unsigned short FuncsGSMGPRSFlagsAsk (void);

void FuncsGSM_ServerAnswer_Set (void);
void FuncsGSM_ServerAnswer_Reset (void);
unsigned char FuncsGSM_ServerAnswer_Get (void);

unsigned char FuncsGSM_GetLocation (void);

#endif    /* _FUNCS_GSM_H_ */

//--- end of file ---//
