#ifndef BLE_INTERFACEX
#define BLE_INTERFACEX 1

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
#include "ECGService.h"
#include "pretty_printer.h"

	extern const char DEVICE_NAME[];
 extern events::EventQueue event_queue;

class ECG_Monitor : ble::Gap::EventHandler {
public:
    ECG_Monitor(BLE &ble, events::EventQueue &event_queue) :
        _ble(ble),
        _event_queue(event_queue),
				//Battery 
        _battery_uuid(GattService::UUID_BATTERY_SERVICE),
        _battery_level(0),
        _battery_service(ble, _battery_level),
				//HRM(HRM UUID)
        _hr_uuid(GattService::UUID_HEART_RATE_SERVICE),
        _hr_counter(0),
        _hr_service(ble, _hr_counter, HeartRateService::LOCATION_FINGER),

				//custom ECG Basin on HRM
        _ECG_uuid(ECG_SERVICE_UUID),
        _ECG_counter(0),
        _ECG_service(ble, _ECG_counter, EcgService::LOCATION_FINGER),

        _adv_data_builder(_adv_buffer) { }

    void start(void);
		void updateECG_S(int16_t value);
private:
    /** Callback triggered when the ble initialization process has finished */
    void on_init_complete(BLE::InitializationCompleteCallbackContext *params);

    void start_advertising(void);

    void update_sensor_value(void);

private:
    /* Event handler */

    void onDisconnectionComplete(const ble::DisconnectionCompleteEvent&);

private:
    BLE &_ble;
    events::EventQueue &_event_queue;
		//battery
    UUID _battery_uuid;
    uint8_t _battery_level;
    BatteryService _battery_service;
		//HRM
    UUID _hr_uuid;
    uint8_t _hr_counter;
    HeartRateService _hr_service;
	  //ECH Data
    UUID _ECG_uuid;
    int16_t _ECG_counter;
    EcgService _ECG_service;

    uint8_t _adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE];
    ble::AdvertisingDataBuilder _adv_data_builder;
};
extern void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context);
#endif