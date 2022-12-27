#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "smbus.h"

#define UINT16 UInt16
#define INT16 int16_t 

/*
 NOTE: As command response lengths may differ between gas gauges, ensure
 sbsCommandResponseLength contains the correct lengths for your
 particular device (check datasheet).
 
 For example, some typical variations:
 Manufacturer Name [0x20] = 20+1 bytes / 11+1 bytes
 Device Name [0x21] = 20+1 bytes / 7+1 bytes
 */
const WORD sbsCommandResponseLength[] = {
2, 2, 2, 2, 2, 2, 2, 2, 2, 2,   // 0x00 - 0x09
2, 2, 1, 1, 1, 2, 2, 2, 2, 2,   // 0x0A - 0x13
2, 2, 2, 2, 2, 2, 2, 2, 2, 0,   // 0x14 - 0x1D
0, 0, 12, 8, 5, 13,            // 0x1E - 0x23
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//0x24-0x2E
1,                              //0x2f
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //0x30-0x3b
2, 2, 2, 2 //0x3c-0x3f
};

enum ValueType
{
	ValueType_none = 0,
	ValueType_uint8,
	ValueType_int16,
	ValueType_uint16,
	ValueType_string,
	ValueType_bytes
};

const int sbsCommandValueType[] = {
ValueType_uint16, ValueType_uint16, ValueType_uint16, ValueType_uint16, ValueType_uint16, ValueType_uint16, ValueType_uint16, ValueType_uint16, ValueType_uint16, ValueType_uint16,   // 0x00 - 0x09
ValueType_int16, ValueType_uint16, ValueType_uint8, ValueType_uint8, ValueType_uint8, ValueType_uint16, ValueType_uint16, ValueType_uint16, ValueType_uint16, ValueType_uint16,   // 0x0A - 0x13
ValueType_uint16, ValueType_uint16, ValueType_uint16, ValueType_uint16, ValueType_uint16, ValueType_uint16, ValueType_uint16, ValueType_uint16, ValueType_uint16, 0,   // 0x14 - 0x1D
0, 0, ValueType_string, ValueType_string, ValueType_string, ValueType_bytes,            // 0x1E - 0x23
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//0x24-0x2E
ValueType_uint8,                              //0x2f;flags
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //0x30-0x3b
ValueType_uint16, ValueType_uint16, ValueType_uint16, ValueType_uint16 //0x3c-0x3f
};

#define PACK_STATUS_AND_PACK_CONFIGURATION 0x2f
#define VCELL4 0x3c
#define VCELL3 0x3d
#define VCELL2 0x3e
#define VCELL1 0x3f

enum sbsCommands {
    MANUFACTURER_ACCESS,        // 0x00
    REMAINING_CAPACITY_ALARM,   // 0x01
    REMAINING_TIME_ALARM,       // 0x02
    BATTERY_MODE,               // 0x03
    AT_RATE,                    // 0x04
    AT_RATE_TIME_TO_FULL,       // 0x05
    AT_RATE_TIME_TO_EMPTY,      // 0x06
    AT_RATE_OK,                 // 0x07
    TEMPERATURE,                // 0x08
    VOLTAGE,                    // 0x09
    CURRENT,                    // 0x0A
    AVERAGE_CURRENT,            // 0x0B
    MAX_ERROR,                  // 0x0C
    RELATIVE_STATE_OF_CHARGE,   // 0x0D
    ABSOLUTE_STATE_OF_CHARGE,   // 0x0E
    REMAINING_CAPACITY,         // 0x0F
    FULL_CHARGE_CAPACITY,       // 0x10
    RUN_TIME_TO_EMPTY,          // 0x11
    AVERAGE_TIME_TO_EMPTY,      // 0x12
    AVERAGE_TIME_TO_FULL,       // 0x13
    CHARGING_CURRENT,           // 0x14
    CHARGING_VOLTAGE,           // 0x15
    BATTERY_STATUS,             // 0x16
    CYCLE_COUNT,                // 0x17
    DESIGN_CAPACITY,            // 0x18
    DESIGN_VOLTAGE,             // 0x19
    SPECIFICATION_INFO,         // 0x1A
    MANUFACTURER_DATE,          // 0x1B
    SERIAL_NUMBER,              // 0x1C
    RESERVED1,                  // 0x1D
    RESERVED2,                  // 0x1E
    RESERVED3,                  // 0x1F
    MANUFACTURER_NAME,          // 0x20
    DEVICE_NAME,                // 0x21
    DEVICE_CHEMISTRY,           // 0x22
    MANUFACTURER_DATA           // 0x23
};

