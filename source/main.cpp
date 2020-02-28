#include <events/mbed_events.h>
#include <mbed.h>
#include "ble/BLE.h"
#include "ble/gap/Gap.h"
#include "ble/services/HeartRateService.h"
#include "pretty_printer.h"
#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "cmsis_os.h"
#include "rtos.h"
#include "BufferedSerial.h"
#include "EcgUart.h"
#include "MaxAFE.h"


int main()
{  
EcgUart_t::EcgUartInit();//assings the thread
MaxAFE_t::EcgAfeInit();
		EcgUart.printf("Main Thread Started \n");
		while(1) {
       
      
    }
}
 
