import cv2
import time
import serial
import re
import numpy as np
import matplotlib.pyplot as plt

ser = serial.Serial('COM11', 9600, timeout=0.1)  # 1/timeout is the frequency at which the port is read

force_values1 = [0]  # Initialize with zero
force_values2 = [1000]  # Initialize with zero
time_points = [0]     # Initialize with zero

plt.ion()  # Turn on interactive mode for live plotting

fig, ax = plt.subplots()

ax.set_xlabel('Time')
ax.set_ylabel('Force')

line1, = ax.plot(time_points, force_values1, label='Force 1')
line2, = ax.plot(time_points, force_values2, label='Force 2')
ax.legend()

# Path to the image
path = "C:\\Users\\vijes\\OneDrive\\Desktop\\greeeen.jpg"

# Read the image
frame = cv2.imread(path)

frame = cv2.resize(frame, (400, 800))



# Resize the window to display the image larger


count = 0
value = 0
delimiters = [",", ":"]
print(frame.shape)
update_flag = False

b = 0
g = 0
r = 0

if frame is not None:
    print("Started...")
else:
    print("Error...")


def display_force(blue1, green1, red1, blue2, green2, red2):
    # Loop through each pixel
    for y in range(200):
        for x in range(200):
            frame[y, x] = [blue1, green1, red1]  # Setting RGB values
            frame[y:y+1, x + 100:x + 200] = [blue2, green2, red2]
            frame[y+100, x+100] = [0, 255, 0]
            #frame[y, x+200] = [255, 0, 255]
            #frame[y, x+300] = [255, 255, 0]

            frame[y+100, x] = [0, 255, 0]
            frame[y+200, x ] = [0, 255, 0]
            frame[y + 200, x+100] = [0, 255, 0]
            frame[y + 300, x] = [0, 255, 0]
            frame[y + 300, x+100] = [0, 255, 0]

    cv2.imshow("Modified Image", frame)
    cv2.waitKey(100)

while True:
    rxdata = ser.readline().decode().strip() #serially read data from arduino
    if rxdata:
        flag = rxdata.startswith("Force")
        if flag == True:
            print(rxdata)
            splitData = re.split('[: , ]', rxdata)
            f1 = int(splitData[1])
            f2 = int(splitData[2])
            update_flag = True
            ypoints = np.array([3, 8, 1, 10, 5, 7])
            time_points.append(time_points[-1] + 1)  # Increment time by 1 for each data point
            force_values1.append(f1)
            force_values2.append(f2)

            line1.set_xdata(time_points)
            line1.set_ydata(force_values1)
            line2.set_xdata(time_points)
            line2.set_ydata(force_values2)

            ax.relim()
            ax.autoscale_view()

            fig.canvas.draw()
            fig.canvas.flush_events()

    if update_flag == True:
        update_flag = False

        if 250 <= f1 <= 500:
            f1 = f1 - 250
            b1 = 0
            g1 = f1
            r1 = 0
        elif 500 <= f1 <= 750:
            f1 = f1 - 500
            b1 = f1
            g1 = 0
            r1 = 0

        elif f1 > 750:
            f1 = f1 - 750
            b1 = 0
            g1 = 0
            r1 = f1

        else:
            b1 = 0
            g1 = 255
            r1 = 0

        if 250 <= f2 <= 500:
            f2 = f2 - 250
            b2 = 0
            g2 = f2
            r2 = 0
        elif 500 <= f2 <= 750:
            f2 = f2 - 500
            b2 = f2
            g2 = 0
            r2 = 0

        elif f2 > 750:
            f2 = f2 - 750
            b2 = 0
            g2 = 0
            r2 = f2

        else:
            b2 = 0
            g2 = 255
            r2 = 0

            # Display the modified image
        display_force(b1, g1, r1, b2, g2, r2)
        time.sleep(1)
