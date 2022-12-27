/////////////////////////////////////////////////////////////////////////////
// SLABCP2112.h
// For libSLABHIDtoSMBus.dylib version 1.0
// and Silicon Labs CP2112 HID to SMBus
/////////////////////////////////////////////////////////////////////////////

#ifndef SLAB_CP2112_H
#define SLAB_CP2112_H

/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////

#include "Types.h"

/////////////////////////////////////////////////////////////////////////////
// Return Code Definitions
/////////////////////////////////////////////////////////////////////////////

// HID_SMBUS_STATUS
typedef int HID_SMBUS_STATUS;

// HID_SMBUS_STATUS Return Codes
#define HID_SMBUS_SUCCESS							0x00
#define	HID_SMBUS_DEVICE_NOT_FOUND					0x01
#define HID_SMBUS_INVALID_HANDLE					0x02
#define	HID_SMBUS_INVALID_DEVICE_OBJECT				0x03
#define	HID_SMBUS_INVALID_PARAMETER					0x04
#define	HID_SMBUS_INVALID_REQUEST_LENGTH			0x05

#define	HID_SMBUS_READ_ERROR						0x10
#define	HID_SMBUS_WRITE_ERROR						0x11
#define	HID_SMBUS_READ_TIMED_OUT					0x12
#define	HID_SMBUS_WRITE_TIMED_OUT					0x13
#define	HID_SMBUS_DEVICE_IO_FAILED					0x14
#define HID_SMBUS_DEVICE_ACCESS_ERROR				0x15
#define HID_SMBUS_DEVICE_NOT_SUPPORTED				0x16

#define HID_SMBUS_UNKNOWN_ERROR						0xFF

// HID_SMBUS_TRANSFER_S0
typedef BYTE HID_SMBUS_S0;

#define HID_SMBUS_S0_IDLE							0x00
#define HID_SMBUS_S0_BUSY							0x01
#define HID_SMBUS_S0_COMPLETE						0x02
#define HID_SMBUS_S0_ERROR							0x03

// HID_SMBUS_TRANSFER_S1
typedef BYTE HID_SMBUS_S1;

// HID_SMBUS_TRANSFER_S0 = HID_SMBUS_S0_BUSY
#define HID_SMBUS_S1_BUSY_ADDRESS_ACKED				0x00
#define HID_SMBUS_S1_BUSY_ADDRESS_NACKED			0x01
#define HID_SMBUS_S1_BUSY_READING					0x02
#define HID_SMBUS_S1_BUSY_WRITING					0x03

// HID_SMBUS_TRANSFER_S0 = HID_SMBUS_S0_ERROR
#define HID_SMBUS_S1_ERROR_TIMEOUT_NACK				0x00
#define HID_SMBUS_S1_ERROR_TIMEOUT_BUS_NOT_FREE		0x01
#define HID_SMBUS_S1_ERROR_ARB_LOST					0x02
#define HID_SMBUS_S1_ERROR_READ_INCOMPLETE			0x03
#define HID_SMBUS_S1_ERROR_WRITE_INCOMPLETE			0x04
#define HID_SMBUS_S1_ERROR_SUCCESS_AFTER_RETRY		0x05

/////////////////////////////////////////////////////////////////////////////
// String Definitions
/////////////////////////////////////////////////////////////////////////////

// Product String Types
#define HID_SMBUS_GET_VID_STR						0x01
#define HID_SMBUS_GET_PID_STR						0x02
#define HID_SMBUS_GET_PATH_STR						0x03
#define HID_SMBUS_GET_SERIAL_STR					0x04
#define HID_SMBUS_GET_MANUFACTURER_STR				0x05
#define HID_SMBUS_GET_PRODUCT_STR					0x06

// String Lengths
#define HID_SMBUS_DEVICE_STRLEN						512

// HID_SMBUS_DEVICE_STR
typedef char HID_SMBUS_DEVICE_STR[HID_SMBUS_DEVICE_STRLEN];

/////////////////////////////////////////////////////////////////////////////
// SMBUS Definitions
/////////////////////////////////////////////////////////////////////////////

// SMbus Configuration Limits
#define HID_SMBUS_MIN_BIT_RATE						1
#define HID_SMBUS_MIN_TIMEOUT						0
#define HID_SMBUS_MAX_TIMEOUT						1000
#define HID_SMBUS_MAX_RETRIES						1000
#define HID_SMBUS_MIN_ADDRESS						0x02
#define HID_SMBUS_MAX_ADDRESS						0xFE

