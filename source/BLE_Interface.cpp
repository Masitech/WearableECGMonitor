#include "main.h"
#include "BLE_Interface.h"
#include "defines.h"

const char DEVICE_NAME[] = "ECG Monitor";

events::EventQueue event_queue(/* event count */ 16 * EVENTS_EVENT_SIZE);

void ECG_Monitor::start(){

        _ble.gap().setEventHandler(this);
        _ble.init(this, &ECG_Monitor::on_init_complete);
        _event_queue.call_every(1000, this, &ECG_Monitor::update_sensor_value);
        _event_queue.dispatch_forever();
}

void ECG_Monitor::on_init_complete(BLE::InitializationCompleteCallbackContext *params){
        if (params->error != BLE_ERROR_NONE) {
            print_error(params->error, "Ble initialization failed.");
            return;
        }
        print_mac_address();
        start_advertising();
}
void ECG_Monitor::updateECG_S(int16_t value){
				_ECG_service.updateEcgData(value);
}
void ECG_Monitor::updateHR(uint16_t value){
				_hr_service.updateHeartRate(value);
}
void ECG_Monitor::update_sensor_value(){
        if (_ble.gap().getState().connected && !BREADBOARD) {
						uint32_t value = batteryObj->getBatteryLevel();
            _battery_service.updateBatteryLevel(value);
        }
				else if(_ble.gap().getState().connected && BREADBOARD){
					_battery_service.updateBatteryLevel(93);
				}
}

void ECG_Monitor::onDisconnectionComplete(const ble::DisconnectionCompleteEvent&){
	 _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);
}
void ECG_Monitor::start_advertising(){
        /* Create advertising parameters and payload */

        ble::AdvertisingParameters adv_parameters(
            ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
            ble::adv_interval_t(ble::millisecond_t(1000))
        );
				ble_error_t error;
        error = _adv_data_builder.setFlags();
        if (error) {
            print_error(error, "_adv_data_builder.setFlags() failed");
            return;
        }
				error =_adv_data_builder.setName(DEVICE_NAME);
        if (error) {
            print_error(error, "_adv_data_builder.setName(DEVICE_NAME) failed");
            return;
        }
			  //error = _adv_data_builder.setAppearance(ble::adv_data_appearance_t::GENERIC_HEART_RATE_SENSOR);
        error = _adv_data_builder.setLocalServiceList(mbed::make_Span(&_battery_uuid, 1));
        if (error) {
            print_error(error, "_adv_data_builder.setLocalServiceList(mbed::make_Span(&_battery_uuid, 1) failed");
            return;
        }
				error = _adv_data_builder.setLocalServiceList(mbed::make_Span(&_hr_uuid, 1));
        if (error) {
            print_error(error, "_adv_data_builder.setLocalServiceList(mbed::make_Span(&_hr_uuid, 1) failed");
            return;
        }
				//error = _adv_data_builder.setLocalServiceList(mbed::make_Span(&_ECG_uuid, 1));
        if (error) {
            print_error(error, "_adv_data_builder.setLocalServiceList(mbed::make_Span(&_ECG_uuid, 1) failed");
            return;
        }
        /* Setup advertising */

        error = _ble.gap().setAdvertisingParameters(
            ble::LEGACY_ADVERTISING_HANDLE,
            adv_parameters
        );

        if (error) {
            print_error(error, "_ble.gap().setAdvertisingParameters() failed");
            return;
        }

        error = _ble.gap().setAdvertisingPayload(
            ble::LEGACY_ADVERTISING_HANDLE,
            _adv_data_builder.getAdvertisingData()
        );

        if (error) {
            print_error(error, "_ble.gap().setAdvertisingPayload() failed");
            return;
        }

        /* Start advertising */

        error = _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);

        if (error) {
            print_error(error, "_ble.gap().startAdvertising() failed");
            return;
        }
}
/** Schedule processing of events from the BLE middleware in the event queue. */
void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context) {
    event_queue.call(Callback<void()>(&context->ble, &BLE::processEvents));
}