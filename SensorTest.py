import serial
import numpy as np

serialPort = serial.Serial(
    port="COM6", baudrate=115200, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE
)

serialString = ""
while 1:
    if serialPort.in_waiting > 0:
        serialString = serialPort.read_until(b'\n\x01')

        sensorAddress = int(serialString[1])

        #Print all sensors address you are receiving
        #print(sensorAddress)

        if sensorAddress == 23:
            msgLen = int(serialString[0])
            sensorTime = int.from_bytes(serialString[2:6], byteorder='little')
            nbTaxel = int((msgLen - 7) / 2)
            taxelValues = np.zeros(nbTaxel)

            for i in np.arange(nbTaxel):
                taxelValues[i] = int.from_bytes(serialString[(6+(i*2)):(8+(i*2))], byteorder='little')

            print("Sensor address: " + str(sensorAddress))
            print("Sensor time: " + str(sensorTime))
            print("Values: " + str(taxelValues))
            print("-------------------------------------")
