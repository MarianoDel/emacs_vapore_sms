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
typedef struct parameters {

    char num_reportar [20];			//20
    char imei [24];					//44
    char num_propio [20];			//64
    char sitio_propio [172];			//236

    unsigned int acumm_wh;				//240
    unsigned int acumm_w2s;				//244
    unsigned short acumm_w2s_index;	//246

    unsigned char bkp_timer_reportar;		//247
    unsigned char bkp_envios_ok;			//248

    unsigned char send_energy_flag;	//249
    unsigned char bkp_prender_ring;			//250

    //dummys para completar
    unsigned char dummy1;			//251
    unsigned char dummy2;			//252
    unsigned char dummy3;			//253
    unsigned char dummy4;			//254
    unsigned char dummy5;			//255
    unsigned char dummy6;			//256

} parameters_typedef;

#define SITE_MAX_LEN    145
// #define SITE_MAX_LEN    20


// Configuration Change
#define envios_ok		(mem_conf.bkp_envios_ok)
#define envios_ok_change	(mem_conf.send_energy_flag & 0x80)
#define envios_ok_change_set	(mem_conf.send_energy_flag |= 0x80)
#define envios_ok_change_reset	(mem_conf.send_energy_flag &= 0x7F)

#define timer_rep		(mem_conf.bkp_timer_reportar)
#define timer_rep_change	(mem_conf.send_energy_flag & 0x40)
#define timer_rep_change_set	(mem_conf.send_energy_flag |= 0x40)
#define timer_rep_change_reset	(mem_conf.send_energy_flag &= 0xBF)

#define prender_ring		(mem_conf.bkp_prender_ring)
#define prender_ring_change	(mem_conf.send_energy_flag & 0x20)
#define prender_ring_change_set	(mem_conf.send_energy_flag |= 0x20)
#define prender_ring_change_reset	(mem_conf.send_energy_flag &= 0xDF)

#define num_tel_imei			(mem_conf.imei)
#define num_tel_prop			(mem_conf.num_propio)

// Flags
#define diag_prender		(mem_conf.send_energy_flag & 0x08)
#define diag_prender_set	(mem_conf.send_energy_flag |= 0x08)
#define diag_prender_reset	(mem_conf.send_energy_flag &= 0xF7)

#define diag_ringing		(mem_conf.send_energy_flag & 0x04)
#define diag_ringing_set	(mem_conf.send_energy_flag |= 0x04)
#define diag_ringing_reset	(mem_conf.send_energy_flag &= 0xFB)

#define sitio_prop        (mem_conf.sitio_propio)
#define sitio_prop_change		(mem_conf.send_energy_flag & 0x02)
#define sitio_prop_change_set		(mem_conf.send_energy_flag |= 0x02)
#define sitio_prop_change_reset	(mem_conf.send_energy_flag &= 0xFD)

#define num_tel_rep		(mem_conf.num_reportar)
#define num_tel_rep_change		(mem_conf.send_energy_flag & 0x01)
#define num_tel_rep_change_set		(mem_conf.send_energy_flag |= 0x01)
#define num_tel_rep_change_reset	(mem_conf.send_energy_flag &= 0xFE)


// #define diag_apagar		(mem_conf.send_energy_flag & 0x20)
// #define diag_apagar_set		(mem_conf.send_energy_flag |= 0x20)
// #define diag_apagar_reset	(mem_conf.send_energy_flag &= 0xDF)

//- End of Data Struct to Backup -----------------------------------------------

#endif    /* _PARAMETERS_H_ */

//--- end of file ---//

