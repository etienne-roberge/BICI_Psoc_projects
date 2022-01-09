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


#define TAXEL_COUNT         (70)
#define READY_READ          (0xFFFE)
#define WAITING_FOR_MASTER  (0)
#define SLAVE_STATE_BYTE    (0)
#define WRITE_BUFFER_SIZE   (1)
//Accelerometer parameters
#define ACC_ADDRESS         (0x53) 
//Accelerometer registers
#define POWER_ALT         (0x2D) 

/* The I2C Slave read and write buffers */
uint16 i2cReadBuffer [TAXEL_COUNT+1];
uint8 i2cWriteBuffer[WRITE_BUFFER_SIZE];


#define TRANSFER_CMPLT      (0x00u)
#define SLAVE_NOT_READY     (0x01u)
#define TRANSFER_ERROR      (0xFFu)

uint32 initAccel();
uint32 readAccel();

/* [] END OF FILE */
