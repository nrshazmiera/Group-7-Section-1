import serial

# Updated to use COM9 as requested
ser = serial.Serial('COM3', 9600)

try:
    while True:
        line = ser.readline().decode().strip()
        if line.isdigit():
            pot_value = int(line)
            print("Potentiometer Value:", pot_value)

            # Check if value exceeds half of its max range (1023 / 2 = ~511)
            if pot_value > 511:
                ser.write(b'H')  # Send 'H' to turn LED ON
            else:
                ser.write(b'L')  # Send 'L' to turn LED OFF

except KeyboardInterrupt:
    ser.close()
    print("Serial connection closed.")