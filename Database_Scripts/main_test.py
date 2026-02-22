import time
from queue import Queue
from threading import Thread, Lock
import cv2
import sys
import os
import serial

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from Database_Scripts.supabase_base_functions import *

import os
import psycopg2
from dotenv import load_dotenv

#Load the .env variables
load_dotenv()

DB_URL = os.getenv("DB_URL")

def main():
    print(update_parking_slot_status(get_parking_slot('EMPLOYEE'), 'FREE'))

main()