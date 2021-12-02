/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "project.h"
#include <stdbool.h>


#define NUMBER_OF_SENSORS   (0x16)
#define TRANSFER_CMPLT      (0x00u)
#define SLAVE_NOT_READY     (0x01u)
#define TRANSFER_ERROR      (0xFFu)

#define SENSOR_BUFFER_SIZE  (300u)
#define MESSAGE_HEADER_SIZE 5
#define UART_BUFFER_SIZE    (SENSOR_BUFFER_SIZE + MESSAGE_HEADER_SIZE)

uint8 sensorValueBuffer[SENSOR_BUFFER_SIZE];
uint8 uartBuffer[UART_BUFFER_SIZE];

typedef struct
{
    uint16 i2cAddr;
    uint8 nbTaxels;
    bool isOnline;
    bool wasRead;
    uint8 nbReadTry;
    
} SensorInfoStruct;

SensorInfoStruct sensorList[NUMBER_OF_SENSORS];

uint16 sensorAddrList[] = 
    {0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x08, 0x09, 0x0A, 0x0B, 
     0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};

uint16 nbTaxelList[] = 
    {67, 28, 28, 28, 79, 67, 31, 31, 31, 79, 67, 
     66, 66, 66, 79, 67, 6, 6, 6, 6, 122, 119};
    
uint32 readSensor(const SensorInfoStruct* sensor);
uint32 startCapSenseAcquisition();
void initSensorsStructs();
void resetSensorsReadStatus();
void readSensorsValues();
void sendDataToUART(const SensorInfoStruct* sensor);
int main(void);
    
/* [] END OF FILE */
