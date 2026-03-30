import os
import psycopg2
from dotenv import load_dotenv
import csv

# Load the .env variables
load_dotenv()

DB_URL = os.getenv("DB_URL")

# We define a constant for the guest user ID (all zeros UUID)
UNKNOWN_USER_UUID = "00000000-0000-0000-0000-000000000000"

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
    conn.commit()
    print("The table 'parking_details' was created succesfully!")

def create_user_details_table(conn, cur):
    # Note: We removed the direct FK to auth.users from the ID definition 
    # to allow your 'unknown' record with ID 000...000
    create_user_details_query = """
        CREATE TABLE IF NOT EXISTS user_details (
            id UUID PRIMARY KEY,
            first_name TEXT NOT NULL,
            last_name TEXT NOT NULL,
            subscription_type TEXT NOT NULL
        );
    """
    cur.execute(create_user_details_query)
    conn.commit()
    print("The table 'user_details' was created succesfully!")

def create_car_table(conn, cur):
    create_table_query = """
        CREATE TABLE IF NOT EXISTS car (
            id SERIAL PRIMARY KEY,
            car_plate TEXT NOT NULL,
            id_owner UUID REFERENCES user_details(id) ON DELETE CASCADE,
            parking_slot INTEGER,
            created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW()
        );
    """
    cur.execute(create_table_query)
    conn.commit()
    print("The table 'car' was created succesfully!")

def create_reservation_table(conn, cur):
    create_reservation_query = """
        CREATE TABLE IF NOT EXISTS reservation (
            id SERIAL PRIMARY KEY,
            user_id UUID REFERENCES user_details(id) ON DELETE CASCADE,
            car_plate TEXT NOT NULL,
            slot INTEGER REFERENCES slots(id) ON DELETE CASCADE,
            status TEXT NOT NULL,
            tax DOUBLE PRECISION,
            entry_timestamp TIMESTAMP,
            start_timestamp TIMESTAMP NOT NULL,
            number_of_hours INTEGER NOT NULL
        );
    """
    cur.execute(create_reservation_query)
    conn.commit()
    print("The table 'reservation' was created succesfully!")

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
    conn.commit()
    print("The table 'slots' was created succesfully!")

def insert_reserved_unknown_user_id(conn, cur):
    # Using the special zero-UUID instead of the integer 0
    insert_query = """
            INSERT INTO user_details (
                id, first_name, last_name, subscription_type
            )
            VALUES (%s, %s, %s, %s)
            ON CONFLICT (id) DO NOTHING;
    """
    record_to_insert = (UNKNOWN_USER_UUID, 'unknown', 'unknown', 'STANDARD')
    cur.execute(insert_query, record_to_insert)
    conn.commit()

    print("The unknown reserved user was added!")

def setup_auth_trigger(conn, cur):
    trigger_sql = """
    DROP TRIGGER IF EXISTS on_auth_user_created ON auth.users;

    CREATE OR REPLACE FUNCTION public.handle_new_user()
    RETURNS trigger AS $$
    BEGIN
    INSERT INTO public.user_details (id, first_name, last_name, subscription_type)
    VALUES (
        new.id, 
        COALESCE(new.raw_user_meta_data->>'first_name', 'NoFirstName'), 
        COALESCE(new.raw_user_meta_data->>'last_name', 'NoLastName'), 
        'STANDARD'
    );
    RETURN new;
    END;
    $$ LANGUAGE plpgsql SECURITY DEFINER;

    CREATE TRIGGER on_auth_user_created
    AFTER INSERT ON auth.users
    FOR EACH ROW EXECUTE FUNCTION public.handle_new_user();
    """
    cur.execute(trigger_sql)
    conn.commit()
    print("The Auth trigger was created succesfully!")

def insert_default_available_slots(conn, cur):
    cur.execute("TRUNCATE TABLE slots RESTART IDENTITY CASCADE;") # Clear existing slots and reset ID sequence
    conn.commit()
    
    insert_query = """
        INSERT INTO slots (
            status, slot_type, parking_id
        )
        VALUES (%s, %s, %s);
    """
    for i in range(1, 6):
        if i <= 3:
            record_to_insert = ('FREE', 'EMPLOYEE', 1)
        else:
            record_to_insert = ('FREE', 'STANDARD', 1)
        cur.execute(insert_query, record_to_insert)
        conn.commit()
    print("The slots were added to the parking!")

