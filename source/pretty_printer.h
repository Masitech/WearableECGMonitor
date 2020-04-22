
#include <mbed.h>
#include "ble/BLE.h"
#include "EcgUart.h"


inline void print_error(ble_error_t error, const char* msg)
{
    EcgUart.printf("%s: ", msg);
    switch(error) {
        case BLE_ERROR_NONE:
            EcgUart.printf("BLE_ERROR_NONE: No error");
            break;
        case BLE_ERROR_BUFFER_OVERFLOW:
            EcgUart.printf("BLE_ERROR_BUFFER_OVERFLOW: The requested action would cause a buffer overflow and has been aborted");
            break;
        case BLE_ERROR_NOT_IMPLEMENTED:
            EcgUart.printf("BLE_ERROR_NOT_IMPLEMENTED: Requested a feature that isn't yet implement or isn't supported by the target HW");
            break;
        case BLE_ERROR_PARAM_OUT_OF_RANGE:
            EcgUart.printf("BLE_ERROR_PARAM_OUT_OF_RANGE: One of the supplied parameters is outside the valid range");
            break;
        case BLE_ERROR_INVALID_PARAM:
            EcgUart.printf("BLE_ERROR_INVALID_PARAM: One of the supplied parameters is invalid");
            break;
        case BLE_STACK_BUSY:
            EcgUart.printf("BLE_STACK_BUSY: The stack is busy");
            break;
        case BLE_ERROR_INVALID_STATE:
            EcgUart.printf("BLE_ERROR_INVALID_STATE: Invalid state");
            break;
        case BLE_ERROR_NO_MEM:
            EcgUart.printf("BLE_ERROR_NO_MEM: Out of Memory");
            break;
        case BLE_ERROR_OPERATION_NOT_PERMITTED:
            EcgUart.printf("BLE_ERROR_OPERATION_NOT_PERMITTED");
            break;
        case BLE_ERROR_INITIALIZATION_INCOMPLETE:
            EcgUart.printf("BLE_ERROR_INITIALIZATION_INCOMPLETE");
            break;
        case BLE_ERROR_ALREADY_INITIALIZED:
            EcgUart.printf("BLE_ERROR_ALREADY_INITIALIZED");
            break;
        case BLE_ERROR_UNSPECIFIED:
            EcgUart.printf("BLE_ERROR_UNSPECIFIED: Unknown error");
            break;
        case BLE_ERROR_INTERNAL_STACK_FAILURE:
            EcgUart.printf("BLE_ERROR_INTERNAL_STACK_FAILURE: internal stack faillure");
            break;
				case BLE_ERROR_NOT_FOUND:
            EcgUart.printf("BLE_ERROR_INTERNAL_STACK_FAILURE: ble not found");
            break;
    }
    EcgUart.printf("\r\n");
}

/** print device address to the terminal */
inline void print_address(const Gap::Address_t &addr)
{
    EcgUart.printf("%02x:%02x:%02x:%02x:%02x:%02x\r\n",
           addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
}

inline void print_mac_address()
{
    /* Print out device MAC address to the console*/
    Gap::AddressType_t addr_type;
    Gap::Address_t address;
    BLE::Instance().gap().getAddress(&addr_type, address);
    EcgUart.printf("DEVICE MAC ADDRESS: ");
    print_address(address);
}

inline const char* phy_to_string(Gap::Phy_t phy) {
    switch(phy.value()) {
        case Gap::Phy_t::LE_1M:
            return "LE 1M";
        case Gap::Phy_t::LE_2M:
            return "LE 2M";
        case Gap::Phy_t::LE_CODED:
            return "LE coded";
        default:
            return "invalid PHY";
    }
}
