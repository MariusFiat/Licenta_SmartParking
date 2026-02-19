import time
from queue import Queue
from threading import Thread, Lock
import cv2
import sys
import os
import serial

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from Database_Scripts.supabase_base_functions import get_reservations, get_parking_pricing, set_reservation_tax, calculate_the_taxes

import os
import psycopg2
from dotenv import load_dotenv

#Load the .env variables
load_dotenv()

DB_URL = os.getenv("DB_URL")

def main():
    conn = psycopg2.connect(DB_URL)
    cur = conn.cursor()

    
    cur.close()
    conn.close()

    calculate_the_taxes()

main()