def insert_initial_parking_details(conn, cur):
    insert_query = """
        INSERT INTO parking_details (id, pricing, number_of_slots, slots)
        VALUES (%s, %s, %s, %s)
        ON CONFLICT (id) DO NOTHING;
    """
    record_to_insert = (1, 3.0, 10, 10)
    
    cur.execute(insert_query, record_to_insert)
    conn.commit()
    print("Initial parking details (Price: 3.0) added successfully!")
    
def create_the_make_reservation_function(conn, cur):
    function_sql = """CREATE OR REPLACE FUNCTION make_future_reservation(
        p_user_id UUID,
        p_car_plate TEXT,
        p_start_time TIMESTAMP,
        p_hours INT
    ) RETURNS INT AS $$
    DECLARE
        v_sub_type TEXT;
        v_slot_id INT;
        v_reservation_id INT;
        v_pricing FLOAT8;
        v_tax FLOAT8;
        v_end_time TIMESTAMP;
    BEGIN
        -- 0. Compute the end time of the reservation based on the start time and number of hours
        v_end_time := p_start_time + (p_hours || ' hours')::interval;

        IF EXISTS (
            SELECT 1
            FROM reservation
            WHERE car_plate = p_car_plate
            -- Verify time overlap (similar to how we do it for slots)
            AND start_timestamp < v_end_time
            AND (start_timestamp + (number_of_hours || ' hours')::interval) > p_start_time
        ) THEN
            -- If we get here, we stop everything and throw an error!
            RAISE EXCEPTION 'Car % already has a reservation in this time interval!', p_car_plate;
        END IF;

        -- 1. Verify if the user exists and get their subscription type
        SELECT subscription_type INTO v_sub_type
        FROM user_details
        WHERE id = p_user_id;

        IF v_sub_type IS NULL THEN
            RAISE EXCEPTION 'User or subscription not found.';
        END IF;

        -- 2. Search for an available slot that does not have overlaps AND is FREE
        SELECT id INTO v_slot_id
        FROM slots s
        WHERE s.slot_type = v_sub_type 
        AND s.status = 'FREE' 
        AND NOT EXISTS (
            SELECT 1
            FROM reservation r
            WHERE r.slot = s.id
                AND r.start_timestamp < v_end_time
                AND (r.start_timestamp + (r.number_of_hours || ' hours')::interval) > p_start_time
        )
        LIMIT 1
        FOR UPDATE SKIP LOCKED;

        IF v_slot_id IS NULL THEN
            RAISE EXCEPTION 'There are no available slots for this time interval.';
        END IF;

        -- 3. Extract the pricing from the parking details (we assume there's only one record in parking_details)
        SELECT pricing INTO v_pricing
        FROM parking_details
        LIMIT 1;

        IF v_pricing IS NULL THEN
            v_pricing := 0.0;
        END IF;

        -- 4. Calculate the total tax
        v_tax := v_pricing * p_hours;

        -- 5. Insert the reservation
        INSERT INTO reservation (user_id, car_plate, slot, status, tax, start_timestamp, number_of_hours)
        VALUES (p_user_id, p_car_plate, v_slot_id, 'STATUS_BOOKED', v_tax, p_start_time, p_hours)
        RETURNING id INTO v_reservation_id;

        RETURN v_reservation_id;
    END;
    $$ LANGUAGE plpgsql;"""
    
    cur.execute(function_sql)
    conn.commit()
    print("The 'make_reservation' function was created succesfully!")

