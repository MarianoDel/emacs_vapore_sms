//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32G030
// ##
// #### FLASH_PROGRAM.H #######################
//---------------------------------------------

// Define to prevent recursive inclusion ---------------------------------------
#ifndef _FLASH_PROGRAM_H_
#define _FLASH_PROGRAM_H_

// Includes for Configurations -------------------------------------------------
#include "stm32g0xx.h"

// Configurations Defines ------------------------------------------------------
//- Where to Backup ------------------------------------------------------------
#define FLASH_PAGE_FOR_BKP    PAGE15
#define FLASH_ADDRESS_FOR_BKP    PAGE15_ADDR

//- Data Struct to Backup ------------------------------------------------------
//OJO esta estructuras deben estar alineadas en 4 bytes y ser multiplo de 8
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

// Define the STM32F10x FLASH Page Size depending on the used STM32 device
// si es mayor a 128K la pagina es de 2KB, sino 1KB

//#define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#define FLASH_PAGE_SIZE        2048
#define FLASH_PAGE_SIZE_DIV2    1024
#define FLASH_PAGE_SIZE_DIV4    512

// #define PAGE28_START	0
// #define PAGE29_START	256
// #define PAGE30_START	512
// #define PAGE31_START	768
// #define PAGE31_END		1023

#define PAGE15        (15U)
#define PAGE15_ADDR        ((uint32_t)0x08007800)
// #define PAGE28			((uint32_t)0x08007000)
// #define PAGE29			((uint32_t)0x08007400)
// #define PAGE30			((uint32_t)0x08007800)
// #define PAGE31			((uint32_t)0x08007C00)

// #define PAGE60			((uint32_t)0x0800F000)
// #define PAGE61			((uint32_t)0x0800F400)
// #define PAGE63			((uint32_t)0x0800FC00)


//de libreria st las tiene #include "stm32f0xx_flash.h"
/**
  * @brief  FLASH Status
  */
typedef enum {
    FLASH_BUSY = 1,
    FLASH_ERROR_WRP,
    FLASH_ERROR_PROGRAM,
    FLASH_COMPLETE,
    FLASH_TIMEOUT
      
} FLASH_Status;

/** @defgroup FLASH_Timeout_definition
  * @{
  */
#define FLASH_ER_PRG_TIMEOUT         ((uint32_t)0x000B0000)

/** @defgroup FLASH_Flags
  * @{
  */

#define FLASH_FLAG_BSY                 FLASH_SR_BSY1     /*!< FLASH Busy flag */
#define FLASH_FLAG_PGERR               FLASH_SR_PGSERR   /*!< FLASH Programming error flag */
#define FLASH_FLAG_WRPERR              FLASH_SR_WRPERR  /*!< FLASH Write protected error flag */
#define FLASH_FLAG_EOP                 FLASH_SR_EOP     /*!< FLASH End of Programming flag */

/******************  FLASH Keys  **********************************************/
#define FLASH_FKEY1        ((uint32_t)0x45670123)        /*!< Flash program erase key1 */
#define FLASH_FKEY2        ((uint32_t)0xCDEF89AB)        /*!< Flash program erase key2: used with FLASH_PEKEY1
                                                              to unlock the write access to the FPEC. */

#define IS_FLASH_CLEAR_FLAG(FLAG) ((((FLAG) & (uint32_t)0xFFFFFFCB) == 0x00000000) && ((FLAG) != 0x00000000))

#define IS_FLASH_GET_FLAG(FLAG)  (((FLAG) == FLASH_FLAG_BSY) || ((FLAG) == FLASH_FLAG_PGERR) || \
                                  ((FLAG) == FLASH_FLAG_WRPERR) || ((FLAG) == FLASH_FLAG_EOP))


// Module exported Functions ---------------------------------------------------
FLASH_Status Flash_ErasePage (uint8_t Page_Num, uint8_t lock_after_finish);
FLASH_Status Flash_WriteConfigurations (uint32_t * Address, uint32_t size);
// unsigned char WriteConfigurations (void);
// unsigned char WriteFlash(unsigned int * p, uint32_t p_addr, unsigned char with_lock, unsigned char len_in_4);

#endif    /* _FLASH_PROGRAM_H_ */

//--- end of file ---//

