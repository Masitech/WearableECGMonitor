#include <events/mbed_events.h>
#include <mbed.h>
#include "ble/BLE.h"
#include "ble/gap/Gap.h"
#include "ble/services/HeartRateService.h"
#include "pretty_printer.h"
#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "cmsis_os.h"
#include "rtos.h"
#include "BufferedSerial.h"
#include "EcgUart.h"

//Thread dataTestThread;

//void createTestDataMailBox(){
//	while(1){
//	static int counter = 0; 	
//	//add random amount of packets 
//	int x = rand()%15; // random number 0 to 15 
//	for(int y = 0; y < x; y++){
//		//allocate memory on the heap
//		EcgUart_t::ecgDataPacket_t *ecgDataPacketMail = MB_ecgDataPacket.alloc();

//		//add data
//		ecgDataPacketMail->voltageY 		= x*rand()%35;
//		ecgDataPacketMail->rTimeX 			= 100+counter; // just returns stystem time us
//		ecgDataPacketMail->HeartRate 		= x*rand()%13;
//		ecgDataPacketMail->HRV 					= x*rand()%10;
//		ecgDataPacketMail->bodyTemp 		= x*rand()%6;
//		ecgDataPacketMail->packetNumber = counter;
//		counter++;
//	
//		//add packet to mail box q
//		MB_ecgDataPacket.put(ecgDataPacketMail);
//	}
//	//wait 100ms before running thread again 
//	ThisThread::sleep_for(100);
//}
//}
// int main()
// {
//			EcgUart_t::EcgUartInit();//assings the thread 
//			dataTestThread.start(createTestDataMailBox);
//     while(true)
//     {
//			 
//     }
// }

#include "mbed.h"
//#include "max32630fthr.h"
#include "Max30003.h"
 
//MAX32630FTHR pegasus(MAX32630FTHR::VIO_3V3);
 
void ecg_config(MAX30003 &ecgAFE);
 
/* ECG FIFO nearly full callback */
volatile bool ecgIntFlag = 0; 
void ecgFIFO_callback()  {
    
    ecgIntFlag = 1;
 
}  
 
