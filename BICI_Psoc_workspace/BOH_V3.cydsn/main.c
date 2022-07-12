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
#include <main.h>

void copyDataToI2CBuffer()
{                  
    for(unsigned int i=0; i<11; ++i)
    {     
        sensorStruct.sensorsList[i]     = CapSense_dsRam.snsList.row0[i].raw[0]; 
        sensorStruct.sensorsList[i+12]  = CapSense_dsRam.snsList.row1[i].raw[0];
        sensorStruct.sensorsList[i+24]  = CapSense_dsRam.snsList.row2[i].raw[0];
        sensorStruct.sensorsList[i+36]  = CapSense_dsRam.snsList.row3[i].raw[0];
        sensorStruct.sensorsList[i+48]  = CapSense_dsRam.snsList.row4[i].raw[0];
        sensorStruct.sensorsList[i+60]  = CapSense_dsRam.snsList.row5[i].raw[0];
        sensorStruct.sensorsList[i+72]  = CapSense_dsRam.snsList.row6[i].raw[0];
        sensorStruct.sensorsList[i+84]  = CapSense_dsRam.snsList.row7[i].raw[0];
        sensorStruct.sensorsList[i+96]  = CapSense_dsRam.snsList.row8[i].raw[0];
    }
    for(unsigned int i=0; i<6; ++i) // Row9
    {
        sensorStruct.sensorsList[i+108] = CapSense_dsRam.snsList.row9[i].raw[0]; 
    }
    for(unsigned int i=0; i<4; ++i) // Row10
    {
        sensorStruct.sensorsList[i+114] = CapSense_dsRam.snsList.row10[i].raw[0]; 
    }
}

uint32 AddressAccepted(void)
{
    /* Read 7-bits right justified slave address */
    activeAddress = I2C_GET_I2C_7BIT_ADDRESS(I2C_RX_FIFO_RD_REG);
   
    switch(activeAddress)
    {
        case (I2C_SLAVE_ADDRESS1):
            /* Address 1: Setup buffers for read and write */
            if(sensorStruct.dataReady == DATA_READY)
            {
                copyDataToI2CBuffer();
            }
            I2C_I2CSlaveInitReadBuf ((uint8 *)&sensorStruct, sizeof(sensorStruct));
        break;
        case (I2C_SLAVE_ADDRESS2):
            /* Address 2: Setup buffers for read and write */
            I2C_I2CSlaveInitReadBuf ((uint8_t *)&CapSense_dsRam, sizeof(CapSense_dsRam));
            I2C_I2CSlaveInitWriteBuf((uint8_t *)&CapSense_dsRam, sizeof(CapSense_dsRam));
        break;
    }
    return I2C_I2C_ACK_ADDR;
}


int main(void)
{    
    sensorStruct.dataReady = DATA_NOT_READY;
    sensorStruct.counterTimer = 0x0000;
    
    I2C_I2CSlaveSetAddress(I2C_SLAVE_ADDRESS1);
    I2C_SetI2cAddressCustomInterruptHandler(&AddressAccepted);
    I2C_Start();
    
    Timer_Start();
    
    CyGlobalIntEnable;
    __enable_irq(); /* Enable global interrupts. */
    
    CapSense_Start();
    CapSense_ScanAllWidgets();
    
    for(;;)
    {
        /* Read complete*/
        if (0u != (I2C_I2CSlaveStatus() & I2C_I2C_SSTAT_RD_CMPLT))
        {
            /* Clear the slave read buffer and status */
            I2C_I2CSlaveClearReadBuf();
            I2C_I2CSlaveClearReadStatus();
            if(activeAddress == I2C_SLAVE_ADDRESS1)
            {
                sensorStruct.dataReady = DATA_NOT_READY;
            }
        }
        
        /* Write complete*/
        if (0u != (I2C_I2CSlaveStatus() & I2C_I2C_SSTAT_WR_CMPLT))
        {
            /* Clean-up status and buffer pointer */
            I2C_I2CSlaveClearWriteBuf();
            I2C_I2CSlaveClearWriteStatus();
        }
        
        /* Do this only when a scan is done */
        if(CapSense_NOT_BUSY == CapSense_IsBusy())
        {
            /* Process all widgets */
            CapSense_ProcessAllWidgets();
            sensorStruct.dataReady = DATA_READY;
            sensorStruct.counterTimer += Timer_ReadCounter();
            Timer_WriteCounter(0);
            
            /* To sync with Tuner application */
            CapSense_RunTuner();
            
            /* Start next scan */
            CapSense_ScanAllWidgets();
        }
        
    }
    
}

/* [] END OF FILE */
