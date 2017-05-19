/**
  ******************************************************************************
  * @file   mod_console.c
  * @author  
  * @version 
  * @date    
  * @brief    consoleģ��
  ******************************************************************************
  * @attention 20151117     v1.1   	jgb		�ع� 	
  ******************************************************************************
  */

#include "mod_console.h"

//!< 	MACROS
//! \brief char interval timer
#define CONSOLE_CHAR_INTERVAL_TIME      (100u)
//! \brief define space key ascii code
#define CONSOLE_SPACE                   (0x20u)
//! \brief Default Separator : Space
#define CONSOLE_SEPARATOR               " "
//! \brief Default Separator : clear
#define CONSOLE_CLEAR_CODE				0x0C
//���������ܴ�С
#define CONSOLE_CMD_CONSOLE_BUFFER_SIZE		(CONSOLE_BUFFER_SIZE + 1)
//!< \bad command note
#define CONSOLE_BADCOMMAND_PRINT	"\tBad command or file name,and use \'help\' or \'ls\' to get more info!"


//!< ��������
#define KEY_CODE_ESC    0x00001BUL
#define KEY_CODE_F1     0x1B4F50UL
#define KEY_CODE_F3     0x1B4F52UL
#define KEY_CODE_UP     0x1B5B41UL
#define KEY_CODE_DOWN   0x1B5B42UL

typedef struct {
	uint8_t index; // ָ������
	uint8_t chCmd[CONSOLE_CMD_CONSOLE_BUFFER_SIZE];         	
} console_curcmd_t;

typedef struct {
	uint8_t chCmdLen; // ָ���
	uint8_t chCmd[CONSOLE_CMD_CONSOLE_BUFFER_SIZE];         	
} console_cmd_t;

typedef struct {
	uint8_t s_chCurHisCmdPtr;	// ��ǰ��ʷ�����±�
	uint8_t s_chLastCmdPtr;		// ���һ����ʷ�����±�
	uint8_t s_chHisCmdNum;		// ��ʷ��������
	uint8_t s_chCurHisCmdNum;	// ��ǰ������ʷ����������
	console_cmd_t s_tHisCmd[CONSOLE_HIS_CMD_NUM];
} his_cmd_queue_t;

static bool cmd_help_hander(uint8_t argc, uint8_t *argv[], void *pcmd);
static bool cmd_clear_hander(uint8_t argc, uint8_t *argv[], void *pcmd);

static console_command_t s_tCMD[] = {	
	{"clear", &cmd_clear_hander, "\tclear\t-- Clear Screen"},
	{"help",  &cmd_help_hander,  "\thelp\t-- How to use the console"},
	{"ls",    &cmd_help_hander,  "\tls\t-- list all console command"},
	CMD_SET_EXTERN_LIST
};

//!< ���յ��������
static console_curcmd_t s_chCurCmd = {0,{'\0'}};
//!< ��ʷ����
static his_cmd_queue_t s_chHisCmd;

//!< dynamic command ��̬�ⲿ����
static const console_command_t *s_ptDynaCMD = NULL;
static uint8_t s_chDynaCount = 0;
//static uint8_t s_argc = 0;
static uint8_t *s_argv[CONSOLE_ARGV_MAX_SIZE];//ָ������

/*============================ IMPLEMENTATION ================================*/

//!< ok
static void console_hiscmd_save(console_cmd_t *ptHistory)
{
	uint8_t i;
	his_cmd_queue_t *ptQ = &s_chHisCmd;
	
	// ������ʷ����
	for ( i = 0; i <= ptHistory->chCmdLen; i++ ){ 	 // ��'\0'Ҳ����
		ptQ->s_tHisCmd[ptQ->s_chLastCmdPtr].chCmd[i] = ptHistory->chCmd[i];
	}
	ptQ->s_tHisCmd[ptQ->s_chLastCmdPtr].chCmdLen = ptHistory->chCmdLen;

	if ( ptQ->s_chHisCmdNum < CONSOLE_HIS_CMD_NUM ){ // ��ʷ��������
		ptQ->s_chHisCmdNum++;
	}
	ptQ->s_chCurHisCmdNum = ptQ->s_chHisCmdNum - 1; // ���µ�ǰ�������������

	ptQ->s_chCurHisCmdPtr = ptQ->s_chLastCmdPtr;  // ���µ�ǰ��ʷ�����±�
	if ( ++ptQ->s_chLastCmdPtr >= CONSOLE_HIS_CMD_NUM){ //������������� ����һ�α��渲�����������
		ptQ->s_chLastCmdPtr = 0;
	}				
}

