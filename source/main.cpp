
//#include <stdio.h>
//#include "mbed.h"
#include "defines.h"
#include "MaxAFE.h"
#include "EcgUart.h"


//Ticker flipper;

//DigitalOut fclockc(ECG_FCLOCK);

//volatile bool status = false;

//void flip() {
//    if(status){
//         NRF_GPIO->OUTSET |= 1<<12;
//         status = false;
//        }
//        else{
//        NRF_GPIO->OUTCLR |= 1<<12;
//         status = true;
//            }

//}
//int main() {
//flipper.attach_us(&flip, 30);
//	MaxAFE_t::EcgAfeInit();
//	EcgUart_t::EcgUartInit();
//	while(1){}
//}

#include <events/mbed_events.h>
#include <mbed.h>
#include "ble/BLE.h"
#include "ble/Gap.h"
#include "ble/services/BatteryService.h"
#include "pretty_printer.h"
#include "nrf_timer.h"
static DigitalOut led1(p27, 1);

const static char DEVICE_NAME[] = "BATTERY";

static events::EventQueue event_queue(/* event count */ 16 * EVENTS_EVENT_SIZE);

class BatteryDemo : ble::Gap::EventHandler {
public:
    BatteryDemo(BLE &ble, events::EventQueue &event_queue) :
        _ble(ble),
        _event_queue(event_queue),
        _battery_uuid(GattService::UUID_BATTERY_SERVICE),
        _battery_level(50),
        _battery_service(ble, _battery_level),
        _adv_data_builder(_adv_buffer) { }

    void start() {
        _ble.gap().setEventHandler(this);

        _ble.init(this, &BatteryDemo::on_init_complete);

        _event_queue.call_every(500, this, &BatteryDemo::blink);
        _event_queue.call_every(1000, this, &BatteryDemo::update_sensor_value);

        _event_queue.dispatch_forever();
    }

private:
    /** Callback triggered when the ble initialization process has finished */
    void on_init_complete(BLE::InitializationCompleteCallbackContext *params) {
        if (params->error != BLE_ERROR_NONE) {
            print_error(params->error, "Ble initialization failed.");
            return;
        }

        print_mac_address();

        start_advertising();
    }

    void start_advertising() {
        /* Create advertising parameters and payload */

        ble::AdvertisingParameters adv_parameters(
            ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
            ble::adv_interval_t(ble::millisecond_t(1000))
        );

        _adv_data_builder.setFlags();
        _adv_data_builder.setLocalServiceList(mbed::make_Span(&_battery_uuid, 1));
        _adv_data_builder.setName(DEVICE_NAME);

        /* Setup advertising */

        ble_error_t error = _ble.gap().setAdvertisingParameters(
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

    void update_sensor_value() {
        if (_ble.gap().getState().connected) {
            _battery_level++;
            if (_battery_level > 100) {
                _battery_level = 20;
            }

            _battery_service.updateBatteryLevel(_battery_level);
        }
    }

    void blink(void) {
        led1 = !led1;
    }

private:
    /* Event handler */

    void onDisconnectionComplete(const ble::DisconnectionCompleteEvent&) {
        _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);
    }

private:
    BLE &_ble;
    events::EventQueue &_event_queue;

    UUID _battery_uuid;

    uint8_t _battery_level;
    BatteryService _battery_service;

    uint8_t _adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE];
    ble::AdvertisingDataBuilder _adv_data_builder;
};

/** Schedule processing of events from the BLE middleware in the event queue. */
void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context) {
    event_queue.call(Callback<void()>(&context->ble, &BLE::processEvents));
}


Ticker flipper;

DigitalOut fclockc(ECG_FCLOCK);

volatile bool status = false;

void flip() {
    if(status){
         NRF_GPIO->OUTSET |= 1<<12;
         status = false;
        }
        else{
        NRF_GPIO->OUTCLR |= 1<<12;
         status = true;
            }
					}
/** TIMTER2 peripheral interrupt handler. This interrupt handler is called whenever there it a TIMER2 interrupt
 */
extern "C" {
void TIMER3_IRQHandler(void)
{
    if (NRF_TIMER3->EVENTS_COMPARE[0] == 1)
    {   
       //body of interrupt 
			nrf_gpio_pin_toggle(27);
        NRF_TIMER3->EVENTS_COMPARE[0] = 0;
    }
}
}
void start_timer(void)
{		
   // 32-bit timer
    NRF_TIMER3->BITMODE = TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos;
 
   // 1us timer period
    NRF_TIMER3->PRESCALER = 9 << TIMER_PRESCALER_PRESCALER_Pos; //31250
 
  // 1000 us compare value, generates EVENTS_COMPARE[0]
    NRF_TIMER3->CC[0] = 25000;
 
    // Enable IRQ on EVENTS_COMPARE[0]
    NRF_TIMER3->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;
 
    // Clear the timer when COMPARE0 event is triggered
    NRF_TIMER3->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;
		NVIC_SetVector(TIMER3_IRQn, (uint32_t)TIMER3_IRQHandler);
    NVIC_EnableIRQ(TIMER3_IRQn);
    NRF_TIMER3->TASKS_START = 1;                                                            // Start TIMER
	
	
//	NRF_TIMER2->CC[0] = 1;
//	NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;
//	NRF_TIMER2->BITMODE = 0; //8-bit
//	NRF_TIMER2->PRESCALER = 9; //16x prescaler
////	NRF_TIMER2->PRESCALER = 7;

//	NRF_TIMER2->INTENSET = (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos); 
////	NRF_TIMER2->SHORTS = 0;
//	NRF_TIMER2->SHORTS = 1; //COMPARE[0] -> CLEAR
//	NVIC_SetVector(TIMER2_IRQn, (uint32_t)TIMER2_IRQHandler);
//	NVIC_EnableIRQ(TIMER2_IRQn);
//	NRF_TIMER2->TASKS_START = 1;

////NRF_TIMER4->CC[0] = 100000;  //100ms
//	NRF_TIMER4->MODE = TIMER_MODE_MODE_Timer;
//	NRF_TIMER4->BITMODE = 1; //8-bit
//	NRF_TIMER4->PRESCALER = 9; //16x prescaler
////	NRF_TIMER2->PRESCALER = 7;

//	NRF_TIMER4->INTENSET = (TIMER_INTENSET_COMPARE1_Enabled); 
////	NRF_TIMER2->SHORTS = 0;
//	NRF_TIMER4->SHORTS = 1; //COMPARE[0] -> CLEAR
//	NVIC_SetVector(TIMER3_IRQn, (uint32_t)TIMER3_IRQHandler);
//	NVIC_EnableIRQ(TIMER2_IRQn);
//	NRF_TIMER4->TASKS_START = 1;
}
		


//Ble Working But cannot generate FCLOCK
int main()
{
	nrf_gpio_pin_dir_set(27,NRF_GPIO_PIN_DIR_OUTPUT);
	nrf_gpio_pin_toggle(27);
	wait_ms(200);
	nrf_gpio_pin_toggle(27);
	start_timer();
	//multiple thread class cannot be started at the same time as ble 
		//	flipper.attach_us(&flip, 30);   //does not work with ble maybe same timer is used
		MaxAFE_t::EcgAfeInit();						//does not work with ble thread related??
//    BLE &ble = BLE::Instance();
//    ble.onEventsToProcess(schedule_ble_events);

//    BatteryDemo demo(ble, event_queue);
//    demo.start();

		while(1){};
}