// Read/Write Limits
#define HID_SMBUS_MIN_READ_REQUEST_SIZE				1
#define HID_SMBUS_MAX_READ_REQUEST_SIZE				512
#define HID_SMBUS_MIN_TARGET_ADDRESS_SIZE			1
#define HID_SMBUS_MAX_TARGET_ADDRESS_SIZE			16
#define HID_SMBUS_MAX_READ_RESPONSE_SIZE			61
#define HID_SMBUS_MIN_WRITE_REQUEST_SIZE			1
#define HID_SMBUS_MAX_WRITE_REQUEST_SIZE			61

/////////////////////////////////////////////////////////////////////////////
// GPIO Definitions
/////////////////////////////////////////////////////////////////////////////

// GPIO Pin Direction Bit Value
#define HID_SMBUS_DIRECTION_INPUT					0
#define HID_SMBUS_DIRECTION_OUTPUT					1

// GPIO Pin Mode Bit Value
#define HID_SMBUS_MODE_OPEN_DRAIN					0
#define HID_SMBUS_MODE_PUSH_PULL					1

// GPIO Function Bitmask
#define HID_SMBUS_MASK_FUNCTION_GPIO_7_CLK			0x01
#define HID_SMBUS_MASK_FUNCTION_GPIO_0_TXT			0x02
#define HID_SMBUS_MASK_FUNCTION_GPIO_1_RXT			0x04

// GPIO Function Bit Value
#define HID_SMBUS_GPIO_FUNCTION						0
#define HID_SMBUS_SPECIAL_FUNCTION					1

// GPIO Pin Bitmask
#define HID_SMBUS_MASK_GPIO_0						0x01
#define HID_SMBUS_MASK_GPIO_1						0x02
#define HID_SMBUS_MASK_GPIO_2						0x04
#define HID_SMBUS_MASK_GPIO_3						0x08
#define HID_SMBUS_MASK_GPIO_4						0x10
#define HID_SMBUS_MASK_GPIO_5						0x20
#define HID_SMBUS_MASK_GPIO_6						0x40
#define HID_SMBUS_MASK_GPIO_7						0x80

/////////////////////////////////////////////////////////////////////////////
// Part Number Definitions
/////////////////////////////////////////////////////////////////////////////

// Part Numbers
#define HID_SMBUS_PART_CP2112						0x0C

/////////////////////////////////////////////////////////////////////////////
// User Customization Definitions
/////////////////////////////////////////////////////////////////////////////

// User-Customizable Field Lock Bitmasks
#define HID_SMBUS_LOCK_VID							0x01
#define HID_SMBUS_LOCK_PID							0x02
#define HID_SMBUS_LOCK_POWER						0x04
#define HID_SMBUS_LOCK_POWER_MODE					0x08
#define HID_SMBUS_LOCK_RELEASE_VERSION				0x10
#define HID_SMBUS_LOCK_MFG_STR						0x20
#define HID_SMBUS_LOCK_PRODUCT_STR					0x40
#define HID_SMBUS_LOCK_SERIAL_STR					0x80

// Field Lock Bit Values
#define HID_SMBUS_LOCK_UNLOCKED						1
#define HID_SMBUS_LOCK_LOCKED						0

// Power Max Value (500 mA)
#define HID_SMBUS_BUS_POWER_MAX						0xFA

// Power Modes
#define HID_SMBUS_BUS_POWER							0x00
#define HID_SMBUS_SELF_POWER_VREG_DIS				0x01
#define HID_SMBUS_SELF_POWER_VREG_EN				0x02

// USB Config Bitmasks
#define HID_SMBUS_SET_VID							0x01
#define HID_SMBUS_SET_PID							0x02
#define HID_SMBUS_SET_POWER							0x04
#define HID_SMBUS_SET_POWER_MODE					0x08
#define HID_SMBUS_SET_RELEASE_VERSION				0x10

// USB Config Bit Values
#define HID_SMBUS_SET_IGNORE						0
#define HID_SMBUS_SET_PROGRAM						1

// String Lengths
#define HID_SMBUS_CP2112_MFG_STRLEN					30
#define HID_SMBUS_CP2112_PRODUCT_STRLEN				30
#define HID_SMBUS_CP2112_SERIAL_STRLEN				30

