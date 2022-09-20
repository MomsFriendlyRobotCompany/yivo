###############################################
# The MIT License (MIT)
# Copyright (c) 2020 Kevin Walchko
# see LICENSE for full details
##############################################
from collections import namedtuple
from struct import Struct
import struct
from collections import namedtuple
from collections import deque
from enum import IntEnum, unique
import time

COMMAND = namedtuple("COMMAND", "id")

REQUEST = namedtuple("REQUEST","msgid")
IDENT = namedtuple("IDENT", "version multitype msp_version capability")
IMU_RAW = namedtuple("IMU_RAW", "ax ay az gx gy gz mx my mz")
ATTITUDE = namedtuple("ATTITUDE", "roll pitch heading")
ALTITUDE = namedtuple("ALTITUDE", "alt vario")

CALIBRATION = namedtuple("CALIBRATION", "bx by bz sx sy sz") # bias, scale
PID = namedtuple("PID", "kp ki kd")

Motors4 = namedtuple("Motors4","m0 m1 m2 m3 armed")

ImuAGMQPT     = namedtuple("ImuAGMQPT", "ax ay az wx wy wz imu_temp mx my mz qw qx qy qz pressure temperature ts")
ImuAGMQT      = namedtuple("ImuAGMQT", "ax ay az wx wy wz mx my mz qw qx qy qz temperature ts")
ImuAGMT       = namedtuple("ImuAGMT",  "ax ay az wx wy wz mx my mz temperature ts")
ImuAGT        = namedtuple("ImuAGT",   "ax ay az wx wy wz temperature ts")
ImuAT         = namedtuple("ImuAT",    "ax ay az temperature ts")
MagneticField = namedtuple("MagneticField","mx my mz")
Range         = namedtuple("Range", "range")
# LaserScan     = namedtuple("LaserScan", "ranges intensities")
TemperaturePressure = namedtuple("TemperaturePressure", "temperature pressure")

YivoError = namedtuple("YivoError","error")
YivoOK = namedtuple("YivoOK", "val")


@unique
class Errors(IntEnum):
    NONE             = 0
    INVALID_HEADER   = 1
    INVALID_LENGTH   = 2
    INVALID_CHECKSUM = 4
    INVALID_COMMAND  = 8
    INVALID_MSGID    = 16


@unique
class MsgIDs(IntEnum):
    """MSP message ID"""
    # Commands
    PING = 10
    # CALIBRATION_DATA     = 14  # bias, scale
    # SET_CALIBRATION_DATA = 15  # bias, scale
    REBOOT = 68

    # 100's are from the Flight Controller ===========
    # IDENT    = 100
    # STATUS   = 101
    # RAW_IMU  = 102  # 9DOF
    # SERVO    = 103
    # MOTOR    = 104
    # RC       = 105
    # RAW_GPS  = 106
    # COMP_GPS = 107
    # ATTITUDE = 108
    # ALTITUDE   = 109
    # ANALOG     = 110
    # RC_TUNING  = 111
    # PID        = 112
    # POSE       = 113
    # BOX        = 113
    # MISC       = 114
    # MOTOR_PINS = 115
    # BOXNAMES   = 116
    # PIDNAMES   = 117
    # WP         = 118
    # BOXIDS     = 119
    # SERVO_CONF = 120


    IMU_AGMQPT= 139
    IMU_AGMQT = 140
    IMU_AGMT  = 141
    IMU_AGT   = 142
    IMU_AT    = 143
    MAGNETIC  = 144
    RANGE     = 145
    GPS       = 146
    TEMP_PRES = 147
    VOLTAGE   = 148


    MOTORS       = 200
    SET_MOTORS      = 201
    ACC_CALIBRATION  = 202  # bias, scale ??
    MAG_CALIBRATION  = 203  # bias, scale ??
    GYR_CALIBRATION  = 204  # bias, scale ??
    SET_TEL_STREAM   = 205
    # SET_MISC         = 207
    # RESET_CONF       = 208
    # SET_WP           = 209
    # SWITCH_RC_SERIAL = 210
    # SET_HEADING      = 211
    # SET_POSE          = 211  # pos (x,y,altitude), attitude(roll,pitch,yaw)
    # SET_SERVO_CONF   = 212

    YIVO_ERROR = 250

valid_msgids = [int(x) for x in MsgIDs]

def checksum(size,msgid,msg):
    if size == 0 and msg == None:
        return msgid

    a,b = struct.pack('H', size)
    # cs = size ^ msgid
    cs = a ^ b
    cs ^= msgid
    for m in msg:
        cs ^= m
    # print("cs", cs, cs.to_bytes(1,'little'))
    return cs

def chunk(msg):
    # size = msg[3]
    # msgid = msg[4]
    # payload = msg[5:-1]
    # cs = msg[-1]
    # return size, msgid, payload, cs
    size = msg[2] + (msg[3] << 8) # messages sent little endian
    msgid = msg[4]

    if size == 0:
        payload = None
    else:
        payload = msg[5:-1]

    cs = msg[-1]
    return size, msgid, payload, cs

def num_fields(sensor):
    return len(sensor._fields)