//!< ok
static bool console_display_backspace(uint8_t chNum)
{
	if(chNum == 0)
		return true;

	while(chNum--){
		console_writebyte('\b'); 	//����һ��
		console_writebyte(' '); //����հ�
		console_writebyte('\b'); //����һ��
	}
	
	return true;
}
//!< ok
static void console_prn_rn(void)
{
	console_writebyte('\r'); //����س�
	console_writebyte('\n'); //����һ��
}


#define FRONT_END_CHECK24_START     0
#define FRONT_END_CHECK24_SEC       1
#define FRONT_END_CHECK24_THR       2
#define FRONT_END_CHECK24_RESET()   do{s_chState = 0;}while(0)
#define FRONT_END_CHECK24_TIMEOUT_NUM       500UL
/**
  * @brief	 ��⹦�ܰ�������(���ܰ���24λ����)
  * @param	pwCmd  -- ���Է��ؼ�⵽�Ĺ��ܼ�����ĵ�ַ
  * @note	
  * @retval 	FSM stutas detail in common_type.h
  */
static fsm_rt_t front_end_check24(uint32_t *pwCmd)
{
    static uint8_t s_chState = FRONT_END_CHECK24_START;
    static uint32_t s_wTimeOut;     // ��ʱ������
    static uint8_t s_chTemp;
    static uint32_t s_wCmdTemp;

	assert_param(NULL != pwCmd);

    *pwCmd = 0;
    switch (s_chState) 
	{
        case FRONT_END_CHECK24_START:
            s_wTimeOut = 0;
            s_chState = FRONT_END_CHECK24_SEC;      // fall through
            
        case FRONT_END_CHECK24_SEC:
            if( s_wTimeOut < FRONT_END_CHECK24_TIMEOUT_NUM ) {
				if(console_readbyte(&s_chTemp)) {
					s_wCmdTemp = (KEY_CODE_ESC << 16) + ( ((uint32_t)s_chTemp) << 8 );
					s_wTimeOut = 0;
					s_chState = FRONT_END_CHECK24_THR;
				} else {
					s_wTimeOut++;
				}
            } else {                    // ��ʱ
                *pwCmd = KEY_CODE_ESC;  // ����Esc������
                FRONT_END_CHECK24_RESET();
                return fsm_rt_cpl;
            }
            break;
			
        case FRONT_END_CHECK24_THR:
			if( s_wTimeOut < FRONT_END_CHECK24_TIMEOUT_NUM ) {
				if( console_readbyte(&s_chTemp) ) {
					s_wCmdTemp += s_chTemp;
					s_wTimeOut = 0;
					*pwCmd = s_wCmdTemp;
					FRONT_END_CHECK24_RESET();
					return fsm_rt_cpl;
				} else {
					s_wTimeOut++;
				}
            } else {                    // ��ʱ
                *pwCmd = KEY_CODE_ESC;  // ����Esc������
                FRONT_END_CHECK24_RESET();
                return fsm_rt_cpl;
            }
            break;
			
			default: break;
    	}
    return fsm_rt_on_going;
}
/**
  * @brief	 ʵ�� F1 ����
  * @param	None
  * @note	
  * @retval none
  */
static void front_fun_key_f1(void)
{
	uint8_t s_chTemp;
	
	if( s_chCurCmd.index < s_chHisCmd.s_tHisCmd[s_chHisCmd.s_chCurHisCmdPtr].chCmdLen ) {
		s_chTemp = s_chHisCmd.s_tHisCmd[s_chHisCmd.s_chCurHisCmdPtr].chCmd[s_chCurCmd.index];
		s_chCurCmd.chCmd[s_chCurCmd.index++] = s_chTemp;
		console_writebyte(s_chTemp);
	} 
}
/**
  * @brief	ʵ�� F3 ���ܵĺ���
  * @param	None
  * @note	
  * @retval 	none
  */
