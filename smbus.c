#include "smbus.h"

#include <stdio.h>
#include <stdlib.h>

#define VID 0x10C4
#define PID 0xEA90

#define INT HID_SMBUS_STATUS
#define UINT16 UInt16

INT SMBus_Open(HID_SMBUS_DEVICE *device)
{
    INT                     deviceNum = -1;
    DWORD                   numDevices;
    HID_SMBUS_DEVICE_STR    deviceString;
    HID_SMBUS_STATUS        status;

    // Search for device
    if(HidSmbus_GetNumDevices(&numDevices, VID, PID) == HID_SMBUS_SUCCESS)
    {
		DWORD i;
        for (i = 0; i < numDevices; i++)
        {
            if(HidSmbus_GetString(i, VID, PID, deviceString, HID_SMBUS_GET_SERIAL_STR) == HID_SMBUS_SUCCESS)
            {
                deviceNum = i;
                break;
            }
        }
    }

    // Device not found
    if(deviceNum == -1)
    {
        return -1;
    }
    // Device found
    else
    {
        // Attempt open
        status = HidSmbus_Open(device, deviceNum, VID, PID);
        // Check status
        if(status != HID_SMBUS_SUCCESS)
        {
            return -1;
        }
    }

    // Success
    return 0;
}

INT SMBus_Close(HID_SMBUS_DEVICE device)
{
    HID_SMBUS_STATUS status;

    // Attempt close
    status = HidSmbus_Close(device);
    // Check status
    if(status != HID_SMBUS_SUCCESS)
    {
        return -1;
    }

    return 0;
}

INT SMBus_Reset(HID_SMBUS_DEVICE device)
{
    BOOL                opened;
    HID_SMBUS_STATUS    status;

    // Make sure that the device is opened
    if(HidSmbus_IsOpened(device, &opened) == HID_SMBUS_SUCCESS && opened)
    {
        // Attempt reset
        status = HidSmbus_Reset(device);
        // Check status
        if(status != HID_SMBUS_SUCCESS)
        {
            return -1;
        }
    }

    return 0;
}

INT SMBus_Configure(HID_SMBUS_DEVICE device, DWORD bitRate, BYTE address, BOOL autoReadRespond, WORD writeTimeout, WORD readTimeout, BOOL sclLowTimeout, WORD transferRetries, DWORD responseTimeout)
{
    BOOL                opened;
    HID_SMBUS_STATUS    status;

    // Make sure that the device is opened
    if(HidSmbus_IsOpened(device, &opened) == HID_SMBUS_SUCCESS && opened)
    {
        // Attempt configuration
        status =  HidSmbus_SetSmbusConfig(device, bitRate, address, autoReadRespond, writeTimeout, readTimeout, sclLowTimeout, transferRetries);
        // Check status
        if(status != HID_SMBUS_SUCCESS)
        {
            return -1;
        }

        // Set response timeout
        status = HidSmbus_SetTimeouts(device, responseTimeout);
        // Check status
        if(status != HID_SMBUS_SUCCESS)
        {
            return -1;
        }
    }

    return 0;
}

