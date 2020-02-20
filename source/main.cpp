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

Thread dataTestThread;

void createTestDataMailBox(){
	while(1){
	static int counter = 0; 	
	//add random amount of packets 
	int x = rand()%15; // random number 0 to 15 
	for(int y = 0; y < x; y++){
		//allocate memory on the heap
		EcgUart_t::ecgDataPacket_t *ecgDataPacketMail = MB_ecgDataPacket.alloc();

		//add data
		ecgDataPacketMail->voltageY 		= x*rand()%35;
		ecgDataPacketMail->rTimeX 			= 100+counter; // just returns stystem time us
		ecgDataPacketMail->HeartRate 		= x*rand()%13;
		ecgDataPacketMail->HRV 					= x*rand()%10;
		ecgDataPacketMail->bodyTemp 		= x*rand()%6;
		ecgDataPacketMail->packetNumber = counter;
		counter++;
	
		//add packet to mail box q
		MB_ecgDataPacket.put(ecgDataPacketMail);
	}
	//wait 100ms before running thread again 
	ThisThread::sleep_for(100);
}
}
 int main()
 {
			EcgUart_t::EcgUartInit();//assings the thread 
			dataTestThread.start(createTestDataMailBox);
     while(true)
     {
			 
     }
 }