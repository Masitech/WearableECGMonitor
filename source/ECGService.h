#ifndef ECGService_H
#define ECGService_H

#include "ble/BLE.h"
extern const char ECG_UUID[];
extern const char ECG_SERVICE_UUID[];
class EcgService {
	public: 
	
	/**
	* ECH Sensor Location 
	*/
	enum EcgSensorLocation{
		        /**
         * Other location.
         */
        LOCATION_OTHER = 0,

        /**
         * Chest.
         */
        LOCATION_CHEST = 1,

        /**
         * Wrist.
         */
        LOCATION_WRIST = 2,

        /**
         * Finger.
         */
        LOCATION_FINGER,

        /**
         * Hand.
         */
        LOCATION_HAND,

        /**
         * Earlobe.
         */
        LOCATION_EAR_LOBE,

        /**
         * Foot.
         */
        LOCATION_FOOT,
    };
	
	public:
	EcgService(BLE &_ble, int16_t ecgCounter, EcgSensorLocation location):
		ble(_ble),
		valueBytes(ecgCounter),
		ecgData(
			ECG_UUID,
			valueBytes.getPointer(),
			valueBytes.getNumValueBytes(),
			EcgValueBytes::MAX_VALUE_BYTES,
			GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY
		),
		ecgLocation(
            GattCharacteristic::UUID_BODY_SENSOR_LOCATION_CHAR,
            reinterpret_cast<uint8_t*>(&location)
        ) 
		{ ecgSetupService();}
		
    void updateEcgData(int16_t ecgCounter) {
        valueBytes.updateEcgData(ecgCounter);
        ble.gattServer().write(
            ecgData.getValueHandle(),
            valueBytes.getPointer(),
            valueBytes.getNumValueBytes()
        );
    }
	protected:
    void ecgSetupService(void) {
        GattCharacteristic *charTable[] = {
            &ecgData,
            &ecgLocation
        };
        GattService ecgService(
            ECG_SERVICE_UUID,
            charTable,
            sizeof(charTable) / sizeof(GattCharacteristic*)
        );

        ble.gattServer().addService(ecgService);
    }
	protected:
    /*
     * ECG measurement value.
     */
    struct EcgValueBytes {
        /* 1 byte for the Flags, and up to two bytes for heart rate value. */
        static const unsigned MAX_VALUE_BYTES = 3;
        static const unsigned FLAGS_BYTE_INDEX = 0;

        static const unsigned VALUE_FORMAT_BITNUM = 0;
        static const uint8_t  VALUE_FORMAT_FLAG = (1 << VALUE_FORMAT_BITNUM);

        EcgValueBytes(int16_t ecgCounter) : valueBytes()
        {
            updateEcgData(ecgCounter);
        }

        void updateEcgData(int16_t ecgCounter)
        {
            if (ecgCounter <= 127 &&  ecgCounter >= -128) {  //uint8_t = 2^8 different values 
                valueBytes[FLAGS_BYTE_INDEX] &= ~VALUE_FORMAT_FLAG; //need the flag, the flag indicates the value field
                valueBytes[FLAGS_BYTE_INDEX + 1] = ecgCounter;
            } else {
                valueBytes[FLAGS_BYTE_INDEX] |= VALUE_FORMAT_FLAG;
                valueBytes[FLAGS_BYTE_INDEX + 2] = (uint8_t)(ecgCounter & 0xFF);
                valueBytes[FLAGS_BYTE_INDEX + 1 ] = (uint8_t)(ecgCounter >> 8);
            }
        }

        uint8_t *getPointer(void)
        {
            return valueBytes;
        }

        const uint8_t *getPointer(void) const
        {
            return valueBytes;
        }

        unsigned getNumValueBytes(void) const
        {
            if (valueBytes[FLAGS_BYTE_INDEX] & VALUE_FORMAT_FLAG) {
                return 1 + sizeof(int16_t); //1 for the flag byte
            } else {
                return 1 + sizeof(int8_t);
            }
        }

    private:
        uint8_t valueBytes[MAX_VALUE_BYTES];
    };
	protected:
    BLE &ble;
	EcgValueBytes valueBytes;
    GattCharacteristic ecgData;
    ReadOnlyGattCharacteristic<uint8_t> ecgLocation;
};

#endif






