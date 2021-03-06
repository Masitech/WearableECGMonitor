
#ifndef MaxAFE_H
#define MaxAFE_H 1
 
	#include 	"mbed.h"
	#include 	"defines.h"
	#include 	"Max30003.h"
	#include  "EcgUart.h"
/**
 *  @class 
 *  @brief 
 */
 

 extern SPI spiBus;     // SPIS_PSELMOSI = p10, SPIS_PSELMISO = p11,SPIS_PSELSCK  = p9,    
 //extern volatile bool ecgFIFOIntFlag;
 
 class MaxAFE_t : public MAX30003
	{
  private:
		//variable 
		
	  static const int EINT_STATUS_MASK 			=  1 << 23;
    static const int FIFO_OVF_MASK 					=  0x7;
    static const int FIFO_VALID_SAMPLE_MASK =  0x0;
    static const int FIFO_FAST_SAMPLE_MASK 	=  0x1;
    static const int ETAG_BITS_MASK 				= 0x7;
		static const int RTOR_STATUS 						=  1 << 10;
	  static const int RTOR_REG_OFFSET 				= 10;
    static constexpr float RTOR_LSB_RES 				= 0.0078125f;
		static uint32_t packetCounter;
		//
		
		static void ISR_AFE_Int(void);

		static void ConfigAFE(MAX30003& ecgAFE);
		
	
		static void EcgAFEThread(void);
		
	public:
	
	 //constructor 
	MaxAFE_t()
	//create the maxim30003 object 
	:MAX30003(spiBus, ECG_CS)
	{
		//call the init code ?
		//ecgFIFOIntFlag = 0; //no interrupt false 
		//init();//add thread  do setup
	};
	
	static void EcgAfeInit(void);
	

  
	};

	extern MaxAFE_t EcgAFE;
#endif
