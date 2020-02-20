
#ifndef ECGUART_H
#define ECGUART_H 1
 
#include "BufferedSerial.h"
#include "defines.h"


/**
 *  @class BufferedSerial
 *  @brief Software buffers and interrupt driven tx and rx for Serial
 */  
class EcgUart_t : public BufferedSerial 
{
public:
	
	struct ecgDataPacket_t{
		uint32_t voltageY = 0; //voltage data (processed) 
		uint32_t rTimeX   = 0; //realtive time 
		uint32_t HeartRate = 0; // contionus heart rate averaged ?
		uint32_t HRV			= 0;  //Heart rate variability index 
		uint32_t bodyTemp = 0; 
		uint32_t packetNumber = 0;
	};


	/** Create a BufferedSerial port, connected to the specified transmit and receive pins
     *  @param tx Transmit pin
     *  @param rx Receive pin
     *  @param buf_size printf() buffer size
     *  @param tx_multiple amount of max printf() present in the internal ring buffer at one time
     *  @param name optional name
     *  @note Either tx or rx may be specified as NC if unused
     */
    EcgUart_t(PinName ecg_tx = RX_PIN, PinName ecg_rx = TX_PIN, uint32_t baudRate = BAUD_RATE);
    
	
		void unpackSend(ecgDataPacket_t *fifo);
	
		static void sendTestData();
	
		static void EcgUartPacketHandler();
	
		static void EcgUartInit();
		
		template <class T>
		static void sendTaggedHex(const char *tag, T data);
			
	private:

    };

		extern Mail<EcgUart_t::ecgDataPacket_t, 256> MB_ecgDataPacket;
		extern EcgUart_t EcgUart;
		extern Thread EcgUart_Thread;
#endif
