#ifndef BATTERY
#define BATTERY 1

// experimentation with a Battery_t class - one instance of a class 
#include "defines.h"
#include "mbed.h"
#include "EcgUart.h"


/*
2.90V = 9300
4.20V = 10220
5.00V = 13000  
PC Long lead USB = 10500  (4.25v at BATT pin ) charging  
Short Charger = 10500
*/

/*
2.90V = 580
4.20V = 640
5.00V = 770  
*/

class Battery_t {
public:
   // This is how clients can access the single instance
   static Battery_t* getInstance();

//   void setValue(int val) {batteryLevel = val;}
//   uint32_t  getValue()           {return(batteryLevel);}

static uint32_t getBatteryLevel(void); //take readings every 500ms 
	 
protected:
//super class accessable but no super class 	 
private:
	static 	uint32_t 				batteryLevel; 	//in percentage 
	static 	uint32_t 				rawBattCountB;	//updated value taking in count of random change 
	static 	uint32_t				rawBattCountA;	//stright up raw data 

	static 	bool 						chargeStatus; 	//1 = charging, 0 = not charging 
	static 	const uint32_t 	minBatRawCount = BATT_MIN;
	static 	const uint32_t 	maxBatRawCount = BATT_MAX;
	static 	const uint8_t 	changeDifference = BATT_CEL;

  static Battery_t* inst_;   // The one, single instance
	static uint32_t convertToPercentage(void);
	static uint32_t takeRawReading(void); //take readings every 500ms 
  Battery_t() // private constructor
	{} 
	
  Battery_t(const Battery_t&);
  Battery_t& operator=(const Battery_t&);
	 
	

};

extern Battery_t* batteryObj;
#endif