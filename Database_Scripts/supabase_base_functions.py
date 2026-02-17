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
        return True
    else:
        print(f"Unknown car")
        return False
    

    

def create_cars_table(conn, cur):
    # SQL query for table creation
        create_table_query = """
        CREATE TABLE IF NOT EXISTS cars_table (
            id SERIAL PRIMARY KEY,
            car_plate TEXT NOT NULL,
            id_owner INTEGER NOT NULL,
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