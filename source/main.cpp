
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

  BLE &bleX = BLE::Instance();
  ECG_Monitor AFEBLE(bleX, event_queue);
 

int main()
{
	MaxAFE_Clock_Init();
	MaxAFE_t::EcgAfeInit();						//does not work with ble thread related??

	bleX.onEventsToProcess(schedule_ble_events);
	AFEBLE.start();
		while(1){};
}
