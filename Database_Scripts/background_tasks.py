import time
import schedule
import os
import sys

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from Database_Scripts.supabase_base_functions import calculate_the_taxes

NUMBER_OF_MINUTES = 1


def calculate_the_taxes_scheduled_task():
    #Enable the scheduler
    schedule.every(NUMBER_OF_MINUTES).minutes.do(calculate_the_taxes)

    #Run in a loop this task
    while True:
        schedule.run_pending()
        time.sleep(10)