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


float X_out, Y_out, Z_out;
uint32 initAccel()
{
    uint32 status = TRANSFER_ERROR;
    
    (void) I2CM_I2CMasterClearStatus();
    
    uint32 aaa = I2CM_I2CMasterSendStart(ACC_ADDRESS,I2CM_I2C_WRITE_XFER_MODE, 0);
   
    if(I2CM_I2C_MSTR_NO_ERROR == aaa)
    {
         aaa = I2CM_I2CMasterWriteByte(POWER_ALT,0);
         if(I2CM_I2C_MSTR_NO_ERROR == aaa)
         {
            aaa = I2CM_I2CMasterWriteByte(8,0);
            if(I2CM_I2C_MSTR_NO_ERROR == aaa)
            {
              status = TRANSFER_CMPLT;
            }
         }
    }
    I2CM_I2CMasterSendStop(0);
    return (status);     
}

uint32 readAccel()
{
    uint8  buffer[1];
    uint32 status = TRANSFER_ERROR;

    buffer[0] = 0x32;
    
    (void) I2CM_I2CMasterClearStatus();
    
    uint32 aaa=I2CM_I2CMasterWriteBuf(ACC_ADDRESS, buffer, 1, I2CM_I2C_MODE_COMPLETE_XFER);
   
    if(I2CM_I2C_MSTR_NO_ERROR == aaa)
    {
        
        while (0u == (I2CM_I2CMasterStatus() & I2CM_I2C_MSTAT_WR_CMPLT))
        {
            /* Wait */
        }
        
            uint8 sensorValueBuffer[6];
    
            aaa=I2CM_I2CMasterReadBuf(ACC_ADDRESS, sensorValueBuffer, sizeof(sensorValueBuffer), I2CM_I2C_MODE_COMPLETE_XFER);
            
            if(I2CM_I2C_MSTR_NO_ERROR ==  aaa)
            {
                /* If I2C read started without errors, 
                / wait until master complete read transfer */
                while (0u == (I2CM_I2CMasterStatus() & I2CM_I2C_MSTAT_RD_CMPLT))
                {
                    /* Wait */
                }
                X_out = ( sensorValueBuffer[0] | sensorValueBuffer[1] << 8); // X-axis value
                //X_out = X_out/265.0;
                Y_out = ( sensorValueBuffer[2] | sensorValueBuffer[3] << 8); // Y-axis value
                //Y_out = Y_out/265.0;
                Z_out = ( sensorValueBuffer[4] | sensorValueBuffer[5] << 8); // Z-axis value
                //Z_out = Z_out/256.0;
                I2CM_I2CMasterSendStop(0);
                status = TRANSFER_CMPLT;
            }
       
         
    }
    return (status);     
}

int main(void)
{    
    CyGlobalIntEnable;
    __enable_irq(); /* Enable global interrupts. */
    
    /* Start the I2C Slave */
    I2C_I2CSlaveInitReadBuf ((uint8 *)i2cReadBuffer, sizeof(i2cReadBuffer));
    I2C_I2CSlaveInitWriteBuf(i2cWriteBuffer, WRITE_BUFFER_SIZE);
    I2C_Start();
    
    /* Start the I2C Master */
    I2CM_Start();
    
    //Start capsenses
    CapSense_Start();
    CapSense_ScanAllWidgets();
    
    uint8 readCapSenseFlag = 0;
    while(TRANSFER_CMPLT != initAccel())
    {
        CyDelay(500u);
    }
    CyDelay(10u);


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
                
                for(unsigned int i=0; i<9; ++i) // Rows0-1
                {
                    i2cReadBuffer[i+1] = CapSense_dsRam.snsList.row0[i].raw[0]; 
                    i2cReadBuffer[i+10] = CapSense_dsRam.snsList.row1[i].raw[0]; 
                }
                for(unsigned int i=0; i<7; ++i) // Row2
                {
                    i2cReadBuffer[i+19] = CapSense_dsRam.snsList.row2[i].raw[0]; 
                }
                for(unsigned int i=0; i<5; ++i) // Row3
                {
                    i2cReadBuffer[i+26] = CapSense_dsRam.snsList.row3[i].raw[0]; 
                }
                for(unsigned int i=0; i<4; ++i) // Rows4-12
                {
                    i2cReadBuffer[i+31] = CapSense_dsRam.snsList.row4[i].raw[0]; 
                    i2cReadBuffer[i+35] = CapSense_dsRam.snsList.row5[i].raw[0]; 
                    i2cReadBuffer[i+39] = CapSense_dsRam.snsList.row6[i].raw[0]; 
                    i2cReadBuffer[i+43] = CapSense_dsRam.snsList.row7[i].raw[0]; 
                    i2cReadBuffer[i+47] = CapSense_dsRam.snsList.row8[i].raw[0]; 
                    i2cReadBuffer[i+51] = CapSense_dsRam.snsList.row9[i].raw[0]; 
                    i2cReadBuffer[i+55] = CapSense_dsRam.snsList.row10[i].raw[0]; 
                    i2cReadBuffer[i+59] = CapSense_dsRam.snsList.row11[i].raw[0]; 
                    i2cReadBuffer[i+63] = CapSense_dsRam.snsList.row12[i].raw[0]; 
                }
                for(unsigned int i=0; i<1; ++i) // Proxi
                {
                    i2cReadBuffer[i+67] = CapSense_dsRam.snsList.proxi[i].raw[0]; 
                } 
                
                if(TRANSFER_CMPLT == readAccel()){
                    
                    i2cReadBuffer[68] = X_out;
                    i2cReadBuffer[69] = Y_out;
                    i2cReadBuffer[70] = Z_out;
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
