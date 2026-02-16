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

def log(msg):
    print(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] {msg}", flush=True)

def main():
    port_virtual = '/dev/ttyACM0' # This is the pico serial port.
    ser = serial.Serial(port_virtual, 115200, timeout = 1)

    print("\nStarting parking recognition service...")
    cap = CameraWrapper()
    

    
    try:    
        
        while True:
            line_raw = ser.readline()
                
            line = line_raw.decode('utf-8').strip()
            
            if not line:
                continue

            print(f"Comanda primita: {line}")

            if line == "A":
                print("[System] Cerere Intrare (A)")
                plateNumber = plateRecognition(cap, "left")
                result = check_license_plate(plateNumber)
                
                ser.write("ENA000\n".encode('utf-8'))
                ser.flush()
                print("Trimis catre Pico: ENA000")

            elif line == "B":
                print("[System] Cerere Iesire (B)")
                plateNumber = plateRecognition(cap, "right")
                result = check_license_plate(plateNumber)
                
                ser.write("EXA000\n".encode('utf-8'))
                ser.flush()
                print("Trimis catre Pico: EXA000")
            
            time.sleep(0.01)
                
    except KeyboardInterrupt:
        print("\nScript stopped by user.")
    
main()