#define PACK_STATUS_AND_PACK_CONFIGURATION 0x2f
#define VCELL4 0x3c
#define VCELL3 0x3d
#define VCELL2 0x3e
#define VCELL1 0x3f

/////////////////////////////////////////////////////

#define BITRATE_HZ                  25000
#define ACK_ADDRESS                 0x02
#define AUTO_RESPOND                FALSE
#define WRITE_TIMEOUT_MS            1000
#define READ_TIMEOUT_MS             1000
#define TRANSFER_RETRIES            0
#define SCL_LOW_TIMEOUT             TRUE
#define RESPONSE_TIMEOUT_MS         1000

#define CHARGER_SLAVE_ADDRESS_W     0x12
#define BATTERY_SLAVE_ADDRESS_W     0x16

char* to_string(unsigned char* buffer, int valueType)
{
	char* s = (char*)malloc(256);
	memset(s,0,256);
	
	switch (valueType)
	{
		case ValueType_uint8:
		{
			int value = buffer[0];
			
			sprintf(s, "%d", value);
		}
			break;
			
		case ValueType_uint16:
		{
			UINT16 value = (buffer[1] << 8) | buffer[0];
			
			sprintf(s, "%d", value);
		}
			break;
			
		case ValueType_int16:
		{
		    INT16 value = (buffer[1] << 8) | buffer[0];	
			
			sprintf(s, "%d", value);
		}
			break;
			
		case ValueType_string:
		{
			const char* b = (const char*)buffer;
			int i;
			for(i = 1; i < (buffer[0] + 1); i++)
			{
				s[i-1] = b[i];
			}
		}
			break;
			
		case ValueType_bytes:
		{
			const char* b = (const char*)buffer;
			int i;
			int p = 0;
			const char* str = "0123456789ABCDEF";
			for(i = 1; i < (buffer[0] + 1); i++)
			{
				s[p++] = str[(buffer[i] >> 4)] ;
				s[p++] = str[(buffer[i] & 0x0f)];
				s[p++] = ' ';
			}
		}
			break;
			
		default:
		    s[0] = 0;
	};
	
	return s;
}



char* batteryModeString(uint16_t batteryMode)
{
    char* str = (char*)malloc(1024);
    char* fullString = str;
    memset(str, 0, 1024);
    
    strcpy(str, "\n    INTERNAL_CHARGE_CONTROLLER  ");
    str += strlen(str);
    
    strcpy(str, (batteryMode & 0x0001) ? "1" : "0");
    str += strlen(str);
    
    strcpy(str, "\n    PRIMARY_BATTERY_SUPPORT     ");
    str += strlen(str);
    
    strcpy(str, (batteryMode & 0x0002) ? "1" : "0");
    str += strlen(str);
    
    strcpy(str, "\n    RELEARN_FLAG                ");
    str += strlen(str);
    
    strcpy(str, (batteryMode & 0x0080) ? "1  (Relearn cycle requested)" : "0 (Battery OK)");
    str += strlen(str);
    
    strcpy(str, "\n    CHARGE_CONTROLLER_ENABLED   ");
    str += strlen(str);
    
    strcpy(str, (batteryMode & 0x0100) ? "1" : "0");
    str += strlen(str);
    
    strcpy(str, "\n    PRIMARY_BATTERY             ");
    str += strlen(str);
    
    strcpy(str, (batteryMode & 0x0200) ? "1" : "0");
    str += strlen(str);
    
    strcpy(str, "\n    ALARM_MODE                  ");
    str += strlen(str);
    
    strcpy(str, (batteryMode & 0x2000) ? "1  Disable alarm broadcast" : "0  Enable alarm broadcast (default)");
    str += strlen(str);
    
    strcpy(str, "\n    CHARGER_MODE                ");
    str += strlen(str);
    
    strcpy(str, (batteryMode & 0x4000) ? "1  Disable charging broadcast" : "0  Enable charging broadcast (default)");
    str += strlen(str);
    
    strcpy(str, "\n    CAPACITY_MODE               ");
    str += strlen(str);
    
    strcpy(str, (batteryMode & 0x8000) ? "1  Report in 10mW or 10 mWh" : "0  Report in mA or mAh (default)");
    str += strlen(str);
    
    return fullString;
}

