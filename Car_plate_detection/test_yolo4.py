import time
from queue import Queue
from threading import Thread, Lock
import cv2
import sys
import os

from yolo4 import plateRecognition
from checkplates import check_license_plate

cap = cv2.VideoCapture(0)
myPlate = plateRecognition(cap, "left")
print(check_license_plate(myPlate))