def create_the_update_future_reservation_function(conn, cur):
    function_sql= """
        CREATE OR REPLACE FUNCTION update_future_reservation(
        p_reservation_id INT,
        p_user_id UUID,
        p_new_start TIMESTAMP,
        p_new_hours INT
    ) RETURNS BOOLEAN AS $$
    DECLARE
        v_sub_type TEXT;
        v_car_plate TEXT;
        v_new_slot_id INT;
        v_new_end TIMESTAMP;
        v_pricing FLOAT8;
    BEGIN
        -- 0. Compute the new end time of the reservation based on the new start time and new number of hours
        v_new_end := p_new_start + (p_new_hours || ' hours')::interval;

        -- 1. Get the car plate and subscription type for the reservation we want to update
        SELECT car_plate INTO v_car_plate FROM reservation WHERE id = p_reservation_id AND user_id = p_user_id;
        SELECT subscription_type INTO v_sub_type FROM user_details WHERE id = p_user_id;

        -- 2. Verify if the car has another reservation that overlaps with the new time interval (excluding the current reservation)
        IF EXISTS (
            SELECT 1 FROM reservation 
            WHERE car_plate = v_car_plate AND id != p_reservation_id
            AND start_timestamp < v_new_end 
            AND (start_timestamp + (number_of_hours || ' hours')::interval) > p_new_start
        ) THEN
            RAISE EXCEPTION 'The car %s already has another reservation that overlaps with the new time interval!', v_car_plate;
        END IF;

        -- 3. Search for an available slot that does not have overlaps (excluding the current reservation)
        SELECT id INTO v_new_slot_id
        FROM slots s
        WHERE s.slot_type = v_sub_type and s.status = 'FREE'
        AND NOT EXISTS (
            SELECT 1 FROM reservation r
            WHERE r.slot = s.id AND r.id != p_reservation_id
                AND r.start_timestamp < v_new_end
                AND (r.start_timestamp + (r.number_of_hours || ' hours')::interval) > p_new_start
        )
        LIMIT 1
        FOR UPDATE SKIP LOCKED;

        IF v_new_slot_id IS NULL THEN
            RAISE EXCEPTION 'There are no available slots for the new time interval!';
        END IF;

        -- 4. Recalculăm taxa
        SELECT pricing INTO v_pricing FROM parking_details LIMIT 1;
        
        -- 5. Update rezervare
        UPDATE reservation
        SET start_timestamp = p_new_start,
            number_of_hours = p_new_hours,
            slot = v_new_slot_id,
            tax = p_new_hours * COALESCE(v_pricing, 0)
        WHERE id = p_reservation_id;

        RETURN TRUE;
    END;
    $$ LANGUAGE plpgsql;
    """
    cur.execute(function_sql)
    conn.commit()
    print("The 'update_future_reservation' function was created succesfully!")
    
    
#MOCK data generation and insertion for parking_history
def create_parking_history_table(conn, cur):
    create_table_query = """
        CREATE TABLE IF NOT EXISTS parking_history (
            id SERIAL PRIMARY KEY,
            parking_id INTEGER REFERENCES parking_details(id) ON DELETE CASCADE,
            recorded_at TIMESTAMP NOT NULL DEFAULT NOW(),
            occupied_slots INTEGER NOT NULL,
            total_slots INTEGER NOT NULL
        );
    """
    cur.execute(create_table_query)
    conn.commit()
    print("The table 'parking_history' was created succesfully!")
    
def insert_parking_history_from_csv(conn, cur):
    cur.execute("DELETE FROM parking_history;")
    conn.commit()

    filename = 'parking_history_data.csv'
    insert_query = """
        INSERT INTO parking_history (
            parking_id, recorded_at, occupied_slots, total_slots
        )
        VALUES (%s, %s, %s, %s);
    """
    
    try:
        with open(filename, mode='r', encoding='utf-8') as file:
            reader = csv.reader(file)
            header = next(reader)
            
            for row in reader:
                cur.execute(insert_query, row)
                
            conn.commit()
            print(f"Historical data from '{filename}' inserted into 'parking_history' table!")
    except FileNotFoundError:
        print(f"File '{filename}' not found. Run generate_csv_data.py first.")
    except Exception as e:
        print(f"Error during historical data insertion: {e}")

def create_database_tables():
    try:
        conn = psycopg2.connect(DB_URL)
        cur = conn.cursor()

        create_parking_details_table(conn, cur) # Need this first for slots
        create_user_details_table(conn, cur)
        create_car_table(conn, cur)
        create_reservation_table(conn, cur)
        create_slots_table(conn, cur)
        
        insert_initial_parking_details(conn, cur)
        setup_auth_trigger(conn, cur)
        insert_reserved_unknown_user_id(conn, cur)
        insert_default_available_slots(conn, cur)
        create_the_make_reservation_function(conn, cur)
        create_the_update_future_reservation_function(conn, cur)
        create_parking_history_table(conn, cur)
        insert_parking_history_from_csv(conn, cur)

    except Exception as e:
        print(f"Error at table creation: {e}")
    finally:
        if 'conn' in locals() and conn:
            cur.close()
            conn.close()

if __name__ == "__main__":
    create_database_tables()