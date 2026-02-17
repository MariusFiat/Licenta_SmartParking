import time
from queue import Queue
from threading import Thread, Lock
import cv2
import sys
import os
import serial

from CameraWrapper import CameraWrapper
from yolo4 import plateRecognition
from checkplates import check_license_plate
from queue import Queue

def log(msg):
    print(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] {msg}", flush=True)

serialQueue = Queue(maxsize = 2)
port_virtual = '/dev/ttyACM0' # This is the pico serial port.
ser = serial.Serial(port_virtual, 115200, timeout = 1)
cap = CameraWrapper()

def serial_handler():
    while True:
        line_raw = ser.readline()
        line = line_raw.decode('utf-8')

        if not line:
            continue
        
        print(f"Command received : {line}")
        print(f"Line[0] : {line[0]}")
        
        if serialQueue.full() == False:
            if len(line) == 2:
                serialQueue.put(line[0])
                serialQueue.put(line[1])
            else:
                serialQueue.put(line[0])
            
        #Consume the commands
        while serialQueue.empty() == False:
            command = serialQueue.get()
            print(f"Command under execution: {command}")
            detectSide(command)

        time.sleep(0.01)


def detectSide(side):
    if side == 'A':
        print("[System] Entry request (A)")
        plateNumber = plateRecognition(cap, "left")
        result = check_license_plate(plateNumber)
                
        ser.write("ENA000\n".encode('utf-8'))
        ser.flush()
        print("Sent catre Pico: ENA000")
    else:
        print("[System] Exit request (B)")
        plateNumber = plateRecognition(cap, "right")
        result = check_license_plate(plateNumber)
                
        ser.write("EXA000\n".encode('utf-8'))
        ser.flush()
        print("Sent to Pico: EXA000")


def main():
    print("\nStarting parking recognition service...")

    serial_handler()
    
main()