class Yivo:

    # [ 0, 1, 2, 3,4, ..., -1]
    # [h0,h1,LN,HN,T, ..., CS]
    # Header: h0, h1
    # N = (HN << 8) + LN, max data bytes is 65,536 Bytes
    #   HN: High Byte
    #   LN: Low Byte
    # T: packet type or MsgID
    msgInfo = {
        # MsgIDs.REQUEST:   (Struct("<2chBB"), REQUEST),
        # MsgIDs.IDENT:     (Struct("<2chBBBBIB"), IDENT,),
        # MsgIDs.RAW_IMU:   (Struct("<2chB9hB"),   IMU_RAW,),
        # MsgIDs.PING: (Struct("<2chBB"), COMMAND,),
        # MsgIDs.REBOOT: (Struct("<2chBB"), COMMAND,),
        # MsgIDs.PING: (Struct("<2chBB"), COMMAND,),
        MsgIDs.IMU_AGMQPT: (Struct("<2chB16fLB"), ImuAGMQPT,),
        MsgIDs.IMU_AGMQT:  (Struct("<2chB14fLB"), ImuAGMQT,),
        MsgIDs.IMU_AGMT:   (Struct("<2chB10fLB"), ImuAGMT,),
        MsgIDs.IMU_AGT:    (Struct("<2chB7fLB"),  ImuAGT,),
        MsgIDs.IMU_AT:     (Struct("<2chB4fLB"),  ImuAT,),
        MsgIDs.MAGNETIC:   (Struct("<2chB3fB"),   MagneticField,),
        MsgIDs.TEMP_PRES:  (Struct("<2chB2fB"),   TemperaturePressure,),
        MsgIDs.ACC_CALIBRATION: (Struct("<2chB6fB"),   CALIBRATION,),
        MsgIDs.MAG_CALIBRATION: (Struct("<2chB6fB"),   CALIBRATION,),
        MsgIDs.GYR_CALIBRATION: (Struct("<2chB6fB"),   CALIBRATION,),
        MsgIDs.MOTORS: (Struct("<2chB5hB"), Motors4),
        MsgIDs.YIVO_ERROR: (Struct("<2chBBB"),   YivoError,),
    }
    pack_cs = Struct("<B")

    def __init__(self, h0=b'$', h1=b'K'):
        """
        Message header can be changed (not sure why) if you need to
        by setting a new h0 and h1. They must be binary characters.
        """
        if not isinstance(h0, bytes) or not isinstance(h1, bytes):
            raise Exception(f"Invalid header bytes: {h0}({type(h0)}) {h1}({type(h1)})")
        self.header = (h0,h1,)

    def pack(self, msgID, data=None):
        """
        Given a MsgID and a tuple of data, returns a yivo message packet
        """
        if data is None:
            msg = struct.pack("<2chBB",*self.header, 0, msgID, msgID)
        else:
            fmt, _ = self.msgInfo[msgID]
            sz = fmt.size - 6
            # print(f">> {fmt.size - 6} == {sz}")
            # cs = checksum(sz,msgID,data) # can't do this, cs will be diff!!
            # direction = b'>' if msgID < 200 else b'<'
            # msg = fmt.pack(b'$',b'M', sz, msgID, *data, 0)
            msg = fmt.pack(*self.header, sz, msgID, *data, 0)
            cs = checksum(sz,msgID,msg[5:-1])
            msg = msg[:-1] + self.pack_cs.pack(cs) #cs.to_bytes(1,'little')
        return msg

    def unpack(self,msg):
        """
        Unpacks a binary yivo packet

        Returns:
            MsgID
            Message
        """
        size, msgid, payload, cs = chunk(msg)
        # print("size",size)

        # print(msg[0],self.header[0],msg[1], self.header[1])
        # if (msg[:2] != b"$M"):
        # if (msg[:2] != self.header):
        a = ord(self.header[0])
        b = ord(self.header[1])
        if (msg[0] != a) or (msg[1] != b):
            print(msg[:2], self.header)
            return Errors.INVALID_HEADER, None, None

        if msgid not in valid_msgids:
            print(f"invalid id: {msgid}")
            return Errors.INVALID_MSGID, None, None

        if (size != 0) and (size != len(payload)):
            print(len(payload),"!=", size)
            return Errors.INVALID_LENGTH, None, None
        # print(size, len(payload))

        if checksum(size, msgid, payload) != cs:
            print("checksum failure", cs, "!=", checksum(size, msgid, payload))
            return Errors.INVALID_CHECKSUM, None, None
        # print(cs, checksum(size, msgid, payload))

        try:
            fmt, obj = self.msgInfo[msgid]
        except KeyError:
            return Errors.INVALID_MSGID, msgid, None

        if size > 0:
            info = fmt.unpack(msg)
            # print(info)
            val = obj(*info[4:-1])
        else:
            val = REQUEST(msgid)

        return Errors.NONE, msgid, val

    def read_packet(self, ser, retry=64):
        """
        ser: serial object
        retry: how many times to retry reading the serial port to find the header

        Return: namedtuple message or None
        """
        # while ser.in_waiting < 5:
        #     time.sleep(0.0001)

        # make a FIFO of size 2 to hold the header
        rbuf = deque(maxlen=2)
        rbuf.append(ser.read(1))
        header = deque(self.header)

        # while True:
        while retry:
            retry -= 1
            # get header
            rbuf.append(ser.read(1))
            if rbuf != header:
                # print(rbuf)
                continue
            break

        # didn't find header
        # if retry == 0:
        #     print("** No msg")
        #     return None

        sz = ser.read(2)
        size = (sz[1] << 8) + sz[0]
        msgid = ord(ser.read(1))
        payload = ser.read(size)
        cs = ord(ser.read(1))
        # print(rbuf, sz, msgid.to_bytes(1, 'little'), payload, cs.to_bytes(1, 'little'))
        msg = b''.join([rbuf[0],rbuf[1], sz, msgid.to_bytes(1, 'little'), payload, cs.to_bytes(1, 'little')])
        # return list(rbuf)+sz+[msgid]+payload+[cs]

        try:
            fmt, obj = self.msgInfo[msgid]
        except KeyError:
            return None
        data = fmt.unpack(msg)[4:-1]
        return obj(*data)
