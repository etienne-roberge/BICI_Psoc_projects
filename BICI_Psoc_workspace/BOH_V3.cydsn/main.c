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
                
                for(unsigned int i=0; i<12; ++i) // Rows0-8
                {
                    i2cReadBuffer[i+1] = CapSense_dsRam.snsList.row0[i].raw[0]; 
                    i2cReadBuffer[i+13] = CapSense_dsRam.snsList.row1[i].raw[0];
                    i2cReadBuffer[i+25] = CapSense_dsRam.snsList.row2[i].raw[0];
                    i2cReadBuffer[i+37] = CapSense_dsRam.snsList.row3[i].raw[0];
                    i2cReadBuffer[i+49] = CapSense_dsRam.snsList.row4[i].raw[0];
                    i2cReadBuffer[i+61] = CapSense_dsRam.snsList.row5[i].raw[0];
                    i2cReadBuffer[i+73] = CapSense_dsRam.snsList.row6[i].raw[0];
                    i2cReadBuffer[i+85] = CapSense_dsRam.snsList.row7[i].raw[0];
                    i2cReadBuffer[i+97] = CapSense_dsRam.snsList.row8[i].raw[0];
                }
                for(unsigned int i=0; i<6; ++i) // Row9
                {
                    i2cReadBuffer[i+109] = CapSense_dsRam.snsList.row9[i].raw[0]; 
                }
                for(unsigned int i=0; i<4; ++i) // Row10
                {
                    i2cReadBuffer[i+115] = CapSense_dsRam.snsList.row10[i].raw[0]; 
                }
                for(unsigned int i=0; i<1; ++i) // Proxi
                {
                    i2cReadBuffer[i+119] = CapSense_dsRam.snsList.proxi[i].raw[0]; 
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
