import serial
import time as tm

ser = serial.Serial(
    port="COM26", 
    baudrate=115200, 
    timeout=1.0
    )

num = 0

with open("data_24-12-25-14-00.txt", "a") as file:
    while True:
        line = ser.read(8).decode('ascii')
        if line != "":
            file.write(line)
            print(f"{num}. {line}")
            num += 1
