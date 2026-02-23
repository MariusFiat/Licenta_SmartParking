import sys
import os

from flask import Flask, request, jsonify
from flask_cors import CORS

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from Database_Scripts.supabase_base_functions import *

app = Flask(__name__)
CORS(app) #Allow requests from external apps (like mobile app)

@app.route('/')
def hello_world():
    return 'Hello World'

@app.route('/reserve', methods=['POST'])
def handle_reservation_request(): #This request came from the mobile app
    data = request.json
    user_id = data.get('user_id')
    car_plate = data.get('car_plate')

    userDetails = get_user_details(user_id)
    subscription_type = get_user_subscription_type(userDetails)

    #Request a parking_slot from db 
    if subscription_type == 'EMPLOYEE':
        assigned_slot = get_parking_slot('EMPLOYEE')
    else:
        assigned_slot = get_parking_slot()

    if assigned_slot != -1: #If exists at least one parking slot of requested type, than, the reservation can be done
        if make_reservation(user_id, car_plate, assigned_slot) == True:
            return jsonify({"status": "success", "slot": assigned_slot}), 200
        else:
            return jsonify({"status": "error", "message": "Error at reservation request  handling. FlaskSERVER!"})
    else:
        return jsonify({"status": "error", "message": "Parking full"}), 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port = 5000)