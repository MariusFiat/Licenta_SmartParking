import os
import psycopg2
from dotenv import load_dotenv

#Load the .env variables
load_dotenv()

DB_URL = os.getenv("DB_URL")

def create_slots_table(conn, cur):
    create_slots_query = """
        CREATE TABLE IF NOT EXISTS slots (
            id SERIAL PRIMARY KEY,
            status TEXT NOT NULL,
            slot_type TEXT NOT NULL,
            parking_id INTEGER REFERENCES parking_details(id) ON DELETE CASCADE
        );
    """

    cur.execute(create_slots_query)

    #Save
    conn.commit()

    print("The table 'slots' was created succesfully!")

def create_parking_details_table(conn, cur):
    create_parking_details_query = """
        CREATE TABLE IF NOT EXISTS parking_details (
            id SERIAL PRIMARY KEY,
            pricing DOUBLE PRECISION,
            number_of_slots INTEGER,
            slots INTEGER
        );
    """

    cur.execute(create_parking_details_query)

    #Save
    conn.commit()

    print("The table 'parking_details' was created succesfully!")

def create_user_details_table(conn, cur):
    create_user_details_query = """
        CREATE TABLE IF NOT EXISTS user_details (
            id SERIAL PRIMARY KEY,
            first_name TEXT NOT NULL,
            last_name TEXT NOT NULL,
            subscription_type TEXT NOT NULL
        );
    """
    cur.execute(create_user_details_query)

    #Save
    conn.commit()

    print("The table 'user_details' was created succesfully!")

def create_reservation_table(conn, cur):
    create_reservation_query = """
        CREATE TABLE IF NOT EXISTS reservation (
            id SERIAL PRIMARY KEY,
            user_id INTEGER REFERENCES user_details(id) ON DELETE CASCADE,
            car_plate TEXT NOT NULL,
            slot INTEGER NOT NULL,
            status TEXT NOT NULL,
            tax DOUBLE PRECISION,
            entry_timestamp TIMESTAMP,
            start_timestamp TIMESTAMP NOT NULL,
            number_of_hours INTEGER NOT NULL
        );
    """

    cur.execute(create_reservation_query)

    #Save
    conn.commit()

    print("The table 'reservation' was created succesfully!")


def create_car_table(conn, cur):
    # SQL query for table creation
        create_table_query = """
        CREATE TABLE IF NOT EXISTS car (
            id SERIAL PRIMARY KEY,
            car_plate TEXT NOT NULL,
            id_owner INTEGER REFERENCES user_details(id) ON DELETE CASCADE,
            parking_slot INTEGER,
            created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW()
        );
        """

        cur.execute(create_table_query)
        
        # Save
        conn.commit()
        
        print("The table 'car' was created succesfully!")

def insert_reserved_unknown_user_id(conn, cur):
    insert_query = """
            INSERT INTO user_details (
                id, first_name, last_name, subscription_type
            )
            VALUES (%s, %s, %s, %s);
    """
    record_to_insert = (0, 'unknown', 'unknown', 'STANDARD')
    cur.execute(insert_query, record_to_insert)
    conn.commit()

    print("The unknown reserved user was added!")


def insert_default_available_slots(conn, cur):
    insert_query = """
        INSERT INTO slots (
            status, slot_type, parking_id
        )
        VALUES (%s, %s, %s);
    """

    for i in range(1,10):
        if i <= 3:
            record_to_insert = ('FREE', 'EMPLOYEE', 1)
        else:
            record_to_insert = ('FREE', 'STANDARD', 1)
        cur.execute(insert_query, record_to_insert)
        conn.commit()

    print("The slots were added to the parking!")

def create_database_tables():
    try:
        # DB connection
        conn = psycopg2.connect(DB_URL)
        cur = conn.cursor()

        create_user_details_table(conn, cur)
        create_car_table(conn, cur)
        create_reservation_table(conn, cur)
        create_parking_details_table(conn, cur)
        create_slots_table(conn, cur)
        insert_reserved_unknown_user_id(conn, cur)
        insert_default_available_slots(conn, cur)

    except Exception as e:
        print(f"Error at table creation: {e}")
    finally:
        if conn:
            cur.close()
            conn.close()

if __name__ == "__main__":
    create_database_tables()