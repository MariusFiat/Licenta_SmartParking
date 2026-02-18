import os
import psycopg2
from dotenv import load_dotenv

#Load the .env variables
load_dotenv()

DB_URL = os.getenv("DB_URL")

def check_plate_in_the_reservation_table(plate):
    conn = psycopg2.connect(DB_URL)
    cur = conn.cursor()

    #Check if there are a reservation for this car_plate.
    query = "SELECT * FROM reservation WHERE car_plate = %s;"
    cur.execute(query, (plate,))

    result = cur.fetchone()

    cur.close()
    conn.close()

    if result:
        #This means that the car has reservation and now I have to check if is an emplyee or a standard customer
        print(f"The car with the plate number: {plate} was found in the db.")
        checks = check_plate_status_and_subscription_type(result) #Check the plate to see if this car plate is booked for an employee or it's just a simple customer that wants to park
        if checks == True:
            return result[3] #Return the assigned slot.
        else:
            return -1 #Access denied (maybe the car is already inside and the plate is used again?)
    else:
        print(f"Unknown car")
        #Add the car to the parking db and set the NO_SUBSCRIPTION status
        #At the exit, this plate will have to pay the tax
        #id_owner = 0 is the default user_id for unknown car_plates!!!!!

        assigned_slot =  insert_new_car(plate, 0, 'STATUS_PARKED', 0) #user_id -1 means that is an unknown user. 
        return assigned_slot
    

def check_plate_status_and_subscription_type(result):
    if result:
        reservation_id = result[0]
        user_id = result[1]
        car_plate = result[2]
        slot = result[3]
        status = result[4]
        tax = result[5]
        entry_time = result[6]

    if status == 'STATUS_PARKED' or status == 'STATUS_CLOSED':
        print("This reservation is already used!")
        return False

    #If this car has a reservation, just update the reservation status and set the entry time

    return update_car_status(car_plate, 'STATUS_PARKED') and set_the_entry_time(car_plate)


def insert_new_car(car_plate, id_owner, status, parking_tax):
    conn = None
    try:
        conn = psycopg2.connect(DB_URL)
        cur = conn.cursor()

        # SQL Query
        insert_query = """INSERT INTO reservation (
                car_plate, user_id, slot, status, tax, 
                entry_timestamp, start_timestamp, number_of_hours
            )
            VALUES (%s, %s, %s, %s, %s, NOW(), NOW(), %s);
        """

        parking_slot = detect_a_standard_parking_slot() #The system must be assign a standard parking slot if there is at least one not assigned

        if parking_slot >= 1: #This means that exists at least one more empty slot
            record_to_insert = (car_plate, id_owner, parking_slot, status, parking_tax, 1)

            cur.execute(insert_query, record_to_insert)
            # Save
            conn.commit()
            
            print(f"The car with the plate {car_plate} was succesfully added.")
        else:
            print(f"The car with plate {car_plate} can not be accepted! No more standard parking slots!")

    except Exception as e:
        print(f"Error at the inserting new car stage: {e}")
    finally:
        if conn:
            cur.close()
            conn.close()
    return parking_slot

def update_car_status(car_plate, new_status):
    conn = None
    try:
        conn = psycopg2.connect(DB_URL)
        cur = conn.cursor()

        #SQL Query
        update_query = """
        UPDATE reservation 
        SET status = %s 
        WHERE car_plate = %s;
        """
        
        cur.execute(update_query, (new_status, car_plate))

        # Verify if the car exists in the db
        if cur.rowcount == 0:
            print(f"This car plate: {car_plate} wasn't found.")
            return False
        else:
            conn.commit()
            print(f"The status of the car: {car_plate} was updated in: {new_status}")

    except Exception as e:
        print(f"Eroare la update: {e}")
    finally:
        if conn:
            cur.close()
            conn.close()
    return True

def get_user_details(user_details_id):
    conn = psycopg2.connect(DB_URL)
    cur = conn.cursor()

    #Select user_details by id
    query = "SELECT * FROM user_details WHERE id = %s;"
    cur.execute(query, (user_details_id,))

    result = cur.fetchone()
 
    cur.close()
    conn.close()
    
    return result

def set_the_entry_time(car_plate):
    conn = None
    try:
        conn = psycopg2.connect(DB_URL)
        cur = conn.cursor()

        update_query = """
        UPDATE reservation SET entry_timestamp = NOW()
        WHERE car_plate = %s;
        """

        cur.execute(update_query, (car_plate,))

        conn.commit()

        print(f"The entry_timestamp for plate: {car_plate} was set!")
    
    except Exception as e:
        print(f"Error at entry_timestamp update!")
    finally:
        if conn:
            cur.close()
            conn.close()
    return True


def detect_a_standard_parking_slot():
    #To be implemented:
    return 1

def detect_an_employee_parking_slot():
    #To be implemented:
    return 2