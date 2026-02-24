import os
import psycopg2
from dotenv import load_dotenv

# Load the .env variables
load_dotenv()

DB_URL = os.getenv("DB_URL")

# IMPORTANT: We use the same zero-UUID as in the table creation script
UNKNOWN_USER_UUID = "00000000-0000-0000-0000-000000000000"

def check_plate_in_the_reservation_table(plate):
    conn = psycopg2.connect(DB_URL)
    cur = conn.cursor()

    # Check if there are a reservation for this car_plate.
    query = "SELECT * FROM reservation WHERE car_plate = %s and status != 'STATUS_CLOSED';"
    cur.execute(query, (plate,))

    result = cur.fetchone()

    cur.close()
    conn.close()

    if result and result[4] != 'STATUS_CLOSED':
        # This means that the car has reservation and now I have to check if is an emplyee or a standard customer
        print(f"The car with the plate number: {plate} was found in the db.")
        checks = check_plate_status_and_subscription_type(result) # Check the plate to see if this car plate is booked for an employee or it's just a simple customer that wants to park
        if checks == True:
            return result[3] # Return the assigned slot.
        else:
            return -1 # Access denied (maybe the car is already inside and the plate is used again?)
    else:
        print(f"Unknown car")
        # Add the car to the parking db and set the NO_SUBSCRIPTION status
        # At the exit, this plate will have to pay the tax
        # id_owner = UNKNOWN_USER_UUID is the default user_id for unknown car_plates!!!!!
        
        # Modified: Passed UNKNOWN_USER_UUID instead of 0
        assigned_slot = insert_new_car(plate, UNKNOWN_USER_UUID, 'STATUS_PARKED', 0) 
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

    if status == 'STATUS_PARKED':
        print("This reservation is already used!")
        return False

    # If this car has a reservation, just update the reservation status and set the entry time
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

        parking_slot = get_parking_slot('STANDARD') # The system must be assign a standard parking slot if there is at least one not assigned

        if parking_slot >= 1: # This means that exists at least one more empty slot
            # Update the status for this parking_slot
            update_parking_slot_status(parking_slot, 'ASSIGNED')

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

        # SQL Query
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

    # Select user_details by id
    query = "SELECT * FROM user_details WHERE id = %s;"
    cur.execute(query, (user_details_id,))

    result = cur.fetchone()
 
    cur.close()
    conn.close()
    
    return result

def get_user_subscription_type(userDetails):
    return userDetails[3]

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


# Exit logic:
def check_exit_status(plate):
    conn = psycopg2.connect(DB_URL)
    cur = conn.cursor()

    # Get the reservation for this plate
    query = "SELECT * FROM reservation WHERE car_plate = %s and status = 'STATUS_PARKED';"
    cur.execute(query, (plate,))
    result = cur.fetchone()

    if result and result[4] == 'STATUS_PARKED':
        owner_id = result[1]

        # Modified: check if owner_id is not the unknown UUID instead of > 0
        if owner_id and str(owner_id) != UNKNOWN_USER_UUID:
            # This means that the owner of this car has account
            owner_details = get_user_details(owner_id)
            if owner_details[3] == 'EMPLOYEE':
                # No taxes to pay
                return update_car_status(plate, 'STATUS_CLOSED')
            elif owner_details[3] == 'STANDARD':
                if result[5] == 0:
                    # No more taxes
                    # Release the parking slot
                    update_parking_slot_status(result[3], 'FREE')

                    return update_car_status(plate, 'STATUS_CLOSED')
                else: 
                    print(f"The car with car plate {plate} has taxes unpaid!")
                    return False
        else:
            # Unknown customer, he can pay his taxes via mobile app without account, just with the car plate nmber
            if result[5] == 0:
                # Release the parking slot
                update_parking_slot_status(result[3], 'FREE')
                return update_car_status(plate, 'STATUS_CLOSED')
        
    cur.close()
    conn.close()

def get_parking_slot(slot_type = 'STANDARD'): # slot_type can be 'STANDARD' or 'EMPLOYEE'
    conn = psycopg2.connect(DB_URL)
    cur = conn.cursor()

    query = "SELECT * FROM slots WHERE slot_type = %s and status = 'FREE';"
    cur.execute(query, (slot_type,))

    result = cur.fetchone()
 
    cur.close()
    conn.close()
    
    if result:
        return result[0]
    else: 
        return -1

def update_parking_slot_status(slot_number, new_status):
    try:
        conn = psycopg2.connect(DB_URL)
        cur = conn.cursor()
        
        update_query = """
            UPDATE slots SET status = %s
            WHERE id = %s;
        """

        cur.execute(update_query, (new_status, slot_number))

        conn.commit()
        print(f"The status for slot with number : {slot_number} was updated at status: {new_status}")
    except Exception as e:
        print("Error at parking_slot status update!")
    finally:
        if conn:
            cur.close()
            conn.close()
    return True


def release_the_parking_slot(slot_number):
    # To be implemented
    return 1

def get_reservations(conn, cur):
    query = "SELECT * FROM reservation"
    cur.execute(query)

    return cur.fetchall()

def get_parking_pricing(conn, cur):
    query = "SELECT pricing FROM parking_details"
    cur.execute(query)
    return cur.fetchone()

def set_reservation_tax(conn, cur, id, tax):
    query = """
        UPDATE reservation SET tax = %s WHERE id = %s and status = 'STATUS_PARKED';
    """
    cur.execute(query, (tax, id))
    conn.commit()

def calculate_the_taxes():
    conn = psycopg2.connect(DB_URL)
    cur = conn.cursor()

    # Get all reservations
    reservations = get_reservations(conn, cur)
    parking_pricing = get_parking_pricing(conn, cur)

    # For each check the subcription_type
    for res in reservations:
        owner = get_user_details(res[1])
        # Note: owner[3] is the subscription_type
        if owner and owner[3] == 'STANDARD':
            set_reservation_tax(conn, cur, res[0], res[5] + parking_pricing[0])
    
    cur.close()
    conn.close()

# Method that creates a reservation started by a mobile request
def make_reservation(user_id, car_plate, slot): # I HAVE TO ADD A CUSTOM START_TIMESTAMP
    conn = None
    try:
        conn = psycopg2.connect(DB_URL)
        cur = conn.cursor()

        query = """
            INSERT INTO reservation (
                car_plate, user_id, slot, status, tax, entry_timestamp, start_timestamp, number_of_hours
            )
            VALUES(%s, %s, %s, %s, %s, NOW(), NOW(), %s)
        """

        record_to_insert = (car_plate, user_id, slot, 'STATUS_BOOKED', 0, 1)
        cur.execute(query, record_to_insert)
        conn.commit()

    except Exception as e:
        print("Error at reservation request handling! Method: 'make_reservation'")
    finally:
        if conn:
            cur.close()
            conn.close()
    return True