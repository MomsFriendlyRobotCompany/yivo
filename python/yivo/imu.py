import struct
from serial import Serial


def read_serial(ser):
    # ff,ff,id,len
    while ser.in_waiting < 4:
        time.sleep(0.001)

    while True:
        chr = ser.read(1)
        if chr != 0xff:
            time.sleep(0.002)
            continue

        chr = ser.read(1)
        if chr != 0xff:
            time.sleep(0.002)
            continue

        try:
            id, len = read(2)
        except Exception:
            time.sleep(0.002)
            continue

        break

    buf = read(len)
    if id == 2:
        decode_imu(buf)
    elif id == 4:
        pass
    elif id == 8:
        pass
    else:
        raise Exception(f"Invalid message ID: {id}")

def height(p):
        """
        given pressure in hPa, returns altitude in meters.
        """
        h = (1 - pow(p / 1013.25, 0.190263)) * 44330.8
        return h

def decode_imu(data):
    msg = struct.unpack("fffffff", data)
    a = msg[:3]   # g's
    g = msg[3:6]  # rads/sec
    t = msg[7]   # C
    # t = t*9/5+32  # F
    return a,g,t

def decode_pt(data):
    msg = struct.unpack("ff", data)
    p = msg[0]    # pressure hPa
    # p = self.height(p)
    t = msg[1]   # C
    return p,t

def decode_mag(data):
    msg = struct.unpack("fff", data)
    m = msg[:3]  # normalized to uTesla
    return m
