#include <events/mbed_events.h>
#include "Battery.h"
#include "mbed.h"
#include "nrfx_saadc.h"
AnalogIn Lipo(BATT_PIN); //adc pin to battery with voltage divider 
//static events::EventQueue Batt_Event(/* event count */ 4 * EVENTS_EVENT_SIZE);

uint32_t Battery_t::rawBattCountA = 0;//
uint32_t Battery_t::rawBattCountB = 0;//
uint32_t Battery_t::batteryLevel = 0;//
nrf_saadc_value_t reading;
Battery_t* batteryObj = Battery_t::getInstance();

// Define the static Battery_t pointer
Battery_t* Battery_t::inst_ = NULL;

Battery_t* Battery_t::getInstance() {
   if (inst_ == NULL) {
      inst_ = new Battery_t();
   }
   return(inst_);
}

uint32_t Battery_t::getBatteryLevel(){
	takeRawReading();  //update reading 
	batteryLevel = convertToPercentage();
	return batteryLevel;
}
uint32_t Battery_t::takeRawReading(void){
		
		nrfx_saadc_sample_convert(BATT_PIN,&reading);
		rawBattCountA = reading;

	if (rawBattCountA > (rawBattCountB+changeDifference)){ //battery voltage has increase a lot 
		rawBattCountB = rawBattCountA;
	}
	if(rawBattCountA < rawBattCountB){ //battery voltage is decresing 
		rawBattCountB = rawBattCountA;
	}
	if(rawBattCountA < minBatRawCount){ //battery voltage lower than min voltage 
		rawBattCountB = minBatRawCount;
		//send warning low battery 
	}
	if(rawBattCountA > maxBatRawCount){ //battery voltage above max voltage 
		rawBattCountB = maxBatRawCount;
	}
	
#if BATT_DEBUG
		EcgUart.printf("Battery Level A %d\n",rawBattCountA);
		EcgUart.printf("Battery Level B %d\n",rawBattCountB);
#endif
	return rawBattCountB;
	} 

	uint32_t Battery_t::convertToPercentage(){
	  uint16_t diff = maxBatRawCount - minBatRawCount;
    uint16_t current = rawBattCountB - minBatRawCount;
    float value = (float)current / (float)diff ;
    uint32_t percentage = value*100;
#if BATT_DEBUG 
		EcgUart.printf("Battery Percentage %d\n",percentage);
#endif
		return percentage;
	}
