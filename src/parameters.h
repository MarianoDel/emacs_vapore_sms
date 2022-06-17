//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### PARAMETERS.H ##########################
//---------------------------------------------

#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

//- Data Struct to Backup ------------------------------------------------------
// Beware this struct must be 4 bytes align and be multiple of 8
#define PHONE_NUMBER_MAX_LEN    19
#define SITE_MAX_LEN    150    // sms msg len - 15 from rep sitio
typedef struct parameters {

    char num_reportar [PHONE_NUMBER_MAX_LEN + 1];    //20
    char imei [24];    //44
    char num_propio [PHONE_NUMBER_MAX_LEN + 1];    //64
    char sitio_propio [SITE_MAX_LEN + 1];    //215

    // conf backup flags
    unsigned char bkp_envios_ok;    //216
    unsigned char bkp_timer_reportar;    //217
    unsigned char bkp_prender_ring;    //218
    unsigned char bkp_sense_bat;    //219
    unsigned char memory_saved_flag;    //220

    // conf running flags
    unsigned int internal_comms_flag;    //224 4 bytes if its aligned to 4

    // gprs settings
    char ip1 [16];    //240
    char ip_port1 [6];    //246
    char ip2 [16];    //262
    char ip_port2 [6];    //268
    char ip_proto [4];    //272

    char apn [68];    //340

    // monitor account
    char client_number [7];    //347
    unsigned char bkp_socket_use;    //348
    unsigned short keepalive;    //350 2 bytes if its aligned to 4

    // // // complete alignment with dummys
    unsigned char dummy1 [50];    //400    (multiply of eight)

} parameters_typedef;



// Configuration Changes (second nibble)
#define envios_ok		(mem_conf.bkp_envios_ok)
#define envios_ok_change	(mem_conf.internal_comms_flag & 0x80)
#define envios_ok_change_set	(mem_conf.internal_comms_flag |= 0x80)
#define envios_ok_change_reset	(mem_conf.internal_comms_flag &= 0x7F)

#define timer_rep		(mem_conf.bkp_timer_reportar)
#define timer_rep_change	(mem_conf.internal_comms_flag & 0x40)
#define timer_rep_change_set	(mem_conf.internal_comms_flag |= 0x40)
#define timer_rep_change_reset	(mem_conf.internal_comms_flag &= 0xBF)

#define prender_ring		(mem_conf.bkp_prender_ring)
#define prender_ring_change	(mem_conf.internal_comms_flag & 0x20)
#define prender_ring_change_set	(mem_conf.internal_comms_flag |= 0x20)
#define prender_ring_change_reset	(mem_conf.internal_comms_flag &= 0xDF)

#define battery_check		(mem_conf.bkp_sense_bat)
#define battery_check_change	(mem_conf.internal_comms_flag & 0x10)
#define battery_check_change_set	(mem_conf.internal_comms_flag |= 0x10)
#define battery_check_change_reset	(mem_conf.internal_comms_flag &= 0xEF)

#define num_tel_imei			(mem_conf.imei)
#define num_tel_prop			(mem_conf.num_propio)

// Original Flags (first nibble)
#define diag_prender		(mem_conf.internal_comms_flag & 0x08)
#define diag_prender_set	(mem_conf.internal_comms_flag |= 0x08)
#define diag_prender_reset	(mem_conf.internal_comms_flag &= 0xF7)

#define diag_ringing		(mem_conf.internal_comms_flag & 0x04)
#define diag_ringing_set	(mem_conf.internal_comms_flag |= 0x04)
#define diag_ringing_reset	(mem_conf.internal_comms_flag &= 0xFB)

#define sitio_prop        (mem_conf.sitio_propio)
#define sitio_prop_change		(mem_conf.internal_comms_flag & 0x02)
#define sitio_prop_change_set		(mem_conf.internal_comms_flag |= 0x02)
#define sitio_prop_change_reset	(mem_conf.internal_comms_flag &= 0xFD)

#define num_tel_rep		(mem_conf.num_reportar)
#define num_tel_rep_change		(mem_conf.internal_comms_flag & 0x01)
#define num_tel_rep_change_set		(mem_conf.internal_comms_flag |= 0x01)
#define num_tel_rep_change_reset	(mem_conf.internal_comms_flag &= 0xFE)

// Extended Flags (third nibble)
#define diag_battery		(mem_conf.internal_comms_flag & 0x0800)
#define diag_battery_set	(mem_conf.internal_comms_flag |= 0x0800)
#define diag_battery_reset	(mem_conf.internal_comms_flag &= 0xF7FF)

#define diag_battery_low_voltage	(mem_conf.internal_comms_flag & 0x0400)
#define diag_battery_low_voltage_set	(mem_conf.internal_comms_flag |= 0x0400)
#define diag_battery_low_voltage_reset	(mem_conf.internal_comms_flag &= 0xFBFF)

#define diag_battery_disconnect_voltage		(mem_conf.internal_comms_flag & 0x0200)
#define diag_battery_disconnect_voltage_set	(mem_conf.internal_comms_flag |= 0x0200)
#define diag_battery_disconnect_voltage_reset	(mem_conf.internal_comms_flag &= 0xFDFF)

#define diag_battery_good_voltage	(mem_conf.internal_comms_flag & 0x0100)
#define diag_battery_good_voltage_set	(mem_conf.internal_comms_flag |= 0x0100)
#define diag_battery_good_voltage_reset	(mem_conf.internal_comms_flag &= 0xFEFF)

// Extended Flags (forth nibble)
#define socket_use_enable    (mem_conf.bkp_socket_use)
#define socket_conf_change	(mem_conf.internal_comms_flag & 0x1000)
#define socket_conf_change_set	(mem_conf.internal_comms_flag |= 0x1000)
#define socket_conf_change_reset	(mem_conf.internal_comms_flag &= 0xEFFF)




//- End of Data Struct to Backup -----------------------------------------------

#endif    /* _PARAMETERS_H_ */

//--- end of file ---//

