#include "defines.h"
#include "mbed.h"
#include "EcgUart.h"

/** Global Object */
EcgUart_t EcgUart;
/** Thread Object */
Thread EcgUart_Thread; // takes care of sending data ever x amount of time. 
/** ECG Packet Mail Box **/
Mail<EcgUart_t::ecgDataPacket_t, 256> MB_ecgDataPacket;


EcgUart_t::EcgUart_t(PinName ecg_tx, PinName ecg_rx, uint32_t baudRate)
	:BufferedSerial(ecg_tx, ecg_rx, baudRate) // strictly speciffy the base contructor from the drivrived contrusctor 
{
	// contructor body 
}

void EcgUart_t::EcgUartInit(){
	//start a thread 
	EcgUart_Thread.start(EcgUart_t::EcgUartPacketHandler);
}

template <class T>
void EcgUart_t::sendTaggedHex(const char *tag, T data){
		EcgUart.printf("{%s %x}",tag,data);
}

void EcgUart_t::EcgUartPacketHandler(){
	//run once code here 	
	while(true){
	//	EcgUart.printf("\n EcgUartPacketHandler()  mail box %d",!MB_ecgDataPacket.empty());
	// check if there is anything in the mail box. 
	while(!MB_ecgDataPacket.empty()){
	//if something send until it is empty 
		osEvent evt = MB_ecgDataPacket.get();
		ecgDataPacket_t *ecgDP_t = (ecgDataPacket_t*)evt.value.p;
		sendTaggedHex("AAA",ecgDP_t->voltageY);
		sendTaggedHex("AAB",ecgDP_t->rTimeX);
		sendTaggedHex("AAC",ecgDP_t->HeartRate);
		sendTaggedHex("AAD",ecgDP_t->HRV);
		sendTaggedHex("AAE",ecgDP_t->bodyTemp);
		sendTaggedHex("AAF",ecgDP_t->packetNumber);
		
		//free memory 
		MB_ecgDataPacket.free(ecgDP_t);	
		ecgDP_t = {0}; // reset data to zeros 
	}
	
	// else back to sleep 
	ThisThread::sleep_for(ECGUART_UPDATE_RATE_MS);
	}
}