static void front_fun_key_f3(void)
{
	uint8_t chLen;

	if ( s_chCurCmd.index < s_chHisCmd.s_tHisCmd[s_chHisCmd.s_chCurHisCmdPtr].chCmdLen ) {
		chLen =s_chHisCmd.s_tHisCmd[s_chHisCmd.s_chCurHisCmdPtr].chCmdLen;
		for( ; s_chCurCmd.index < chLen; s_chCurCmd.index++ ) {
			s_chCurCmd.chCmd[s_chCurCmd.index] = s_chHisCmd.s_tHisCmd[s_chHisCmd.s_chCurHisCmdPtr].chCmd[s_chCurCmd.index];
			console_writebyte(s_chCurCmd.chCmd[s_chCurCmd.index]);
		}
	}
}
/**
  * @brief	ʵ�� up ���ܵĺ���  ����һ��ָ��
  * @param	None
  * @note	
  * @retval  none
  */
static void front_fun_key_up(void)
{
	uint8_t chLen;

	if( s_chHisCmd.s_chHisCmdNum > 0 ){             // ����ʷ����    
		console_display_backspace(s_chCurCmd.index); //��ǰȫɾ
		chLen =s_chHisCmd.s_tHisCmd[s_chHisCmd.s_chCurHisCmdPtr].chCmdLen;
		for( s_chCurCmd.index = 0; s_chCurCmd.index < chLen; s_chCurCmd.index++ ) {
			s_chCurCmd.chCmd[s_chCurCmd.index] = s_chHisCmd.s_tHisCmd[s_chHisCmd.s_chCurHisCmdPtr].chCmd[s_chCurCmd.index];
			console_writebyte(s_chCurCmd.chCmd[s_chCurCmd.index]);
		}
		if(s_chHisCmd.s_chCurHisCmdPtr == 0){
			s_chHisCmd.s_chCurHisCmdPtr = s_chHisCmd.s_chCurHisCmdNum;
		}else{
			s_chHisCmd.s_chCurHisCmdPtr--;
		}
	}
}
/**
  * @brief	ʵ�� down ���ܵĺ���  ����һ��ָ��
  * @param	None
  * @note	
  * @retval  none
  */
static void front_fun_key_down(void)
{
	uint8_t chLen;

	if( s_chHisCmd.s_chHisCmdNum > 0 ){ 			// ����ʷ����	 
		console_display_backspace(s_chCurCmd.index); //��ǰȫɾ
		chLen =s_chHisCmd.s_tHisCmd[s_chHisCmd.s_chCurHisCmdPtr].chCmdLen;
		for( s_chCurCmd.index = 0; s_chCurCmd.index < chLen; s_chCurCmd.index++ ) {
			s_chCurCmd.chCmd[s_chCurCmd.index] = s_chHisCmd.s_tHisCmd[s_chHisCmd.s_chCurHisCmdPtr].chCmd[s_chCurCmd.index];
			console_writebyte(s_chCurCmd.chCmd[s_chCurCmd.index]);
		}
		if(s_chHisCmd.s_chCurHisCmdPtr == s_chHisCmd.s_chCurHisCmdNum){
			s_chHisCmd.s_chCurHisCmdPtr = 0;
		}else{
			s_chHisCmd.s_chCurHisCmdPtr++;
		}
	}
}
/**
  * @brief	�ַ����ܴ�����
  * @param	wKey --�ַ�����
  * @note	
  * @note	 
  * @note	
  * @retval FSM stutas detail in common_type.h
  */
static void front_end_function_key(uint32_t wKey)
{
	switch(wKey) {
		case KEY_CODE_F1:
			front_fun_key_f1();
			break;

		case KEY_CODE_F3:
			front_fun_key_f3();
			break;
			
		case KEY_CODE_UP:
			front_fun_key_up();
			break;

		case KEY_CODE_DOWN:
			front_fun_key_down();
			break;
		default:            // δ���尴��
			 break;
	}
}


//!< IsSeparator ok
static bool IsSeparator(uint8_t chByte)
{
	//!< command separator
	const uint8_t c_chSeparator[] = CONSOLE_SEPARATOR;
	uint8_t const *pchBuffer = &c_chSeparator[0];

	while ('\0' != *pchBuffer) {
		if (chByte == *pchBuffer) {
			return true;
		}	
		pchBuffer++;
	}
	
	return false;
}