char* batteryStatusString(uint16_t batteryStatus)
{
    //ALARM BITS
    //0x8000 OVER_CHARGED_ALARM
    //0x4000 TERMINATE_CHARGE_ALARM 0x2000 Reserved
    //0x1000 OVER_TEMP_ALARM
    //0x0800 TERMINATE_DISCHARGE_ALARM 0x0400 Reserved
    //0x0200 REMAINING_CAPACITY_ALARM 0x0100 REMAINING_TIME_ALARM
    //STATUS BITS
    //0x0080 Initialized
    //0x0040 DISCHARGING 0x0020 FULLY_CHARGED 0x0010 FULLY_DISCHARGED
    
    char* str = (char*)malloc(1024);
    char* fullString = str;
    memset(str, 0, 1024);
    
    strcpy(str, "\n    ALARM BITS");
    str += strlen(str);
    
    strcpy(str, "\n        OVER_CHARGED_ALARM         ");
    str += strlen(str);
    
    strcpy(str, (batteryStatus & 0x8000) ? "1" : "0");
    str += strlen(str);
    
    strcpy(str, "\n        TERMINATE_CHARGE_ALARM     ");
    str += strlen(str);
    
    strcpy(str, (batteryStatus & 0x4000) ? "1" : "0");
    str += strlen(str);
    
    strcpy(str, "\n        OVER_TEMP_ALARM            ");
    str += strlen(str);
    
    strcpy(str, (batteryStatus & 0x1000) ? "1" : "0");
    str += strlen(str);
    
    strcpy(str, "\n        TERMINATE_DISCHARGE_ALARM  ");
    str += strlen(str);
    
    strcpy(str, (batteryStatus & 0x0800) ? "1" : "0");
    str += strlen(str);
    
    strcpy(str, "\n        REMAINING_CAPACITY_ALARM   ");
    str += strlen(str);
    
    strcpy(str, (batteryStatus & 0x0200) ? "1" : "0");
    str += strlen(str);
    
    strcpy(str, "\n        REMAINING_TIME_ALARM       ");
    str += strlen(str);
    
    strcpy(str, (batteryStatus & 0x0100) ? "1" : "0");
    str += strlen(str);
    
    strcpy(str, "\n    STATUS BITS");
    str += strlen(str);
    
    strcpy(str, "\n        Initialized       ");
    str += strlen(str);
    
    strcpy(str, (batteryStatus & 0x0080) ? "1" : "0");
    str += strlen(str);
    
    strcpy(str, "\n        DISCHARGING       ");
    str += strlen(str);
    
    strcpy(str, (batteryStatus & 0x0040) ? "1" : "0");
    str += strlen(str);
    
    strcpy(str, "\n        FULLY_CHARGED     ");
    str += strlen(str);
    
    strcpy(str, (batteryStatus & 0x0020) ? "1" : "0");
    str += strlen(str);
    
    strcpy(str, "\n        FULLY_DISCHARGED  ");
    str += strlen(str);
    
    strcpy(str, (batteryStatus & 0x0010) ? "1" : "0");
    str += strlen(str);
    
    strcpy(str, "\n    ERROR CODE: ");
    str += strlen(str);
    
    switch (batteryStatus & 0x0007)
    {
        case 7:
            strcpy(str, "7 Unknown Error");
            break;
            
        case 6:
            strcpy(str, "6 BadSize");
            break;
            
        case 5:
            strcpy(str, "5 Overflow/Underflow");
            break;
            
        case 4:
            strcpy(str, "4 AccessDenied");
            break;
            
        case 3:
            strcpy(str, "3 UnsupportedCommand");
            break;
            
        case 2:
            strcpy(str, "2 ReservedCommand");
            break;
            
        case 1:
            strcpy(str, "1 Busy");
            break;
            
        case 0:
            strcpy(str, "0 OK");
            break;
            
        default:
            strcpy(str, "BUG");
    }
    
    return fullString;
}

