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

int main(void)
{    
    CyGlobalIntEnable;
    __enable_irq(); /* Enable global interrupts. */
    
    /* Start the I2C Slave */
    I2C_I2CSlaveInitReadBuf ((uint8 *)i2cReadBuffer, sizeof(i2cReadBuffer));
    I2C_I2CSlaveInitWriteBuf(i2cWriteBuffer, WRITE_BUFFER_SIZE);
    I2C_Start();
    
    //Start capsenses
    CapSense_Start();
    CapSense_ScanAllWidgets();
    
    uint8 readCapSenseFlag = 0;

    for(;;)
    {
        /* Write complete: parse the command packet */
        if (0u != (I2C_I2CSlaveStatus() & I2C_I2C_SSTAT_WR_CMPLT))
        {
            /* Check the packet length */
            if (WRITE_BUFFER_SIZE == I2C_I2CSlaveGetWriteBufSize())
            {
                /* Check the start and end of packet markers */
                if(i2cWriteBuffer[0] == 1)
                {   
                    i2cReadBuffer[SLAVE_STATE_BYTE] = WAITING_FOR_MASTER;
                    readCapSenseFlag = 1;
                }
                i2cWriteBuffer[0] = 0 ;
            }
            
            /* Clear the slave write buffer and status */
            I2C_I2CSlaveClearWriteBuf();
            (void) I2C_I2CSlaveClearWriteStatus();       
        }
        
        //REceived call to read the capsense
        if(readCapSenseFlag == 1)
        {
            if(!CapSense_IsBusy())
            {
                CapSense_ProcessAllWidgets();
                CapSense_RunTuner();
                CapSense_ScanAllWidgets(); 
            
                //for(unsigned int i=0; i<TAXEL_COUNT; ++i)
                //{
                //    i2cReadBuffer[i+1] = CapSense_dsRam.snsList.button0[i].raw[0];
                //}
                
                for(unsigned int i=0; i<4; ++i) // Row0
                {
                    i2cReadBuffer[i+1] = CapSense_dsRam.snsList.row0[i].raw[0];
                }
                for(unsigned int i=0; i<5; ++i) // Row1-3
                {
                    i2cReadBuffer[i+5] = CapSense_dsRam.snsList.row1[i].raw[0];
                    i2cReadBuffer[i+10] = CapSense_dsRam.snsList.row2[i].raw[0];
                    i2cReadBuffer[i+15] = CapSense_dsRam.snsList.row3[i].raw[0];
                }
                for(unsigned int i=0; i<7; ++i) // Row4-5
                {
                    i2cReadBuffer[i+20] = CapSense_dsRam.snsList.row4[i].raw[0];
                    i2cReadBuffer[i+27] = CapSense_dsRam.snsList.row5[i].raw[0];
                }
                for(unsigned int i=0; i<4; ++i) // Row6-7
                {
                    i2cReadBuffer[i+34] = CapSense_dsRam.snsList.row6[i].raw[0];
                    i2cReadBuffer[i+38] = CapSense_dsRam.snsList.row7[i].raw[0];
                }
                for(unsigned int i=0; i<2; ++i) // Row8-10
                {
                    i2cReadBuffer[i+42] = CapSense_dsRam.snsList.row8[i].raw[0];
                    i2cReadBuffer[i+44] = CapSense_dsRam.snsList.row9[i].raw[0];
                    i2cReadBuffer[i+46] = CapSense_dsRam.snsList.row10[i].raw[0];
                }
                
                i2cReadBuffer[SLAVE_STATE_BYTE] = READY_READ;
                readCapSenseFlag = WAITING_FOR_MASTER;
            }
        }
        
        
        /* Read complete*/
        if (0u != (I2C_I2CSlaveStatus() & I2C_I2C_SSTAT_RD_CMPLT))
        {
            /* Clear the slave read buffer and status */
            I2C_I2CSlaveClearReadBuf();
            (void) I2C_I2CSlaveClearReadStatus();
        }
        
    }
}

/* [] END OF FILE */