int main()
{   
    
    // Constants
    const int EINT_STATUS =  1 << 23;
    const int RTOR_STATUS =  1 << 10;
    const int RTOR_REG_OFFSET = 10;
    const float RTOR_LSB_RES = 0.0078125f;
    const int FIFO_OVF =  0x7;
    const int FIFO_VALID_SAMPLE =  0x0;
    const int FIFO_FAST_SAMPLE =  0x1;
    const int ETAG_BITS = 0x7;
    
    // Ports and serial connections
    //Serial pc(USBTX, USBRX);            // Use USB debug probe for serial link
    //EcgUart_t.baud(115200);                    // Baud rate = 115200
    
    DigitalOut rLed(p7, 0);     // Debug LEDs
 
    
    InterruptIn ecgFIFO_int(p12);          // Config P5_4 as int. in for the
    ecgFIFO_int.fall(&ecgFIFO_callback);    // ecg FIFO almost full interrupt
    
    SPI spiBus(p22, p23, p24);     // SPI2_MOSI, SPI2_MISO, SPI2_SCK
                                                    // P5_2 = MISO, P5_0 = SCK
    
    MAX30003 ecgAFE(spiBus, p8);          // New MAX30003 on spiBus, CS = P5_3 
    ecg_config( ecgAFE );                   // Config ECG 
     
    
    ecgAFE.writeRegister( MAX30003::SYNCH , 0);
    
    uint32_t ecgFIFO, RtoR, readECGSamples, idx, ETAG[32], status;
    int16_t ecgSample[32];
    float BPM;
    
    while(1) {
        
        /* Read back ECG samples from the FIFO */
        if( ecgIntFlag ) {
            
            ecgIntFlag = 0;
            EcgUart.printf("Interrupt received....\r\n");
            status = ecgAFE.readRegister( MAX30003::STATUS );      // Read the STATUS register
            EcgUart.printf("Status : 0x%x\r\n"
                      "Current BPM is %3.2f\r\n\r\n", status, BPM);
             
             
            // Check if R-to-R interrupt asserted
            if( ( status & RTOR_STATUS ) == RTOR_STATUS ){           
            
                EcgUart.printf("R-to-R Interrupt \r\n");
                
                // Read RtoR register
                RtoR = ecgAFE.readRegister( MAX30003::RTOR ) >>  RTOR_REG_OFFSET;   
                
                // Convert to BPM
                BPM = 1.0f / ( RtoR * RTOR_LSB_RES / 60.0f );   
                
                // Print RtoR              
                EcgUart.printf("RtoR : %d\r\n\r\n", RtoR);                   
                
            }  
             
            // Check if EINT interrupt asserted
            if ( ( status & EINT_STATUS ) == EINT_STATUS ) {     
            
                EcgUart.printf("FIFO Interrupt \r\n");
                readECGSamples = 0;                        // Reset sample counter
                
                do {
                    ecgFIFO = ecgAFE.readRegister( MAX30003::ECG_FIFO );       // Read FIFO
                    ecgSample[readECGSamples] = ecgFIFO >> 8;                  // Isolate voltage data
                    ETAG[readECGSamples] = ( ecgFIFO >> 3 ) & ETAG_BITS;  // Isolate ETAG
                    readECGSamples++;                                          // Increment sample counter
                
                // Check that sample is not last sample in FIFO                                              
                } while ( ETAG[readECGSamples-1] == FIFO_VALID_SAMPLE || 
                          ETAG[readECGSamples-1] == FIFO_FAST_SAMPLE ); 
            
                EcgUart.printf("%d samples read from FIFO \r\n", readECGSamples);
                
                // Check if FIFO has overflowed
                if( ETAG[readECGSamples - 1] == FIFO_OVF ){                  
                    ecgAFE.writeRegister( MAX30003::FIFO_RST , 0); // Reset FIFO
                    rLed=1;
                }
                
                // Print results 
                for( idx = 0; idx < readECGSamples; idx++ ) {
                    EcgUart.printf("Sample : %6d, \tETAG : 0x%x\r\n", ecgSample[idx], ETAG[idx]);           
                }
                EcgUart.printf("\r\n\r\n\r\n"); 
                                         
            }
        }
    }
}
 
 
 
 
void ecg_config(MAX30003& ecgAFE) { 
 
    // Reset ECG to clear registers
    ecgAFE.writeRegister( MAX30003::SW_RST , 0);
    
    // General config register setting
    MAX30003::GeneralConfiguration_u CNFG_GEN_r;
    CNFG_GEN_r.bits.en_ecg = 1;     // Enable ECG channel
    CNFG_GEN_r.bits.rbiasn = 1;     // Enable resistive bias on negative input
    CNFG_GEN_r.bits.rbiasp = 1;     // Enable resistive bias on positive input
    CNFG_GEN_r.bits.en_rbias = 1;   // Enable resistive bias
    CNFG_GEN_r.bits.imag = 2;       // Current magnitude = 10nA
    CNFG_GEN_r.bits.en_dcloff = 1;  // Enable DC lead-off detection   
    ecgAFE.writeRegister( MAX30003::CNFG_GEN , CNFG_GEN_r.all);
        
    
    // ECG Config register setting
    MAX30003::ECGConfiguration_u CNFG_ECG_r;
    CNFG_ECG_r.bits.dlpf = 1;       // Digital LPF cutoff = 40Hz
    CNFG_ECG_r.bits.dhpf = 1;       // Digital HPF cutoff = 0.5Hz
    CNFG_ECG_r.bits.gain = 3;       // ECG gain = 160V/V
    CNFG_ECG_r.bits.rate = 2;       // Sample rate = 128 sps
    ecgAFE.writeRegister( MAX30003::CNFG_ECG , CNFG_ECG_r.all);
      
    
    //R-to-R configuration
    MAX30003::RtoR1Configuration_u CNFG_RTOR_r;
    CNFG_RTOR_r.bits.wndw = 0b0011;         // WNDW = 96ms
    CNFG_RTOR_r.bits.rgain = 0b1111;        // Auto-scale gain
    CNFG_RTOR_r.bits.pavg = 0b11;           // 16-average
    CNFG_RTOR_r.bits.ptsf = 0b0011;         // PTSF = 4/16
    CNFG_RTOR_r.bits.en_rtor = 1;           // Enable R-to-R detection
    ecgAFE.writeRegister( MAX30003::CNFG_RTOR1 , CNFG_RTOR_r.all);
       
        
    //Manage interrupts register setting
    MAX30003::ManageInterrupts_u MNG_INT_r;
    MNG_INT_r.bits.efit = 0b00011;          // Assert EINT w/ 4 unread samples
    MNG_INT_r.bits.clr_rrint = 0b01;        // Clear R-to-R on RTOR reg. read back
    ecgAFE.writeRegister( MAX30003::MNGR_INT , MNG_INT_r.all);
    
    
    //Enable interrupts register setting
    MAX30003::EnableInterrupts_u EN_INT_r;
    EN_INT_r.bits.en_eint = 1;              // Enable EINT interrupt
    EN_INT_r.bits.en_rrint = 1;             // Enable R-to-R interrupt
    EN_INT_r.bits.intb_type = 3;            // Open-drain NMOS with internal pullup
    ecgAFE.writeRegister( MAX30003::EN_INT , EN_INT_r.all);
       
       
    //Dyanmic modes config
    MAX30003::ManageDynamicModes_u MNG_DYN_r;
    MNG_DYN_r.bits.fast = 0;                // Fast recovery mode disabled
    ecgAFE.writeRegister( MAX30003::MNGR_DYN , MNG_DYN_r.all);
    
    // MUX Config
    MAX30003::MuxConfiguration_u CNFG_MUX_r;
    CNFG_MUX_r.bits.openn = 0;          // Connect ECGN to AFE channel
    CNFG_MUX_r.bits.openp = 0;          // Connect ECGP to AFE channel
    ecgAFE.writeRegister( MAX30003::CNFG_EMUX , CNFG_MUX_r.all);
    
    return;
} 