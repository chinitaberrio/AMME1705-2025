import sys

import struct
import time
import threading 

from threading import Thread
from queue import Queue
import time
import glob

from serial import Serial, SerialException
from serial.threaded import ReaderThread, Protocol, LineReader

from enum import Enum

import matplotlib.pyplot as plt
from matplotlib.widgets import Button, TextBox 

import numpy as np

import matplotlib
matplotlib.use('Qt5Agg')

from matplotlib.backends import qt_compat

import matplotlib.pyplot as plt
import matplotlib.animation as animation

from matplotlib.lines import Line2D

from collections import deque


import argparse

def init_argparse() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        usage="%(prog)s [OPTION]",
        description="Select the serial port and baud rate."
    )
    parser.add_argument(
        "-p", "--port", type=str, 
        help="""Serial port to connect to (in Windows, this is COM<number>)
              default is to autodetect the port""", 
        default=""
    )
    parser.add_argument(
        "-b", "--baud", type=int, 
        help="Baud rate for the serial connection", 
        default=1000000
    )
    #parser.add_argument('files', nargs='*')
    return parser



class Scope:
    def __init__(self, ax, maxt=4, dt=0.01):
        self.btn_1_location = []
        self.btn_2_location = []
        self.write_file_button = []
        self.pause_button = []

        self.ax = ax
        self.dt = dt
        self.maxt = maxt

        self.ani = None

        self.new_tdata = [0]
        self.new_ydata = [0]

        self.new_samples = deque(maxlen=1000)

        self.tdata = [0]
        self.ydata = [0]
        self.line = Line2D(self.tdata, self.ydata, color='blue')

        self.lines = []
        self.lines.append(self.line)

        self.ax.add_line(self.line)

        self.ax.set_ylim(-0.1, 5.1)
        self.ax.set_xlim(0, self.maxt)
        self.measurements = deque(maxlen=1000)
        self.times = deque(maxlen=1000)

        self.text_update_counter = 10

        # these are matplotlib.patch.Patch properties
        self.props = dict(boxstyle='round', facecolor='wheat', alpha=0.5)
        #self.textstr = "text_string"
        # place a text box in upper left in axes coords
        #self.text_box_ref = self.ax.text(0.05, 0.95, "text here", transform=self.ax.transAxes, fontsize=14,
        #             verticalalignment='top', bbox=self.props)
        
        #self.text_box_ref.set_text("0V")

        self.axLabel = plt.axes([0.15, 0.92, 0.21, 0.075])
        self.text_boxLabel = TextBox(self.axLabel, '', "0V")
        self.running = True

        self.exiting = False
        

    def pauseOscilloscope(self, event):
        #print("set to pause/resume")
        self.running = not self.running
        self.pause_button.label.set_text("Pause" if self.running else "Resume")
        self.ax.figure.canvas.draw()

    def writeToFile(self, event):

        self.running = False
        self.ani.pause()

        file, _ = qt_compat._getSaveFileName(fig.canvas.parent(), 
                                             caption = "Save data to a CSV file", 
                                             filter ='*.csv')
        print("writing to file: " + file)

        # write the two deques to a csv file
        with open(file, 'w') as f:
            for i in range(len(self.times)):
                f.write(str(self.times[i])  
                        + ',' + str(self.measurements[i]) + '\n')

        self.ani.resume()

    def update(self, y_array):

        if (self.running == False):
            return self.line,

        for y in y_array:

            scaled_measurement = y[1]

            self.textstr = str(round(scaled_measurement,2)) + " V "
            #self.ax.text(0.05, 0.95, self.textstr, transform=self.ax.transAxes, fontsize=14,
            #            verticalalignment='top', bbox=self.props)

            self.text_update_counter -= 1
            if self.text_update_counter == 0:
                self.text_boxLabel.set_val(self.textstr)
                self.text_update_counter = 10

            self.measurements.append(scaled_measurement)
            self.times.append(y[0])

            lastt = self.tdata[-1]

            self.tdata.append(y[0])
            self.ydata.append(scaled_measurement)

            if y[0] < lastt or lastt <= self.tdata[0] - self.tdata[0] % self.maxt or lastt >= self.tdata[0] - self.tdata[0] % self.maxt + self.maxt:  # reset the arrays
                self.tdata = [self.tdata[-1]]
                self.ydata = [self.ydata[-1]]
                self.ax.set_xlim(self.tdata[-1]-0.01  - self.tdata[-1] % self.maxt, self.tdata[-1] - self.tdata[-1] % self.maxt + self.maxt + 0.01)
                self.ax.figure.canvas.draw()

        self.line.set_data(self.tdata, self.ydata)
        return self.line,


    def emitter(self, p=0.1):
        valid_float_array = []

        # check if there are any new samples
        while len(self.new_samples) > 0:

            if not self.running:
                break

            input_line = self.new_samples.pop()
            valid_floats = []

            if (len(input_line) != 2):
                print('Parser: invalid input count ' + str(len(input_line)))
                continue

            # check that each element in the array is a valid float
            for element in input_line:
                try:
                    # if valid, add the element to the list of floats
                    float(element)
                    valid_floats.append(float(element))
                except ValueError:
                    print('Invalid input line (value)')
                    continue

            # if we have a valid float array, add it to the list of valid float arrays
            valid_float_array.append(valid_floats)

        yield(valid_float_array)