// HID_SMBUS_MFG_STR
typedef char HID_SMBUS_CP2112_MFG_STR[HID_SMBUS_CP2112_MFG_STRLEN];

// HID_SMBUS_PRODUCT_STR
typedef char HID_SMBUS_CP2112_PRODUCT_STR[HID_SMBUS_CP2112_PRODUCT_STRLEN];

// HID_SMBUS_SERIAL_STR
typedef char HID_SMBUS_CP2112_SERIAL_STR[HID_SMBUS_CP2112_SERIAL_STRLEN];

/////////////////////////////////////////////////////////////////////////////
// Typedefs
/////////////////////////////////////////////////////////////////////////////

typedef void* HID_SMBUS_DEVICE;

/////////////////////////////////////////////////////////////////////////////
// Exported Library Functions
/////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// HidSmbus_GetNumDevices
HID_SMBUS_STATUS HidSmbus_GetNumDevices(DWORD* numDevices, WORD vid, WORD pid);

// HidSmbus_GetString
HID_SMBUS_STATUS HidSmbus_GetString(DWORD deviceNum, WORD vid, WORD pid, char* deviceString, DWORD options);

// HidSmbus_GetOpenedString
HID_SMBUS_STATUS HidSmbus_GetOpenedString(HID_SMBUS_DEVICE device, char* deviceString, DWORD options);

// HidSmbus_GetAttributes
HID_SMBUS_STATUS HidSmbus_GetAttributes(DWORD deviceNum, WORD vid, WORD pid, WORD* deviceVid, WORD* devicePid, WORD* deviceReleaseNumber);

// HidSmbus_GetOpenedAttributes
HID_SMBUS_STATUS HidSmbus_GetOpenedAttributes(HID_SMBUS_DEVICE device, WORD* deviceVid, WORD* devicePid, WORD* deviceReleaseNumber);

// HidSmbus_Open
HID_SMBUS_STATUS HidSmbus_Open(HID_SMBUS_DEVICE* device, DWORD deviceNum, WORD vid, WORD pid);

// HidSmbus_Close
HID_SMBUS_STATUS HidSmbus_Close(HID_SMBUS_DEVICE device);

// HidSmbus_IsOpened
HID_SMBUS_STATUS HidSmbus_IsOpened(HID_SMBUS_DEVICE device, BOOL* opened);

// HidSmbus_ReadRequest
HID_SMBUS_STATUS HidSmbus_ReadRequest(HID_SMBUS_DEVICE device, BYTE slaveAddress, WORD numBytesToRead);

// HidSmbus_AddressReadRequest
HID_SMBUS_STATUS HidSmbus_AddressReadRequest(HID_SMBUS_DEVICE device, BYTE slaveAddress, WORD numBytesToRead, BYTE targetAddressSize, BYTE targetAddress[16]);

// HidSmbus_ForceReadResponse
HID_SMBUS_STATUS HidSmbus_ForceReadResponse(HID_SMBUS_DEVICE device, WORD numBytesToRead);

// HidSmbus_ForceReadResponse
HID_SMBUS_STATUS HidSmbus_GetReadResponse(HID_SMBUS_DEVICE device, HID_SMBUS_S0* status, BYTE* buffer, BYTE bufferSize, BYTE* numBytesRead);

// HidSmbus_WriteRequest
HID_SMBUS_STATUS HidSmbus_WriteRequest(HID_SMBUS_DEVICE device, BYTE slaveAddress, BYTE* buffer, BYTE numBytesToWrite);

// HidSmbus_TransferStatusRequest
HID_SMBUS_STATUS HidSmbus_TransferStatusRequest(HID_SMBUS_DEVICE device);

// HidSmbus_GetTransferStatusResponse
HID_SMBUS_STATUS HidSmbus_GetTransferStatusResponse(HID_SMBUS_DEVICE device, HID_SMBUS_S0* status, HID_SMBUS_S1* detailedStatus, WORD* numRetries, WORD* bytesRead);

// HidSmbus_CancelTransfer
HID_SMBUS_STATUS HidSmbus_CancelTransfer(HID_SMBUS_DEVICE device);

// HidSmbus_SetTimeouts
HID_SMBUS_STATUS HidSmbus_SetTimeouts(HID_SMBUS_DEVICE device, DWORD responseTimeout);

// HidSmbus_GetTimeouts
HID_SMBUS_STATUS HidSmbus_GetTimeouts(HID_SMBUS_DEVICE device, DWORD* responseTimeout);