INT SMBus_Read(HID_SMBUS_DEVICE device, BYTE *buffer, BYTE slaveAddress, WORD numBytesToRead, BYTE targetAddressSize, BYTE *targetAddress)
{
    BOOL                opened;
    HID_SMBUS_STATUS    status;
    HID_SMBUS_S0        status0;
    HID_SMBUS_S1        status1;
    BYTE                numBytesRead = 0;
    BYTE                totalNumBytesRead = 0;
    WORD                numRetries;
    WORD                bytesRead;
    BYTE                _buffer[HID_SMBUS_MAX_READ_RESPONSE_SIZE];

    // Make sure that the device is opened
    if(HidSmbus_IsOpened(device, &opened) == HID_SMBUS_SUCCESS && opened)
    {
        // Issue a read request
        status = HidSmbus_AddressReadRequest(device, slaveAddress, numBytesToRead, targetAddressSize, targetAddress);
        // Check status
        if(status != HID_SMBUS_SUCCESS)
        {
            return -1;
        }

        // Issue transfer status request
        status = HidSmbus_TransferStatusRequest(device);
        // Check status
        if (status != HID_SMBUS_SUCCESS)
        {
            return -1;
        }
        // Wait for transfer status response
        status = HidSmbus_GetTransferStatusResponse(device, &status0, &status1, &numRetries, &bytesRead);
        // Check status
        if (status != HID_SMBUS_SUCCESS)
        {
            return -1;
        }

        // Notify device that it should send a read response back
        status = HidSmbus_ForceReadResponse(device, numBytesToRead);
        // Check status
        if (status != HID_SMBUS_SUCCESS)
        {
            return -1;
        }

        // Wait for a read response
        do
        {
            status = HidSmbus_GetReadResponse(device, &status0, _buffer, HID_SMBUS_MAX_READ_RESPONSE_SIZE, &numBytesRead);
            // Check status
            if (status != HID_SMBUS_SUCCESS)
            {
                return -1;
            }
            memcpy(&buffer[totalNumBytesRead], _buffer, numBytesRead);
            totalNumBytesRead += numBytesRead;
        } while (totalNumBytesRead < numBytesToRead);
    }
    else
    {
        return -1;
    }

    // Success
    return totalNumBytesRead;
}

INT SMBus_Write(HID_SMBUS_DEVICE device, BYTE *buffer, BYTE slaveAddress, BYTE numBytesToWrite)
{
    BOOL                opened;
    HID_SMBUS_STATUS    status;
    HID_SMBUS_S0        status0;
    HID_SMBUS_S1        status1;
    WORD                numRetries;
    WORD                bytesRead;

    // Make sure that the device is opened
    if (HidSmbus_IsOpened(device, &opened) == HID_SMBUS_SUCCESS && opened)
    {
        // Issue write request
        status = HidSmbus_WriteRequest(device, slaveAddress, buffer, numBytesToWrite);
        // Check status
        if (status != HID_SMBUS_SUCCESS)
        {
            return -1;
        }

        // Wait for transfer to complete
        do
        {
            // Issue transfer status request
            status = HidSmbus_TransferStatusRequest(device);
            // Check status
            if (status != HID_SMBUS_SUCCESS)
            {
                return -1;
            }

            // Wait for transfer status response
            status = HidSmbus_GetTransferStatusResponse(device, &status0, &status1, &numRetries, &bytesRead);
            // Check status
            if (status != HID_SMBUS_SUCCESS)
            {
                return -1;
            }
        } while (status0 != HID_SMBUS_S0_COMPLETE);
    }
    else
    {
        return -1;
    }

    // Success
    return 0;
}

#define FLASH_ADDRESS_SIZE 5