char* packStatusAndConfig(uint16_t packStatusNCfg)
{
    char* str = (char*)malloc(1024);
    char* fullString = str;
    memset(str, 0, 1024);
    
    strcpy(str, "\n    PACK STATUS");
    str += strlen(str);
    
    strcpy(str, "\n        AFE    ");
    str += strlen(str);
    
    strcpy(str, (packStatusNCfg & 0x0080) ? "1  bq29311 integrity check failed" : "0  bq29311 integrity check passed");
    str += strlen(str);
    
    strcpy(str, "\n        EDV2   ");
    str += strlen(str);
    
    strcpy(str, (packStatusNCfg & 0x0040) ? "1  Voltage ≤ EDV2 threshold" : "0  Voltage > EDV2 threshold (discharging)");
    str += strlen(str);
    
    strcpy(str, "\n        SS     ");
    str += strlen(str);
    
    strcpy(str, (packStatusNCfg & 0x0020) ? "1  The bq20851−V1P2 is in the sealed state" : "0  The bq20851−V1P2 is in the unsealed state");
    str += strlen(str);
    
    strcpy(str, "\n        VDQ    ");
    str += strlen(str);
    
    strcpy(str, (packStatusNCfg & 0x0010) ? "1  Discharge cycle valid" : "0  Discharge cycle not valid");
    str += strlen(str);
    
    strcpy(str, "\n        SOV    ");
    str += strlen(str);
    
    strcpy(str, (packStatusNCfg & 0x0004) ? "1  Safety limits exceeded" : "0  Safety limits not exceeded");
    str += strlen(str);
    
    strcpy(str, "\n        CVOV   ");
    str += strlen(str);
    
    strcpy(str, (packStatusNCfg & 0x0002) ? "1  A secondary protection limit exceeded(Prolonged Overcurrent, Overvoltage, or Overtemperature)" : "0  No secondary protection limits exceeded");
    str += strlen(str);
    
    strcpy(str, "\n        CVUV   ");
    str += strlen(str);
    
    strcpy(str, (packStatusNCfg & 0x0001) ? "1  A secondary protection limit exceeded(overload or overdischarge)" : "0  No secondary protection limits exceeded");
    str += strlen(str);
    
    strcpy(str, "\n    PACK CONFIGURATION");
    str += strlen(str);
    
    strcpy(str, "\n        DMODE    ");
    str += strlen(str);
    
    strcpy(str, (packStatusNCfg & 0x8000) ? "1  LEDs reflect RelativeStateOfCharge" : "0  LEDs reflect AbsoluteStateOfCharge");
    str += strlen(str);
    
    strcpy(str, "\n        HPE      ");
    str += strlen(str);
    
    strcpy(str, (packStatusNCfg & 0x1000) ? "1  PEC byte on alarm warning to host" : "0  No PEC byte on alarm warning to host");
    str += strlen(str);
    
    strcpy(str, "\n        CPE      ");
    str += strlen(str);
    
    strcpy(str, (packStatusNCfg & 0x0800) ? "1  PEC byte on broadcasts to charger" : "0  No PEC byte on broadcasts to charger");
    str += strlen(str);
    
    strcpy(str, "\n        SM       ");
    str += strlen(str);
    
    strcpy(str, (packStatusNCfg & 0x0400) ? "1  Broadcasts to host and charger disabled" : "0  Broadcasts to host and charger enabled");
    str += strlen(str);
    
    strcpy(str, "\n        CC1-СС0  ");
    str += strlen(str);
    
    uint16_t cellsCountCfg = packStatusNCfg & 0x0300;
    
    if (cellsCountCfg == 0x0300)
    {
        strcpy(str, "1-1  four series cells");
    }
    else if (cellsCountCfg == 0x0200)
    {
        strcpy(str, "1-0  three series cells");
    }
    else if (cellsCountCfg == 0x0100)
    {
        strcpy(str, "0-1  unexpected config");
    }
    else
    {
        strcpy(str, "0-0  unexpected config");
    }
    
    str += strlen(str);
    
    return fullString;
}

