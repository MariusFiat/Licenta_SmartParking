# generate_csv_data.py
import csv
import random
from datetime import datetime, timedelta
import pytz

def generate_dummy_data(days_back=14, total_slots=10):
    filename = 'parking_history_data.csv'
    
    # We use a pattern: busy mid-day, random variation, busy weekdays
    header = ['parking_id', 'recorded_at', 'occupied_slots', 'total_slots']
    data = []
    
    timezone = pytz.timezone('Europe/Bucharest')
    now = datetime.now(timezone)
    start_date = now - timedelta(days=days_back)
    
    current_time = start_date.replace(hour=7, minute=0, second=0, microsecond=0) # Start from 07:00
    
    while current_time < now:
        if current_time.hour > 22: # End at 22:00
            current_time = (current_time + timedelta(days=1)).replace(hour=7)
            continue

        weekday = current_time.weekday() # 0 is Monday
        hour = current_time.hour
        
        # Determine base occupancy probability based on hour
        if hour >= 11 and hour <= 14: # Busy lunch
            base_prob = 0.8
        elif hour >= 8 and hour <= 18: # Work hours
            base_prob = 0.6
        else: # Off-peak
            base_prob = 0.3
            
        # Slightly reduce probability on weekends
        if weekday >= 5: 
            base_prob *= 0.5
            
        occupied_slots = 0
        for _ in range(total_slots):
            if random.random() < base_prob:
                occupied_slots += 1
                
        # Insert variation: sometimes a lot fewer slots, rarely completely full
        # based on an extra random factor
        if random.random() < 0.1:
            occupied_slots = max(0, occupied_slots - random.randint(3, 6))
        
        formatted_time = current_time.isoformat()
        data.append([1, formatted_time, occupied_slots, total_slots])
        
        # Advance by 1 hour
        current_time += timedelta(hours=1)
        
    with open(filename, mode='w', newline='', encoding='utf-8') as file:
        writer = csv.writer(file)
        writer.writerow(header)
        writer.writerows(data)
        
    print(f"File '{filename}' generated with {len(data)} dummy records.")

if __name__ == "__main__":
    generate_dummy_data()