//token ok
static void token(console_cmd_t *pcmd, uint8_t *pchCount)
{
	uint8_t *pchRead;
	uint8_t *pchWrite;
	bool s_bLast = true;
	bool bSeparator;
	uint8_t cmdlen = 0;
	
	pchRead = &pcmd->chCmd[0];
	pchWrite = &pcmd->chCmd[0];
	(*pchCount) = 0;
	for(;;){
		if ('\0' != *pchRead) {
			bSeparator = IsSeparator(*pchRead);
			if (bSeparator) {
				if (!s_bLast) {
					*pchWrite++ = '\0';
					cmdlen++;
				}
			}else{
				if (s_bLast) {						
					(*pchCount)++;
				}
				*pchWrite++ = *pchRead;
				cmdlen++;
			}
			s_bLast = bSeparator;
			pchRead++;				
		} else {
			*pchWrite = '\0';
			pcmd->chCmdLen = cmdlen + 1;
			return;
		}
	}
}

#define FRONT_END_START 					0
#define FRONT_END_INPUT						1
#define FRONT_END_CHECK24					2
#define FRONT_END_TAKEN						3
#define FRONT_END_RESET_FSM()    			do{s_chState = 0;}while(0)

static fsm_rt_t front_end(uint8_t **pchTokens, uint8_t *pchCount)
{
	static uint8_t s_chState = FRONT_END_START;
	uint8_t chKey;
	uint32_t wKey;
	console_curcmd_t *pcmd = &s_chCurCmd;
	
	switch (s_chState) {
		case FRONT_END_START:
			pcmd->index = 0;
			console_writebyte('>'); // ���'>'
			s_chState = FRONT_END_INPUT;
			//!< break;
			
		case FRONT_END_INPUT:
			if(console_readbyte(&chKey)) {
				if ('\b' == chKey) {	   			 //!<  Backspace(BS) �˸�
					if (0 != pcmd->index) {
						pcmd->index--;
						console_display_backspace(1); // ɾ��һ���ַ�
					}		
				} else if ('\r' == chKey) {	    // �������� �س�
					console_prn_rn(); 			
					if(0 == pcmd->index){
						s_chState = FRONT_END_START;
					}else{
						pcmd->chCmd[pcmd->index] = '\0';
						FRONT_END_RESET_FSM();
						s_chState = FRONT_END_TAKEN;
					}
				}else if( (chKey >= 32) && (chKey <= 127) ){
					if ( pcmd->index < (CONSOLE_CMD_CONSOLE_BUFFER_SIZE - 1)){ // δ��
						pcmd->chCmd[pcmd->index++] = chKey;
						console_writebyte(chKey); //!< ����
					}
				}
				else if(chKey == 0x1B){ // Esc ����24λ���밴��
					s_chState = FRONT_END_CHECK24; 
				}
			}
			break;
		case FRONT_END_CHECK24:
			if(IS_FSM_CPL(front_end_check24( &wKey ))){
				front_end_function_key(wKey);
				s_chState = FRONT_END_INPUT; 
			}
			break;
			
		case FRONT_END_TAKEN:
			//������ʷ����
			console_hiscmd_save((console_cmd_t *)pcmd);
			token((console_cmd_t *)pcmd, pchCount);
			
			*pchTokens = (uint8_t *)&pcmd->chCmd[0];
			FRONT_END_RESET_FSM();
			return fsm_rt_cpl;
			//break;
		default:
			FRONT_END_RESET_FSM();
			break;
	}
	
	return fsm_rt_on_going;
}

//!< ok
bool console_str_cpy(uint8_t *toHere, const uint8_t *fromHere)
{
	if ((NULL == toHere) || (NULL == fromHere)) {
		return false;
	}
	
	while ('\0' != *fromHere) {
		*toHere = *fromHere;
		toHere++;
		fromHere++;
	}
	
	*toHere = *fromHere;
	
	return true;
}

// ok
bool console_cmp_str(uint8_t *pchString1, uint8_t *pchString2)
{
	if ( (NULL == pchString1) || (NULL == pchString2) ) {
		return false;
	}

	while (*pchString1 == *pchString2) {
		if ('\0' == *pchString1) {
			return true;
		}
		pchString1++;
		pchString2++;
	}
	
	return false;
}

// ok
uint8_t console_str_len(uint8_t *pchString)
{
	uint8_t chLen = 0;
	
	while ('\0' != *pchString) {
		pchString++;
		chLen++;
	}
	
	return chLen;
}

