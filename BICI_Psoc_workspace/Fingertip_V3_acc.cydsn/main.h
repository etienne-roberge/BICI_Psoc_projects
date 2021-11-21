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

#define TAXEL_COUNT         (14)
#define READY_READ          (0xFFFE)
#define WAITING_FOR_MASTER  (0)
#define SLAVE_STATE_BYTE    (0)
#define WRITE_BUFFER_SIZE   (1)

/* The I2C Slave read and write buffers */
uint16 i2cReadBuffer [TAXEL_COUNT+1];
uint8 i2cWriteBuffer[WRITE_BUFFER_SIZE];

/* [] END OF FILE */