// HidSmbus_SetSmbusConfig
HID_SMBUS_STATUS HidSmbus_SetSmbusConfig(HID_SMBUS_DEVICE device, DWORD bitRate, BYTE address, BOOL autoReadRespond, WORD writeTimeout, WORD readTimeout, BOOL sclLowTimeout, WORD transferRetries);

// HidSmbus_GetSmbusConfig
HID_SMBUS_STATUS HidSmbus_GetSmbusConfig(HID_SMBUS_DEVICE device, DWORD* bitRate, BYTE* address, BOOL* autoReadRespond, WORD* writeTimeout, WORD* readTimeout, BOOL* sclLowtimeout, WORD* transferRetries);

// HidSmbus_Reset
HID_SMBUS_STATUS HidSmbus_Reset(HID_SMBUS_DEVICE device);

// HidSmbus_SetGpioConfig
HID_SMBUS_STATUS HidSmbus_SetGpioConfig(HID_SMBUS_DEVICE device, BYTE direction, BYTE mode, BYTE function, BYTE clkDiv);

// HidSmbus_GetGpioConfig
HID_SMBUS_STATUS HidSmbus_GetGpioConfig(HID_SMBUS_DEVICE device, BYTE* direction, BYTE* mode, BYTE* function, BYTE* clkDiv);

// HidSmbus_ReadLatch
HID_SMBUS_STATUS HidSmbus_ReadLatch(HID_SMBUS_DEVICE device, BYTE* latchValue);

// HidSmbus_WriteLatch
HID_SMBUS_STATUS HidSmbus_WriteLatch(HID_SMBUS_DEVICE device, BYTE latchValue, BYTE latchMask);

// HidSmbus_GetPartNumber
HID_SMBUS_STATUS HidSmbus_GetPartNumber(HID_SMBUS_DEVICE device, BYTE* partNumber, BYTE* version);

// HidSmbus_GetLibraryVersion
HID_SMBUS_STATUS HidSmbus_GetLibraryVersion(BYTE* major, BYTE* minor, BOOL* release);

// HidSmbus_GetHidLibraryVersion
HID_SMBUS_STATUS HidSmbus_GetHidLibraryVersion(BYTE* major, BYTE* minor, BOOL* release);

/////////////////////////////////////////////////////////////////////////////
// Exported Library Functions - Device Customization
/////////////////////////////////////////////////////////////////////////////

// HidSmbus_SetLock
HID_SMBUS_STATUS HidSmbus_SetLock(HID_SMBUS_DEVICE device, BYTE lock);

// HidSmbus_GetLock
HID_SMBUS_STATUS HidSmbus_GetLock(HID_SMBUS_DEVICE device, BYTE* lock);

// HidSmbus_SetUsbConfig
HID_SMBUS_STATUS HidSmbus_SetUsbConfig(HID_SMBUS_DEVICE device, WORD vid, WORD pid, BYTE power, BYTE powerMode, WORD releaseVersion, BYTE mask);

// HidSmbus_GetUsbConfig
HID_SMBUS_STATUS HidSmbus_GetUsbConfig(HID_SMBUS_DEVICE device, WORD* vid, WORD* pid, BYTE* power, BYTE* powerMode, WORD* releaseVersion);

// HidSmbus_SetManufacturingString
HID_SMBUS_STATUS HidSmbus_SetManufacturingString(HID_SMBUS_DEVICE device, char* manufacturingString, BYTE strlen);

// HidSmbus_GetManufacturingString
HID_SMBUS_STATUS HidSmbus_GetManufacturingString(HID_SMBUS_DEVICE device, char* manufacturingString, BYTE* strlen);

// HidSmbus_SetProductString
HID_SMBUS_STATUS HidSmbus_SetProductString(HID_SMBUS_DEVICE device, char* productString, BYTE strlen);

// HidSmbus_GetProductString
HID_SMBUS_STATUS HidSmbus_GetProductString(HID_SMBUS_DEVICE device, char* productString, BYTE* strlen);

// HidSmbus_SetSerialString
HID_SMBUS_STATUS HidSmbus_SetSerialString(HID_SMBUS_DEVICE device, char* serialString, BYTE strlen);

// HidSmbus_GetSerialString
HID_SMBUS_STATUS HidSmbus_GetSerialString(HID_SMBUS_DEVICE device, char* serialString, BYTE* strlen);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SLAB_CP2112_H
