import time
from queue import Queue
from threading import Thread, Lock
import cv2
import sys
import os

from yolo4 import plateRecognition
from checkplates import check_license_plate

def my_yolo_model():
    cap = cv2.VideoCapture(0)

    while True:
        data = pipe_serial_detect.readline()
        print(f"Received: {data.strip()}")
        if not data or data.strip() == "exit":
                    # Dacă nu mai sunt date, înseamnă că producătorul a închis pipe-ul
            break

        if data.strip() == "left":
            print("Left turn signal detected")
            plate = plateRecognition(cap, "left")
                    
            time.sleep(3)
                        
            pipe_detect_serial.write(plate + "\n")
            pipe_detect_serial.flush()
        elif data.strip() == "right":
            print("Right turn signal detected")
            plate = plateRecognition(cap, "right")
                    
            time.sleep(3)
                        
            pipe_detect_serial.write(plate + "\n")
            pipe_detect_serial.flush()
        print("Waiting for next command...")
                
my_yolo_model()