import os
import psycopg2
from dotenv import load_dotenv

#Load the .env variables
load_dotenv()

DB_URL = os.getenv("DB_URL")

def create_parking_table():
    try:
        # DB connection
        conn = psycopg2.connect(DB_URL)
        cur = conn.cursor()

        # SQL query for table creation
        create_table_query = """
        CREATE TABLE IF NOT EXISTS istoric_parcare (
            id SERIAL PRIMARY KEY,
            nr_inmatriculare TEXT NOT NULL,
            directie TEXT CHECK (directie IN ('intrare', 'iesire')),
            poarta TEXT,
            creat_la TIMESTAMP WITH TIME ZONE DEFAULT NOW()
        );
        """

        cur.execute(create_table_query)
        
        # Save
        conn.commit()
        
        print("The table 'istoric_parcare' was created succesfully!")

    except Exception as e:
        print(f"Error at table creation: {e}")
    finally:
        if conn:
            cur.close()
            conn.close()

if __name__ == "__main__":
    create_parking_table()