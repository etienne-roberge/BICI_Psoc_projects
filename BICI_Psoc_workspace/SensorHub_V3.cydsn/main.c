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
#include "comm_driver.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"


/*******************************************************************************
* uint32 readSensor(const SensorInfoStruct* sensor)
*
* Hub initiates the transfer to read values packet from the Slave.
*
* Param:
*  - sensor: SensorInfoStruct containing the info of the sensor to read.
*
* Return:
*  Status of the transfer. There are 3 statuses
*  - TRANSFER_CMPLT: transfer completed successfully and is valid.
*  - SLAVE_NOT_READY: transfert completed, but data is invalid.
*  - TRANSFER_ERROR: the error occurred while transfer or.
*******************************************************************************/
uint32 readSensor(const SensorInfoStruct* sensor)
{
    uint32 status = TRANSFER_ERROR;
    
    uint32 sizeToRead = sensor->nbTaxels*2 + 8; //(4 READY_BYTE + 4 TIME_BYTE)
    
    (void) I2CM_I2CMasterClearStatus();
    
    if(I2CM_I2C_MSTR_NO_ERROR ==  I2CM_I2CMasterReadBuf(sensor->i2cAddr,
                                    sensorValueBuffer, sizeToRead,
                                    I2CM_I2C_MODE_COMPLETE_XFER))
    {
        /* If I2C read started without errors, 
        / wait until master complete read transfer */
        while (0u == (I2CM_I2CMasterStatus() & I2CM_I2C_MSTAT_RD_CMPLT))
        {
            /* Wait */
        }
        
        /* Display transfer status */
        if (0u == (I2CM_I2C_MSTAT_ERR_XFER & I2CM_I2CMasterStatus()))
        {
            /* Check packet structure */
            uint32 dataLen =  I2CM_I2CMasterGetReadBufSize();
            if (dataLen == sizeToRead && sensorValueBuffer[0] == 0x01)
            {
                status = TRANSFER_CMPLT;
            }
            else
            {
                status = SLAVE_NOT_READY;
            }
        }
    }
    return (status);     
}

/*******************************************************************************
* void initSensorsStructs()
*
* Initialize sensor list with defaults values: I2C_address, nbTaxels, isOnline
* and wasRead.
*
*******************************************************************************/
void initSensorsStructs()
{
    //init sensorList structs
    for(uint8 i=0; i<NUMBER_OF_SENSORS; ++i)
    {
        sensorList[i].i2cAddr = sensorAddrList[i];
        sensorList[i].nbTaxels = nbTaxelList[i];
        sensorList[i].isOnline = true;
        sensorList[i].wasRead = false;
    }
}

/*******************************************************************************
* void resetSensorsReadStatus()
*
* Reset reading status values of sensors after a read iteration. 
* wasRead = false and nbReadTry=0
*
*******************************************************************************/
void resetSensorsReadStatus()
{
    for(int i=0; i<NUMBER_OF_SENSORS; ++i)
    {
        sensorList[i].wasRead = false;
        sensorList[i].nbReadTry = 0;
    }
}

/*******************************************************************************
* uint32 sendDataToUART(const SensorInfoStruct* sensor)
*
* Send the content of sensorValueBuffer + the sensor address to the UART.
*
* Param:
*  - sensor: SensorInfoStruct containing the info of the sensor.
*******************************************************************************/
void sendDataToUART(const SensorInfoStruct* sensor)
{
    memset(uartBuffer, 0, UART_BUFFER_SIZE);
    //Insert the sensor id in the first byte of the message
    uartBuffer[0] = sensor->i2cAddr;
    
    memcpy(uartBuffer + SENSOR_TAG_SIZE, sensorValueBuffer + TIME_DATA_SIZE, sensor->nbTaxels*2 + TIME_DATA_SIZE);
    comm_putmsg((uint8*)uartBuffer, SENSOR_TAG_SIZE + TIME_DATA_SIZE + sensor->nbTaxels*2);
}

/*******************************************************************************
* void readSensorsValues()
*
* This function tries to read all sensors from the system. It iterate over all
* sensors in the list and try to read its values. 
*
* When a sensor values is read, if it was sucessful, we send the data immediately 
* to the UART and set this sensor to wasRead=True. If the data could not be read,
* we increment this sensor nbReadTry++, then if nbReadTry<=10, we assume this sensor
* is not online anymore (isOnline=false).
*
* readSensorsVAlues() exits when all sensors are either wasRead=true or isOnline=false.
* When exiting, we reset the values of wasRead and nbReadTry of all sensors.
*
*******************************************************************************/
void readSensorsValues()
{
    bool done = false;
    
    //Loop until all sensors have been read or have been declared offline
    while(!done)
    {
        done = true;
        for(int index=0; index<NUMBER_OF_SENSORS; ++index)
        { 
            //Iterate over all sensors that are online and that were not read yet.
            if(sensorList[index].isOnline==true && sensorList[index].wasRead==false)
            {
                done = false;
                memset(sensorValueBuffer, 0, SENSOR_BUFFER_SIZE);
                
                //Try to read sensor
                uint32 result = readSensor(&sensorList[index]);
                if(result == TRANSFER_CMPLT)
                {                   
                    sendDataToUART(&sensorList[index]);
                    sensorList[index].wasRead = true;
                }
                else// if(result == SLAVE_NOT_READY)//can't read sensor, increment number of try. If over 10, remove sensor from list.
                //Added a little patch for now (never put sensor offline)
                {
                    sensorList[index].nbReadTry += 1;
                    if(sensorList[index].nbReadTry >= 5)
                    {
                        sensorList[index].wasRead = true;
                    } 
                }
               // else
                //{
                //    sensorList[index].isOnline = false;
                //}
            }
        }
    }
    
    resetSensorsReadStatus();
}

int main(void)
{
    CyGlobalIntEnable;
    comm_init();

     /* Start the I2C Master */
    I2CM_Start();
    
    initSensorsStructs();
    
    for(;;)
    {    
        readSensorsValues();
        
        // Delay (ms)
        CyDelay(10u);
    }
}

/* [] END OF FILE */
