#!/usr/bin/env python3

import serial
import yivo
from pprint import pprint

import time
import struct
from collections import namedtuple

# [0xFF,0xFF] # start
ACCEL = 0xFE # accel
GYRO = 0xFD # gyro
MAG = 0xFC # mag
TEMP_PRES = 0xFB # temperature, pressure
# 0xFA #
# 0xF9 # light
# 0xF8 # MLX90640 IR camera
LIDAR = 0xF7 # lidar
# 0xF6-0xF3 # unused
# 0xF2 # quaternion
# 0xF1 # velocity
# 0xF0 # position
# [0xEE,0xEE] # end


TempPres = namedtuple("TempPres", "temperature pressure")
Vector2 = namedtuple("Vector3", "x y")
Vector3 = namedtuple("Vector3", "x y z")
Lidar = namedtuple("Lidar","distance")

def read_serial(ser):
    # ff,ff,id,len
    while ser.in_waiting < 3:
        time.sleep(0.1)
        # print(".", ser.in_waiting)
        # print(".", ser.in_waiting, " -- ", ser.read(100))

    # print("got data")

    while True:
        chr = ser.read(1)
        if chr != b'\xff':
            # print(chr)
            time.sleep(0.002)
            continue
        # print("one")

        chr = ser.read(1)
        if chr != b'\xff':
            time.sleep(0.002)
            continue
        # print("two")

        try:
            len = ser.read(1)
            # print("len:", len)
            len = ord(len)
            # print(len)
        except Exception:
            time.sleep(0.002)
            continue

        break

    ans = {}
    while len > 5:
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
    return ans


def main():
    port = "/dev/tty.usbmodem14501"
    s = serial.Serial(port,1000000, timeout=1)
    if not s.is_open:
        print("*** serial fail ***")
        exit(1)

    for _ in range(3):
        s.write(b"g\n")
        s.flush()
        d = read_serial(s)
        pprint(d)
        print("\n ----")

    s.close()

if __name__ == "__main__":
    main()
