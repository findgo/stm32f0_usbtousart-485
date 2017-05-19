


#include "util_fifo.h"

/*	���г�ʼ��		*/  		
bool fifo_init(fifo_buf_t *ptQ, uint8_t *ptBuffer, uint16_t tSize)
{                                                        
    if (NULL == ptQ || NULL == ptBuffer || 0 == tSize) { 
        return false;                                    
    }                                                    
                                                         
    ptQ->ptBuffer 	= ptBuffer;                          
    ptQ->tSize 		= tSize;                             
    ptQ->tHead 		= 0;                                 
    ptQ->tTail 		= 0;                                 
    ptQ->tCounter 	= 0;                                 
                                                         
    return true;                                         
}

// ��ջ���
bool fifo_flush(fifo_buf_t *ptQ)
{
	uint16_t head;      /* used to avoid volatile decision */

	if (NULL == ptQ ){
		return false;				
	}
	
	head = ptQ->tHead;
	ptQ->tTail = head;
	ptQ->tCounter = 0;
	
	return true;
}

 /*	���		*/                   					     
bool fifo_put(fifo_buf_t *ptQ, uint8_t tObj)
{													
	bool bResult = false;
	
	if (NULL == ptQ ){
		return false;				
	}												
																					
	if (ptQ->tCounter != ptQ->tSize) {		
		ptQ->ptBuffer[ptQ->tTail] = tObj;
		ptQ->tCounter++;
		if (++ptQ->tTail >= ptQ->tSize) { 		
			ptQ->tTail = 0; 					
		}										
						
		bResult = true; 						
	}
	
	return bResult; 								
}	
  /*	��ͷ���	 ʵ��ջ	*/  
bool fifo_put_front(fifo_buf_t *ptQ, uint8_t tObj)
{
	bool bResult = false;
	
	if (NULL == ptQ ){
		return false;				
	}												
																					
	if (ptQ->tCounter != ptQ->tSize) {
		if(ptQ->tHead == 0){
			ptQ->tHead = ptQ->tSize - 1;
		}else{
			ptQ->tHead--;
		}
		ptQ->ptBuffer[ptQ->tHead] = tObj;
		ptQ->tCounter++;
			
		bResult = true; 						
	}
	
	return bResult; 	
}
/*	���	 ����Ϊlength������buf	*/  
uint16_t fifo_push(fifo_buf_t *ptQ, uint8_t *buf, uint16_t length)
{
	uint16_t lens;
	uint16_t availcount;

	if (NULL == ptQ || buf == NULL ){
		return 0;				
	}	
	//�鿴�ж��ٿ�λ
	availcount = ptQ->tSize - ptQ->tCounter;
	if(availcount >= length){
		lens = length;
	}else{
		lens = availcount;
	}
	
	availcount = lens;
	while(availcount){
		ptQ->ptBuffer[ptQ->tTail] = *buf;
		ptQ->tCounter++;
		if (++ptQ->tTail >= ptQ->tSize) { 		
			ptQ->tTail = 0; 					
		}
		buf++;
		availcount--;
	}
	
	return lens;
}
  /*	����		*/                                       
bool fifo_pop(fifo_buf_t *ptQ, uint8_t *ptObj)
{                                                          
	bool bResult = false;
	
	if(ptQ == NULL && NULL == ptObj){						
		return false;										
	  }
	
	if (ptQ->tCounter != 0) {						
		*ptObj = ptQ->ptBuffer[ptQ->tHead];
		ptQ->tCounter--;	
		if(++ptQ->tHead >= ptQ->tSize){				
			ptQ->tHead = 0; 						
		}										
							
		bResult = true; 								
	}												
		                                                       
    return bResult;                                        
}
  /*	����	 ����Ϊlength�����ݵ�buf	*/ 
uint16_t fifo_pull(fifo_buf_t *ptQ, uint8_t *buf, uint16_t length)
{
	uint16_t lens;
	uint16_t validcount;
	
	if (NULL == ptQ || buf == NULL ){
		return 0;				
	}	
	//�鿴�ж�����Ч��ֵ
	validcount = ptQ->tCounter;
	if(validcount >= length){
		lens = length;
	}else{
		lens = validcount;
	}
	validcount = lens;
	while(validcount)
	{
		*buf = ptQ->ptBuffer[ptQ->tHead];
		ptQ->tCounter--;	
		if(++ptQ->tHead >= ptQ->tSize){				
			ptQ->tHead = 0; 						
		}
		buf++;
		validcount--;
	}
	
	return lens;
}
/* �ҾͿ���,���ǲ�����*/
bool fifo_peek(fifo_buf_t *ptQ, uint8_t *ptObj)
{														
	bool bResult = false;
	
	if (NULL == ptQ && NULL == ptObj) {	
		return false;
	}
																
	if (ptQ->tCounter != 0) {			
		*ptObj = ptQ->ptBuffer[ptQ->tHead]; 	
		bResult = true; 				
	}
	
	return bResult; 
}