//!< is ok
static const console_command_t *search_cmd_map(uint8_t* pchTokens)
{
	uint8_t chCount = 0;
	
	while (chCount < UBOUND(s_tCMD)) {
		if (console_cmp_str(pchTokens, s_tCMD[chCount].pchCMD)) {
			return &s_tCMD[chCount];
		}
		chCount++;
	}
	
	chCount = 0;
	while (chCount < s_chDynaCount) {
		if (console_cmp_str(pchTokens, s_ptDynaCMD[chCount].pchCMD)) {
			return &s_ptDynaCMD[chCount];
		}
		chCount++;		
	}
	
	return NULL;
}

// ok
static bool IsMeetHelp(uint8_t *pchTokens)
{
	uint8_t *s_pchHelpString[] = {"H", "h", "?"};
	uint8_t i = 0;

	if ( NULL == pchTokens ){
		return false;
	}
	
	for ( i = 0; i < UBOUND(s_pchHelpString); i++ ) {
		if ( console_cmp_str(s_pchHelpString[i], pchTokens) ) {
			return true;
		}
	}
	return false;
}

static bool cmd_help_hander(uint8_t argc, uint8_t *argv[], void *pcmd)
{
	uint8_t i;

	if(argc != 1){
		console_writestring(CONSOLE_BADCOMMAND_PRINT);
		console_prn_rn();
		return false;
	}
	for(i = 0; i < UBOUND(s_tCMD); i++){
		console_writestring(s_tCMD[i].pchHelp);
		console_prn_rn();
	}

	for(i = 0; i < s_chDynaCount; i++){
		console_writestring(s_ptDynaCMD[i].pchHelp);
		console_prn_rn();
	}

	return true;
}


static bool cmd_clear_hander(uint8_t argc, uint8_t *argv[], void *pcmd)
{
	if(argc != 1){
		console_writestring(CONSOLE_BADCOMMAND_PRINT);
		console_prn_rn();
		return false;
	}
	
	console_writebyte(CONSOLE_CLEAR_CODE);

	return true;
}


static void parse(uint8_t *pchTokens, uint8_t chCount)
{
	const console_command_t *s_ptCMD;
	uint8_t i;
	
	s_ptCMD = search_cmd_map(pchTokens);
	if (NULL == s_ptCMD) { 
		console_writestring(CONSOLE_BADCOMMAND_PRINT);
		console_prn_rn();
	}else{//��Ϣ�ҵ�

		//befor use ,must clean all the point
		for(i = 0; i < CONSOLE_ARGV_MAX_SIZE; i++){
			s_argv[i] = NULL;
		}
		
		// ת��������argc *argv[] ��ʽ���Ա����
		chCount = (chCount < CONSOLE_ARGV_MAX_SIZE) ? chCount : CONSOLE_ARGV_MAX_SIZE;
		s_argv[0] =  pchTokens;
		for(i = 1; i < chCount; i++){
			while(*pchTokens != '\0'){
				pchTokens++;
			}
			pchTokens++;
			s_argv[i] = pchTokens;
		}
		
		if(IsMeetHelp(s_argv[1])){
			if(chCount != 2){
				console_writestring(CONSOLE_BADCOMMAND_PRINT);
			}else{
				console_writestring(s_ptCMD->pchHelp);
			}
			console_prn_rn();
		}else{
			(void)s_ptCMD->fncmd_handler(chCount ,s_argv, (void *)s_ptCMD);
		}	
	}
}

void console_init(void)
{
	console_extern_init();
}

//!< ok 
void console_task(void)
{
	uint8_t s_chCounter = 0;
	fsm_rt_t fsm;
	uint8_t *ptoken;
	
	fsm = front_end(&ptoken, &s_chCounter );
	if (IS_FSM_CPL(fsm)) {
		parse(ptoken, s_chCounter);
	}
;	
}


/*! \note console dynamic command register
 *  \param ptCMD register command list
 *  \retval true command register succeeded.
 *  \retval false command register  failed
 */
bool console_cmd_register(const console_command_t* ptCMD, uint8_t chCount)
{	
	if (NULL == ptCMD) {
		return false;
	}
	
	s_ptDynaCMD = ptCMD;
	s_chDynaCount = chCount;
	
	return true;
}

/* EOF */