def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = Serial(port)
            s.close()
            result.append(port)
        except (OSError, SerialException):
            pass
    return result



# Create a new thread to read from the serial port
parser = init_argparse()
args = parser.parse_args()

for port in serial_ports():
    print("found port: " + port)

if (args.port == ""):
    args.port = serial_ports()[-1]
    print("Trying autodetected port: " + args.port + ", if this \r\ndoesn't work you can try the others using the command line argument:\r\npython pyscope.py -p COM<number here>\r\n")

# Create a new graph 
fig, ax = plt.subplots()
plt.grid()
plt.rcParams['font.size'] = 22
fig.subplots_adjust(bottom=0.2)
scope = Scope(ax)

fig.set_figwidth(12)
fig.set_figheight(8)

def SerialReadThread(scope):
    
    print("Starting read from serial port " + args.port + " at " + str(args.baud) + " baud")

    try:
        ser = Serial(args.port, args.baud, timeout=1)
    except SerialException as e:
        print("Error opening serial port: " + str(e))
        exit(1)


    while not scope.exiting:
        # wait for serial data
        input_line = ser.readline()

        # check that input_line is a valid string
        if not input_line:
            print('Serial data: invalid line')
            continue

        # input_line should be a csv string, so convert to an array of floats
        input_line = input_line.decode('utf-8').strip()
        input_line = input_line.split(',')

        valid_floats = []

        if (len(input_line) != 2):
            print('Serial data: invalid number of elements in row ' + str(len(input_line)))
            continue

        # check that each element in the array is a valid float
        for element in input_line:
            try:
                float(element)
                # add the element to the list of floats
                valid_floats.append(float(element))
            except ValueError:
                print('Serial data: invalid value in row ' + str(len(input_line)) + ' (' + element + ')')
                continue

        scope.new_samples.appendleft(valid_floats)

    # when the program exits, close the serial port
    ser.close()


# Create a new thread to read from the serial port
print("Spawning new serial read thread")
serial_thread = Thread(target=SerialReadThread, args=(scope,))
serial_thread.start()

print("Generating the graphs")
scope.btn_1_location = fig.add_axes([0.55, 0.05, 0.2, 0.075])
scope.btn_2_location = fig.add_axes([0.775, 0.05, 0.2, 0.075])

scope.write_file_button = Button(scope.btn_1_location, 'Write to File')
scope.write_file_button.on_clicked(scope.writeToFile)

scope.pause_button = Button(scope.btn_2_location, 'Pause')
scope.pause_button.on_clicked(scope.pauseOscilloscope)


# pass a generator in "emitter" to produce data for the update func
scope.ani = animation.FuncAnimation(fig, scope.update, scope.emitter, interval=20,
                              blit=False, save_count=100)

plt.show()

scope.exiting = True

# Close the serial port when done
serial_thread.join()

print("Exiting")