void print_value(HID_SMBUS_DEVICE m_hidSmbus, int smb_command, const char* name, char* unitName)
{
	BYTE                buffer[HID_SMBUS_MAX_READ_RESPONSE_SIZE];
    BYTE                targetAddress[16];
	
    targetAddress[0] = smb_command;
	int responseLength = sbsCommandResponseLength[smb_command];
    if (SMBus_Read(m_hidSmbus, buffer, BATTERY_SLAVE_ADDRESS_W, responseLength, 1, targetAddress) != responseLength)
    {
        fprintf(stderr,"ERROR: Could not perform SMBus read.\r\n");
        //SMBus_Close(m_hidSmbus);
        return;
    }
	
	if (BATTERY_STATUS == smb_command)
	{
		uint16_t i = (buffer[1] << 8) | buffer[0];
		unitName = batteryStatusString(i);
	}
	else if (BATTERY_MODE == smb_command)
	{
		uint16_t i = (buffer[1] << 8) | buffer[0];
		unitName = batteryModeString(i);
	}
	else if (PACK_STATUS_AND_PACK_CONFIGURATION == smb_command)
	{
		uint16_t i = (buffer[1] << 8) | buffer[0];
		unitName = packStatusAndConfig(i);
	}
	
    fprintf(stderr, "%s = %s %s\r\n",name, to_string(buffer,sbsCommandValueType[smb_command]), unitName);
}

void print_flash_value(HID_SMBUS_DEVICE m_hidSmbus, UInt8 address, const char* name)
{
	BYTE                buffer[HID_SMBUS_MAX_READ_RESPONSE_SIZE];
	memset(buffer, 0, sizeof(buffer));
	ReadFlash(m_hidSmbus, BATTERY_SLAVE_ADDRESS_W, address, buffer);
    //{
        //fprintf(stderr,"ERROR: Could not perform SMBus read.\r\n");
        //SMBus_Close(m_hidSmbus);
        //return;
    //}
	
	fprintf(stderr, "0x%02x\t0x%02x\r\n", address, buffer[0]);
}

