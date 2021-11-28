###############################################
# The MIT License (MIT)
# Copyright (c) 2020 Kevin Walchko
# see LICENSE for full details
##############################################
import time
from . import files
# from struct import Struct
# from collections import namedtuple
from .sensors import sensorList
import serial

# # [0xFF,0xFF] # start
# ACCEL      = 0xFE # accel
# GYRO       = 0xFD # gyro
# MAG        = 0xFC # mag
# TEMP_PRES  = 0xFB # temperature, pressure
# # 0xFA #
# LIGHT      = 0xF9 # light
# IR_CAMERA  = 0xF8 # MLX90640 IR camera
# LIDAR      = 0xF7 # lidar
# # 0xF6-0xF3 # unused
# QUATERNION = 0xF2 # quaternion
# VELOCITY   = 0xF1 # velocity
# POSITION   = 0xF0 # position
# # [0xEE,0xEE] # end


class SerialPort:
    def __init__(self, port, buad=1000000):
        self.serial = serial.Serial(port, buad, timeout=0.001)
        if not self.serial.is_open:
            print("*** serial fail ***")
            exit(1)

    def close(self):
        self.serial.close()

    def read(self, c=None):
        ser = self.serial

        if c is None:
            c = b"g\n"
        elif c[-1] != b"\n":
            c += b"\n"

        ser.write(c)

        # ff,ff,msglen
        while ser.in_waiting < 3:
            time.sleep(0.0001)

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

        # [0xff,0xff,len, [header, size, data, ...],[...],0xee,0xee]
        while len > 5:
            id = ser.read(1)
            id = ord(id)

            # sensors = (LIDAR, SOXLIS, DPS310,)
            found = False

            for s in sensorList:
                if id == s.header:
                    data = ser.read(s.size - 1) # ser.read(4)
                    len -= s.size # 5
                    msg = s.cls._make(s.unpack(data))
                    # ans[type(msg).__name__] = msg
                    ans[s.name] = msg
                    found = True
                    break

            if not found:
                print("wtf:", id, len)
                exit(1)

        ans["timestamp"] = time.time()
        return ans


def findSerialPort():
    # handle macOS or Linux
    sp = files.find("/dev","tty.usbmodem*")[0].as_posix()
    print(sp)
    return sp
