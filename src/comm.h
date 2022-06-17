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
#define comms_activate_sms_flag    (comms_global_flag & 0x0001)
#define comms_activate_sms_flag_set    (comms_global_flag |= 0x0001)
#define comms_activate_sms_flag_reset    (comms_global_flag &= 0xFFFE)

#define comms_activate_ringing_flag    (comms_global_flag & 0x0002)
#define comms_activate_ringing_flag_set    (comms_global_flag |= 0x0002)
#define comms_activate_ringing_flag_reset    (comms_global_flag &= 0xFFFD)

#define comms_battery_check_flag    (comms_global_flag & 0x0004)
#define comms_battery_check_flag_set    (comms_global_flag |= 0x0004)
#define comms_battery_check_flag_reset    (comms_global_flag &= 0xFFFB)

#define comms_battery_low_flag    (comms_global_flag & 0x0008)
#define comms_battery_low_flag_set    (comms_global_flag |= 0x0008)
#define comms_battery_low_flag_reset    (comms_global_flag &= 0xFFF7)

#define comms_battery_disconnect_flag    (comms_global_flag & 0x0010)
#define comms_battery_disconnect_flag_set    (comms_global_flag |= 0x0010)
#define comms_battery_disconnect_flag_reset    (comms_global_flag &= 0xFFEF)

#define comms_battery_good_flag    (comms_global_flag & 0x0020)
#define comms_battery_good_flag_set    (comms_global_flag |= 0x0020)
#define comms_battery_good_flag_reset    (comms_global_flag &= 0xFFDF)

#define comms_memory_save_flag    (comms_global_flag & 0x0040)
#define comms_memory_save_flag_set    (comms_global_flag |= 0x0040)
#define comms_memory_save_flag_reset    (comms_global_flag &= 0xFFBF)



// Module Exported Functions ---------------------------------------------------
void CommsProcessSMSPayload (char * orig_num, char * payload);


#endif    /* _COMM_H_ */

//--- end of file ---//
