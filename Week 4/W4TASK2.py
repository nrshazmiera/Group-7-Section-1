import time
import matplotlib.pyplot as plt
import serial

SERIAL_PORT = "COM3"  # Change to match your system
BAUD_RATE = 9600
AUTHORIZED_UID = "0003855091"


def analyze_circular_motion(ser, duration=4):
  """Collects MPU6050 data for a set duration, plots it live,

  and verifies if a circular motion pattern was performed.
  """
  print(
      f"Perform circular motion now! Recording for {duration}"
      " seconds..."
  )

  x_vals, y_vals = [], []
  plt.ion()
  fig, ax = plt.subplots()
  ax.set_xlim(-20000, 20000)
  ax.set_ylim(-20000, 20000)
  ax.set_title("Live MPU6050 Motion Tracking")
  (line,) = ax.plot([], [], "ro-", alpha=0.6)

  start_time = time.time()
  motion_detected = False

  try:
    while time.time() - start_time < duration:
      line_data = ser.readline().decode("utf-8", errors="ignore").strip()
      if line_data:
        values = line_data.split(",")
        if len(values) >= 6:
          try:
            ax_val = int(values[0])
            ay_val = int(values[1])

            x_vals.append(ax_val)
            y_vals.append(ay_val)

            line.set_data(x_vals, y_vals)
            plt.draw()
            plt.pause(0.01)
          except ValueError:
            continue
  except KeyboardInterrupt:
    pass

  plt.close(fig)
  plt.ioff()

  if len(x_vals) > 15:
    x_range = max(x_vals) - min(x_vals)
    y_range = max(y_vals) - min(y_vals)
    if x_range > 3000 and y_range > 3000:
      motion_detected = True

  return motion_detected


def main():
  try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    time.sleep(2)
  except Exception as e:
    print(f"Error opening serial port {SERIAL_PORT}: {e}")
    return

  print("--- RFID Access Control System Ready ---")

  try:
    while True:
      uid = input("Scan RFID tag (or type UID): ").strip()

      if not uid:
        continue

      if uid == AUTHORIZED_UID:
        print("RFID Authorized! Starting motion verification...")

        ser.reset_input_buffer()

        if analyze_circular_motion(ser):
          print("Motion Verified! Access Granted.")
          ser.write(b"A")
        else:
          print("Motion Invalid! Access Denied.")
          ser.write(b"D")
      else:
        print(f"UID {uid} Unauthorized. Access Denied.")
        ser.write(b"D")

      print("-" * 40)

  except KeyboardInterrupt:
    print("\nProgram terminated by user.")
  finally:
    if ser.is_open:
      ser.close()


if __name__ == "__main__":
  main()