UINT16 ReadFlash(HID_SMBUS_DEVICE device, BYTE slaveAddress, UINT16 regAddr, UInt16 *data)
{
	BYTE dataLen = 2;
    BOOL                opened;                 //Открылось ли устройство
    HID_SMBUS_STATUS    status;
    HID_SMBUS_S0        status0;                //Расширенный статус 0
    HID_SMBUS_S1        status1;                //Расширенный статус 1
    WORD                numRetries;             //Количество повторов чтения
    WORD                bytesRead;              //Количество прочитанных байт HidSmbus_GetTransferStatusResponse
    BYTE                numBytesRead;           //Количество прочитанных байт HidSmbus_GetReadResponse
    BYTE                totalNumBytesRead = 0;  //Сколько всего байт прочитано
    BYTE                buffer[HID_SMBUS_MAX_READ_RESPONSE_SIZE]={0}; //Приемный буфер
    BYTE                address[3]={0};
    BYTE                magic_word [1] = {0x51}; // 0xff - наверное команда чтения флеш, 0x03 - количество отправляемых байт
	
    address[0] = magic_word[0];
    address[1] = (BYTE)regAddr&0xff;
    address[2] = 0;
	
	//    writeRequest(device, address, sizeof(address));
	//    readRequest(device, data, dataLen);
	
    if (HidSmbus_IsOpened(device, &opened) == HID_SMBUS_SUCCESS && opened)
    {
        // Issue write transfer request
        status = HidSmbus_WriteRequest(device, slaveAddress/*BATTERY_ADDRESS << 1*/, address, 3/*sizeof address*/);
        if (status!=HID_SMBUS_SUCCESS)return FALSE;
		
        if (HidSmbus_TransferStatusRequest(device)!=HID_SMBUS_SUCCESS)return FALSE;
		
        if (HidSmbus_GetTransferStatusResponse(device, &status0,
                                               &status1, &numRetries,
                                               &bytesRead)!=HID_SMBUS_SUCCESS)return FALSE;
		
        if (status1 == HID_SMBUS_S1_BUSY_ADDRESS_NACKED)
        {
            HidSmbus_Close(device);
            return FALSE;
        }
		
		BYTE targetAddress[1];
		targetAddress[0] = 0x52;
		
        status = HidSmbus_AddressReadRequest(device, slaveAddress, 2, 1, targetAddress);
		//(device, slaveAddress/*BATTERY_ADDRESS << 1*/, dataLen);
        if (status!=HID_SMBUS_SUCCESS)return FALSE;
		
        status = HidSmbus_ForceReadResponse(device, dataLen);
        if (status!=HID_SMBUS_SUCCESS)return FALSE;
		
        do
        {
            status = HidSmbus_GetReadResponse(device, &status0,
                                              buffer, HID_SMBUS_MAX_READ_RESPONSE_SIZE,
                                              &numBytesRead);
			
            if (status!=HID_SMBUS_SUCCESS)return FALSE;
            memcpy(&data[totalNumBytesRead], buffer, numBytesRead); //Копируем часть прочитанных данных во временный
			//буфер, начиная с адреса totalNumBytesRead
            totalNumBytesRead += numBytesRead;                      //Увеличиваем адрес на величину прочитанных байт
        } while (totalNumBytesRead < dataLen);                      //Все ли мы прочитали
		
        return TRUE;
    }
    return FALSE;
}

UINT16 WriteFlash(HID_SMBUS_DEVICE device, BYTE slaveAddress, UINT16 regAddr, BYTE data)
{
	BYTE dataLen = 1;
    BOOL                opened;                 //Открылось ли устройство
    HID_SMBUS_STATUS    status;
    HID_SMBUS_S0        status0;                //Расширенный статус 0
    HID_SMBUS_S1        status1;                //Расширенный статус 1
    WORD                numRetries;             //Количество повторов чтения HidSmbus_GetTransferStatusResponse
    WORD                bytesRead;              //Количество прочитанных байт HidSmbus_GetTransferStatusResponse
    BYTE                buffer[HID_SMBUS_MAX_READ_RESPONSE_SIZE]={0}; //Приемный буфер
    BYTE                magic_word [1] = {0x50}; // 0xff - наверное команда доступа во флеш, 0x01 - команда записи
	
    buffer[0] = magic_word[0];
    buffer[1] = (BYTE)regAddr&0xff;
    buffer[2] = data;	
	
    if (HidSmbus_IsOpened(device, &opened) == HID_SMBUS_SUCCESS && opened)
    {
        // Issue write transfer request
        status = HidSmbus_WriteRequest(device, /*BATTERY_ADDRESS << 1*/slaveAddress, buffer, 3);
        if (status!=HID_SMBUS_SUCCESS)return FALSE;
        if (HidSmbus_TransferStatusRequest(device)!=HID_SMBUS_SUCCESS)return FALSE;
		
        if (HidSmbus_GetTransferStatusResponse(device, &status0,
                                               &status1, &numRetries,
                                               &bytesRead)!=HID_SMBUS_SUCCESS)return FALSE;
		
        if (status1 == HID_SMBUS_S1_BUSY_ADDRESS_NACKED)
        {
            HidSmbus_Close(device);
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}


