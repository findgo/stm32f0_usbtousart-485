
#include "app_cfg.h"

//for driver
#include "systick.h"
#include "usart.h"
#include "hal_led.h"

//for mod
#include "mod_leds.h"
#include "mod_clock.h"
#include "mod_timers.h"
#include "mod_console.h"

//for usb
#include "usbd_cdc_core.h"
#include  "usbd_usr.h"

// ----- main() ---------------------------------------------------------------
USB_CORE_HANDLE  USB_Device_dev ;


int main(int argc, char* argv[])
{
  // At this stage the system clock should have already been configured
  // at high speed.
	uint32_t elapseTime;

    SystemCoreClockUpdate();
	USBD_Init(&USB_Device_dev,
	        &USR_desc,
	        &USBD_CDC_cb,
	        &USR_cb);
	
	Systick_Configuration();
	Usart_Configuration();
	halledInit();
	ledseqInit();
	ledseqset( HAL_LED_2, LEDSEQ_EVE_STOP, LEDSEQ_MODE_ON);
	ledseqset( HAL_LED_1, LEDSEQ_EVE_FLASH_BLINK, LEDSEQ_MODE_NONE);
	//DBG_LOG("test is init!\r\n");
	// Infinite loop
  while (1)
    {
       // Add your code here.
	  console_task();
	  if(elapseTime = mcu_elapse_clocktime(), elapseTime > 0){
		  ledsequpdate (elapseTime);
	  }
    }
}


// ----------------------------------------------------------------------------