int main(int argc, char* argv[])
{
    HID_SMBUS_DEVICE    m_hidSmbus;
	
    // Open device
    if(SMBus_Open(&m_hidSmbus) != 0)
    {
        fprintf(stderr,"ERROR: Could not open device.\r\n");
        SMBus_Close(m_hidSmbus);
        return -1;
    }
    fprintf(stderr,"Device successfully opened.\r\n");
	
    // Configure device
    if(SMBus_Configure(m_hidSmbus, BITRATE_HZ, ACK_ADDRESS, AUTO_RESPOND, WRITE_TIMEOUT_MS, READ_TIMEOUT_MS, SCL_LOW_TIMEOUT, TRANSFER_RETRIES, RESPONSE_TIMEOUT_MS) != 0)
    {
        fprintf(stderr,"ERROR: Could not configure device.\r\n");
        SMBus_Close(m_hidSmbus);
        return -1;
    }
    fprintf(stderr,"Device successfully configured.\r\n");
	
	uint8_t the_patch[7][2] = {
		{ 0x29, 0xC9 },
		{ 0x86, 0x24 },
		{ 0x87, 0x54 },
		{ 0x88, 0x25 },
		{ 0x89, 0x80 },
		{ 0x35, 0x0B },
		{ 0x36, 0xB8 },
	};
	
	if (argc >= 2 &&
		0 == strcmp(argv[1], "patch"))
	{
		int q=0;
		for (; q<(sizeof(the_patch)/sizeof(the_patch[0]));)
		{
			uint8_t address = the_patch[q][0];
			uint8_t value = the_patch[q][1];
			
			fprintf(stderr,"Applying the patch. Writing at address: 0x%02x value: 0x%02x\r\n", address, value);
			WriteFlash(m_hidSmbus, BATTERY_SLAVE_ADDRESS_W, address, value);

			sleep(1.0);
			
			uint8_t buffer[2];
			buffer[0] = 0;
			buffer[1] = 0;
			
			ReadFlash(m_hidSmbus, BATTERY_SLAVE_ADDRESS_W, address, &buffer);
			
			sleep(1.0);
			
			if (buffer[0] == value)
			{
				fprintf(stderr,"Success\r\n");
				++q;
			}
			else
			{
				fprintf(stderr,"Failed. Retrying...\r\n");
			}
		}
#if 1
		WriteFlash(m_hidSmbus, BATTERY_SLAVE_ADDRESS_W, 0x29, 0xC9);//0x29, 0xC9
	    WriteFlash(m_hidSmbus, BATTERY_SLAVE_ADDRESS_W, 0x86, 0x24);//0x86, 0x24
	    WriteFlash(m_hidSmbus, BATTERY_SLAVE_ADDRESS_W, 0x87, 0x54);//0x87, 0x54
	    WriteFlash(m_hidSmbus, BATTERY_SLAVE_ADDRESS_W, 0x88, 0x25);//0x88, 0x25
	    WriteFlash(m_hidSmbus, BATTERY_SLAVE_ADDRESS_W, 0x89, 0x80);//0x89, 0x80
		
		fprintf(stderr,"The patch which disables chemistry specific features of the battery has been applied\r\n");
#endif
	}
	else
	{
		fprintf(stderr,"To apply patch which disables chemistry specific features of the battery you should run the following command in the terminal:\r\n");
		fprintf(stderr,"    %s patch\r\n\r\n", argv[0]);
	}

	fprintf(stderr,"\r\nRegisters:\r\n\r\n");
	
	//print_value_2bytes(m_hidSmbus, MANUFACTURER_ACCESS, 0x0001, ValueType_uint16, 2, "Device Type", "");
	//print_value_2bytes(m_hidSmbus, MANUFACTURER_ACCESS, 0x0002, ValueType_uint16, 2, "Firmware Version", "");
	//print_value_2bytes(m_hidSmbus, MANUFACTURER_ACCESS, 0x0003, ValueType_uint16, 2, "EDV Level", "mV");
	//print_value_2bytes(m_hidSmbus, MANUFACTURER_ACCESS, 0x062b, ValueType_uint16, 2, "Seal", "");
#if 1
	print_value(m_hidSmbus, MANUFACTURER_ACCESS, "Manufacturer Access", "");
	print_value(m_hidSmbus, BATTERY_MODE, "Battery Mode", "(2 bytes Bitmap)");
	print_value(m_hidSmbus, BATTERY_STATUS, "Battery Status", "(2 bytes Bitmap)");
	print_value(m_hidSmbus, PACK_STATUS_AND_PACK_CONFIGURATION, "Pack Status and Pack Configuration", "(2 bytes Bitmap)");
	print_value(m_hidSmbus, MANUFACTURER_DATA, "Manufacturer Data", "bytes array(max 12 bytes)");
	print_value(m_hidSmbus, MAX_ERROR, "Max Error", "%");
    print_value(m_hidSmbus, REMAINING_CAPACITY_ALARM, "Remaining Capacity Alarm", "mAh");
	print_value(m_hidSmbus, REMAINING_TIME_ALARM, "Remaining Time Alarm", "min(s)");
	print_value(m_hidSmbus, AT_RATE, "At Rate", "min(s)");
	print_value(m_hidSmbus, AT_RATE_TIME_TO_FULL, "At Rate time to full", "min(s)");
	print_value(m_hidSmbus, AT_RATE_TIME_TO_EMPTY, "At Rate time to empty", "min(s)");
	print_value(m_hidSmbus, AT_RATE_OK, "At Rate OK", "");
	print_value(m_hidSmbus, TEMPERATURE, "Temperature", "?");
	print_value(m_hidSmbus, VOLTAGE, "Voltage", "mV");
	print_value(m_hidSmbus, CURRENT, "Current", "mA");
	print_value(m_hidSmbus, AVERAGE_CURRENT, "Average Current", "mA");
	print_value(m_hidSmbus, RELATIVE_STATE_OF_CHARGE, "Relative State Of Charge(RSOC)", "%");
	print_value(m_hidSmbus, ABSOLUTE_STATE_OF_CHARGE, "Absolute State Of Charge(ASOC)", "%");
	print_value(m_hidSmbus, REMAINING_CAPACITY, "Remaining Capacity", "mAh");
	print_value(m_hidSmbus, FULL_CHARGE_CAPACITY, "Full Charge Capacity", "mAh");
	print_value(m_hidSmbus, RUN_TIME_TO_EMPTY, "Run Time to Empty", "min(s)");
	print_value(m_hidSmbus, AVERAGE_TIME_TO_EMPTY, "Average Time to Empty", "min(s)");
	print_value(m_hidSmbus, AVERAGE_TIME_TO_FULL, "Average Time to Full", "min(s)");
	print_value(m_hidSmbus, CHARGING_CURRENT, "Charging Current", "mA");
	print_value(m_hidSmbus, CHARGING_VOLTAGE, "Charging Voltage", "mV");
	print_value(m_hidSmbus, MANUFACTURER_DATE, "Manufacturer date", "");
	print_value(m_hidSmbus, SERIAL_NUMBER, "Serial Number", "");
	print_value(m_hidSmbus, MANUFACTURER_NAME, "Manufacturer Name", "");
	print_value(m_hidSmbus, DEVICE_NAME, "Device Name", "");
	print_value(m_hidSmbus, DEVICE_CHEMISTRY, "Device Chemistry", "");
	print_value(m_hidSmbus, CYCLE_COUNT, "Cycle Count", "");
	print_value(m_hidSmbus, DESIGN_CAPACITY, "Design Capacity", "mAh");
	print_value(m_hidSmbus, DESIGN_VOLTAGE, "Design Voltage", "mV");
#endif	
	print_value(m_hidSmbus, VCELL4, "Cell 4 Voltage", "mV");
	print_value(m_hidSmbus, VCELL3, "Cell 3 Voltage", "mV");
	print_value(m_hidSmbus, VCELL2, "Cell 2 Voltage", "mV");
	print_value(m_hidSmbus, VCELL1, "Cell 1 Voltage", "mV");
	
	fprintf(stderr,"\r\n\r\nData flash bytes:\r\nAddr\tValue\r\n");
	int j;
	for (j = 0; j<251; j+=1)
	{
	    print_flash_value(m_hidSmbus, j, "");
	}
#if 0
    // Check if charger is present
    // Charger Status [0x13]
    targetAddress[0] = 0x13;
    if (SMBus_Read(m_hidSmbus, buffer, CHARGER_SLAVE_ADDRESS_W, 2, 1, targetAddress) != 2)
    {
        fprintf(stderr, "ERROR: Could not perform SMBus read.\r\n");
        //SMBus_Close(m_hidSmbus);
        //return -1;
    }
	else
	{
        UINT16 chargerStatus = (buffer[1] << 8) | buffer[0];
        if (chargerStatus & 0x8000)
        {
            fprintf(stderr, "Charger connected.\r\n");
        }
        else
        {
            fprintf(stderr, "Charger NOT connected.\r\n");
        }
	}
#endif
    // Success
    SMBus_Close(m_hidSmbus);
    return 0;
}
