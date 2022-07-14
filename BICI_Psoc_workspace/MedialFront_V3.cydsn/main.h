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

#define TAXEL_COUNT         (28)
#define I2C_SLAVE_ADDRESS1  (0x18u)
#define I2C_SLAVE_ADDRESS2  (I2C_SLAVE_ADDRESS1+(0x40u))
#define DATA_READY          (0x01)
#define DATA_NOT_READY      (0x00)

typedef struct
{
    uint8 dataReady;
    uint32 counterTimer;
    uint16 sensorsList[TAXEL_COUNT];    
} SensorStruct;

SensorStruct sensorStruct;
uint8 activeAddress = 0xFF;

/* [] END OF FILE */