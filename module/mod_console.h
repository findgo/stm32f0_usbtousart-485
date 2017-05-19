/**
  ******************************************************************************
  * @file   mod_console.h
  * @author  
  * @version 
  * @date    
  * @brief    console模块
  ******************************************************************************
  * @attention 20151117     v1.1   	jgb		重构 						20151118已测试
  *            20151120     v1.2 	jgb		修正3级优化 产生死机问题  		已测试
  ******************************************************************************
  */

#ifndef __MOD_CONSOLE_H_
#define __MOD_CONSOLE_H_

#include "app_cfg.h"


#include "mod_serial.h"


//! \brief argv max point Size
#define CONSOLE_ARGV_MAX_SIZE			(10)
//! \brief Buffer Size
#define CONSOLE_BUFFER_SIZE		        (20)
//! \brief histoty size
#define CONSOLE_HIS_CMD_NUM    			(5u)

//! \name console dynamic command type define
typedef bool CONSOLE_CMD_HANDLER(uint8_t argc, uint8_t *argv[], void *pcmd);

typedef struct {
	uint8_t *pchCMD;
	CONSOLE_CMD_HANDLER *fncmd_handler;
	uint8_t *pchHelp;
}console_command_t;


//! serial in a character
#define console_readbyte(pchByte)	Serial_Read(pchByte, 1)
//! serial out a character
#define console_writebyte(chByte)  Serial_WriteByte(chByte)
//!< serial out a string
#define console_writestring(pstring)	Serial_WriteStr(pstring)

#define console_extern_init()

//! \brief Default Static Command : help and clear you can add command at here
#ifndef CMD_SET_EXTERN_LIST
#define CMD_SET_EXTERN_LIST //{"led", &cmd_led_control, "\tled\t-- led control"}
#endif

bool console_str_cpy(uint8_t *toHere, const uint8_t *fromHere);
bool console_cmp_str(uint8_t *pchString1, uint8_t *pchString2);
uint8_t console_str_len(uint8_t *pchString);

void console_init(void);
bool console_cmd_register(const console_command_t* ptCMD, uint8_t chCount);
void console_task(void);

#endif

