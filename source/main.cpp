
#include "defines.h"
#include "MaxAFE.h"
#include "EcgUart.h"
#include "Battery.h"

#include <events/mbed_events.h>
#include <mbed.h>
#include "ble/BLE.h"
#include "ble/Gap.h"
#include "ble/services/BatteryService.h"
#include "ble/services/HeartRateService.h"
#include "MaxClock.h"
#include "BLE_Interface.h"
#include "mbed.h"


	const char ECG_UUID[] = "485f4145-52b9-4644-af1f-7a6b9322490f";
	const char ECG_SERVICE_UUID[] = "51311102-030e-485f-b122-f8f381aa84ed";
  BLE &bleX = BLE::Instance();
  ECG_Monitor AFEBLE(bleX, event_queue);


int main()
{
	//Other code is event driven and do not need a init.
	MaxAFE_Clock_Init();			//AFE CLOCK
	MaxAFE_t::EcgAfeInit();		//AFE SPI					

	bleX.onEventsToProcess(schedule_ble_events); 
	AFEBLE.start();						//BLE
	
		while(1){__WFE ();  // SLEEP when nothing to do
		};
}


