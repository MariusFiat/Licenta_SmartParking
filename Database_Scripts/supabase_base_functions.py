import os
import psycopg2
from dotenv import load_dotenv

#Load the .env variables
load_dotenv()

DB_URL = os.getenv("DB_URL")

def check_plate_into_db(plate):
    conn = psycopg2.connect(DB_URL)
    cur = conn.cursor()

    query = "SELECT * FROM cars_table WHERE car_plate = %s;"
    cur.execute(query, (plate,))

    result = cur.fetchone()

    cur.close()
    conn.close()

    if result:
        print(f"The car with the plate number: {plate} was found in the db.")
        return check_plate_status_and_subscription_type(result) #Check the plate to see if this car plate is booked or it's just a simple customer that wants to park
    else:
        print(f"Unknown car")
        #Add the car to the parking db and set the NO_SUBSCRIPTION status
        #At the exit, this plate will have to pay the tax
        return insert_new_car(plate, 0, 1, 'STATUS_PARKED', 'NO_SUBSCRIPTION', 0)

def check_plate_status_and_subscription_type(result):
    if result:
        plate = result[1]
        owner_id = result[2]
        slot = result[3]
        status = result[4]
        subscription_type = result[5]

    if status == 'STATUS_PARKED':
        return False

    #Check if this is an employee subscription plate
    if subscription_type == 'EMPLOYEE':
        #No taxes for this car and there is nothing to do, let the car to enter
        return update_car_status(plate, 'STATUS_PARKED')


def insert_new_car(car_plate, id_owner, parking_slot, status, subscription_type, parking_tax):
    conn = None
    try:
        conn = psycopg2.connect(DB_URL)
        cur = conn.cursor()

        # SQL Query
        insert_query = """
        INSERT INTO cars_table (car_plate, id_owner, parking_slot, status, subscription_type, parking_tax)
        VALUES (%s, %s, %s, %s, %s, %s);
        """

        record_to_insert = (car_plate, id_owner, parking_slot, status, subscription_type, parking_tax)

        cur.execute(insert_query, record_to_insert)

        # Save
        conn.commit()
        
        print(f"The car with the plate {car_plate} was succesfully added.")

    except Exception as e:
        print(f"Error at the inserting new car stage: {e}")
    finally:
        if conn:
            cur.close()
            conn.close()
    return True

def update_car_status(car_plate, new_status):
    conn = None
    try:
        conn = psycopg2.connect(DB_URL)
        cur = conn.cursor()

        #SQL Query
        update_query = """
        UPDATE cars_table 
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
            result = cur.fetchone()
            if result[4] == 'STATUS_PARKED':
                print(f"The car with the plate: {car_plate} is already in the parking.")
            else:
                print(f"The status of the car: {car_plate} was updated in: {new_status}")

    except Exception as e:
        print(f"Eroare la update: {e}")
    finally:
        if conn:
            cur.close()
            conn.close()
    return True

def create_cars_table(conn, cur):
    # SQL query for table creation
        create_table_query = """
        CREATE TABLE IF NOT EXISTS cars_table (
            id SERIAL PRIMARY KEY,
            car_plate TEXT NOT NULL,
            id_owner INTEGER,
            parking_slot INTEGER,
            status TEXT NOT NULL,
            subscription_type TEXT NOT NULL,
            parking_tax INTEGER,
            created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW()
        );
        """

        cur.execute(create_table_query)
        
        # Save
        conn.commit()
        
        print("The table 'cars_table' was created succesfully!")

def create_database_tables():
    try:
        # DB connection
        conn = psycopg2.connect(DB_URL)
        cur = conn.cursor()

        create_cars_table(conn, cur)

    except Exception as e:
        print(f"Error at table creation: {e}")
    finally:
        if conn:
            cur.close()
            conn.close()

if __name__ == "__main__":
    create_database_tables()