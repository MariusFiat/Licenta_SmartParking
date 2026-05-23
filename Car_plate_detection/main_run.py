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
from threading import *

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from Database_Scripts.supabase_base_functions import check_plate_in_the_reservation_table
from Database_Scripts.supabase_base_functions import check_exit_status
from Database_Scripts.background_tasks import calculate_the_parking_occupancy_rate_scheduled_task, calculate_the_taxes_scheduled_task

def log(msg):
    print(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] {msg}", flush=True)

serialQueue = Queue(maxsize = 2)
port_virtual = '/dev/ttyACM0' # This is the pico serial port.
ser = serial.Serial(port_virtual, 115200, timeout = 1)
cap = CameraWrapper()

def send_init_sequence():
    command = "INIT00\n"
                
    ser.write(command.encode('utf-8'))
    ser.flush()
    print("[System RP5]: The init command was send to the PICO!\n")

def serial_handler():
    while True:
        line_raw = ser.readline()
        line = line_raw.decode('utf-8')

        if not line:
            continue
        
        print(f"[System RP5]: Command received : {line}")
        if "[RP2040]" in line:
                continue

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
            print(f"[System RP5]: Command under execution: {command}")
            detectSide(command)

        time.sleep(0.01)


def detectSide(side):
    if side == 'A':
        print("[System RP5]: Entry request (A)")
        plateNumber = plateRecognition(cap, "left")
        result = check_license_plate(plateNumber)
        
        if result == True:
            slot = check_plate_in_the_reservation_table(plateNumber)
            if slot != -1:
                #Create the response message with the parking_slot 
                command = f"ENA{slot:03d}\n"
                
                # Send the command
                ser.write(command.encode('utf-8'))
                ser.flush()
    
                print(f"[System RP5]: Sent to pico: {command.strip()}")
            else:
                ser.write("END000\n".encode('utf-8'))
                ser.flush()
                print("[System RP5]: Sent to Pico: END000. Permission denied!")
        else:
            ser.write("END000\n".encode('utf-8'))
            ser.flush()
            print("[System RP5]: Sent to Pico: END000. Invalid number")
    else:
        print("[System RP5]: Exit request (B)")
        plateNumber = plateRecognition(cap, "right")
        result = check_license_plate(plateNumber)

        if result == True:
            status = check_exit_status(plateNumber)

            if status == True:    
                ser.write("EXA000\n".encode('utf-8'))
                ser.flush()
                print("[System RP5]: Sent to Pico: EXA000. ")
            else:
                ser.write("EXD000\n".encode('utf-8'))
                ser.flush()
                print("[System RP5]: Sent to Pico: EXD000")
        else:
            #Incorrect detection
            ser.write("EXD000\n".encode('utf-8'))
            ser.flush()
            print("[System RP5]: Sent to Pico: EXD000. Invalid number")


def main():
    print("\n[System RP5]: Starting parking recognition service...")

    #Update tax field daemon thread
    Tax_Thread = Thread(target = calculate_the_taxes_scheduled_task, daemon = True) #This task is a main helper, it must die when the main thread stops.
    Tax_Thread.start()
    
    #Occupancy rate update thread
    Occupancy_Thread = Thread(target = calculate_the_parking_occupancy_rate_scheduled_task, daemon = True)
    Occupancy_Thread.start()

    send_init_sequence()

    #Main thread will enter in a loop and will execute the main detection app stage
    serial_handler()
    
main()