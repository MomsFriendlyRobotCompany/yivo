#!/usr/bin/env python3

import serial
import yivo
from pprint import pprint
from slurm import storage, files
import time

# # put in
# import pickle
# import gzip
#
# class PickleZip:
#     def __init__(self, filename, data):
#         pass
#
#     def write(self, filename, data):
#         filename = filename + ".pickle.gz"
#         with gzip.open(filename, 'wb') as f:
#             f.write(pickle.dumps(data, pickle.HIGHEST_PROTOCOL))
#
#     def open(self, filename):
#         pass

import time
import struct
from collections import namedtuple
from collections import deque

# [0xFF,0xFF] # start
ACCEL      = 0xFE # accel
GYRO       = 0xFD # gyro
MAG        = 0xFC # mag
TEMP_PRES  = 0xFB # temperature, pressure
# 0xFA #
LIGHT      = 0xF9 # light
IR_CAMERA  = 0xF8 # MLX90640 IR camera
LIDAR      = 0xF7 # lidar
# 0xF6-0xF3 # unused
QUATERNION = 0xF2 # quaternion
VELOCITY   = 0xF1 # velocity
POSITION   = 0xF0 # position
# [0xEE,0xEE] # end


TempPres = namedtuple("TempPres", "temperature pressure")
Vector2 = namedtuple("Vector2", "x y")
Vector3 = namedtuple("Vector3", "x y z")
Lidar = namedtuple("Lidar","distance")

def read_serial(ser):
    # ff,ff,msglen
    while ser.in_waiting < 3:
        time.sleep(0.0001)
        # print(".", ser.in_waiting)
        # print(".", ser.in_waiting, " -- ", ser.read(100))

    # print("got data")

    while True:
        # get start header [0xff,0xff]
        chr = ser.read(1)
        if chr != b'\xff':
            continue

        chr = ser.read(1)
        if chr != b'\xff':
            continue

        try:
            len = ser.read(1)
            len = ord(len)
        except Exception:
            # time.sleep(0.002)
            continue

        break

    ans = {}
    while len > 5: # [0xff,0xff,len, ...,0xee,0xee]
        # print(f"{len} ----")
        # print(ans,"\n")
        id = ser.read(1)
        id = ord(id)
        if id == ACCEL:
            data = ser.read(12)
            len -= 13
            ans["accel"] = Vector3._make(struct.unpack("<fff", data))
        elif id == GYRO:
            data = ser.read(12)
            len -= 13
            ans["gyro"] = Vector3._make(struct.unpack("<fff", data))
        elif id == MAG:
            data = ser.read(12)
            len -= 13
            ans["mag"] = Vector3._make(struct.unpack("<fff", data))
        elif id == TEMP_PRES:
            data = ser.read(8)
            len -= 9
            ans["temppres"] = TempPres._make(struct.unpack("<ff", data))
        elif id == LIDAR:
            data = ser.read(4)
            len -= 5
            ans["lidar"] = Lidar._make(struct.unpack("<f", data))
        else:
            print("wtf:", id, len)

    # pprint(ans)
    ans["timestamp"] = time.time()
    return ans

def findSerialPort():
    # handle macOS or Linux
    sp = files.find("/dev","tty.usbmodem*")[0].as_posix()
    print(sp)
    return sp

def main():
    port = findSerialPort()

    s = serial.Serial(port, 1000000, timeout=0.001)
    if not s.is_open:
        print("*** serial fail ***")
        exit(1)

    times = deque()
    last = time.time()
    data = deque()
    for _ in range(300):
        s.write(b"g\n")
        d = read_serial(s)
        data.append(d)

    s.close()

    storage.write("data.pickle", data)

    # dd = storage.read("data.pickle")
    # print(dd)

    # print(times)


if __name__ == "__main__":
    main()
