import serial
import time

# Înlocuiește cu path-ul PTY1 primit de la socat
port_virtual = '/dev/ttys001' 

ser = serial.Serial(port_virtual, 9600, timeout=1)

print(f"Simulez Raspberry Pi pe {port_virtual}...")

try:
    while True:
        mesaj = "Senzor_Temperatura: 22.5 C\n"
        ser.write(mesaj.encode('utf-8'))
        print(f"Trimis: {mesaj.strip()}")
        time.sleep(2)
except KeyboardInterrupt:
    ser.close()