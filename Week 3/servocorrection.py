import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import re

PORT = 'COM3'
BAUD = 9600
MAX_POINTS = 50  # Number of data points to show on screen at once

try:
    ser = serial.Serial(PORT, BAUD, timeout=0.1)
except Exception as e:
    print(f"Could not open serial port {PORT}: {e}")
    exit()

x_data = []
y_data = []
counter = 0

fig, ax = plt.subplots(figsize=(8, 4))
line, = ax.plot([], [], 'b-', lw=2, label="Potentiometer Value")

ax.set_xlim(0, MAX_POINTS)
ax.set_ylim(0, 1025)  # Potentiometer ranges from 0 to 1023
ax.set_title("Real-Time Arduino Potentiometer Monitor\n(Press 'q' on this window to reset & exit)")
ax.set_xlabel("Sample Index")
ax.set_ylabel("Value (0-1023)")
ax.grid(True)
ax.legend(loc="upper right")


def init():
    line.set_data([], [])
    return line,


def update(frame):
    global counter
    if ser.in_waiting > 0:
        try:
            line_str = ser.readline().decode('utf-8').strip()
            match = re.search(r'Potentiometer:\s*(\d+)', line_str)
            if match:
                pot_val = int(match.group(1))

                x_data.append(counter)
                y_data.append(pot_val)
                counter += 1

                # Keep window rolling by slicing the data lists
                if len(x_data) > MAX_POINTS:
                    x_data.pop(0)
                    y_data.pop(0)
                    ax.set_xlim(x_data[0], x_data[-1])
                else:
                    ax.set_xlim(0, MAX_POINTS)

                line.set_data(x_data, y_data)
        except Exception as err:
            print(f"Parse error: {err}")

    return line,


def on_key(event):
    if event.key in ['q', 'Q']:
        print("\n[INFO] 'q' pressed. Sending reset command to Arduino...")
        try:
            ser.write(b'q')
        except Exception as e:
            print(f"Error writing to serial: {e}")
        ser.close()
        plt.close()


fig.canvas.mpl_connect('key_press_event', on_key)

ani = animation.FuncAnimation(
    fig, update, init_func=init, interval=50, blit=True, cache_frame_data=False
)

plt.show()

if ser.is_open:
    ser.close()
    print("[INFO] Serial port closed.")[]