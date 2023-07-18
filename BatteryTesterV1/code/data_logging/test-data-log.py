import serial
import time
import csv

ser = serial.Serial('/dev/ttyACM0')
ser.reset_input_buffer()

while True:
    try:
        ser_bytes = ser.readline()
        try:
            #decoded_bytes = float(ser_bytes[0:len(ser_bytes)-2].decode("utf-8"))
            named_tuple = time.localtime() # get struct_time
            time_string = time.strftime("%H:%M:%S", named_tuple)
            print(time_string)
        except:
            continue
        with open("cell_1.csv","a") as f:
            writer = csv.writer(f,delimiter=",")
            writer.writerow([time.time(),decoded_bytes])
#        y_var = np.append(y_var,decoded_bytes)
#        y_var = y_var[1:plot_window+1]
#        line.set_ydata(y_var)
#        ax.relim()
#        ax.autoscale_view()
#        fig.canvas.draw()
#        fig.canvas.flush_events()
    except:
        print("Keyboard Interrupt")
        break