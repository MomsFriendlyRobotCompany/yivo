###############################################
# The MIT License (MIT)
# Copyright (c) 2020 Kevin Walchko
# see LICENSE for full details
##############################################
from struct import Struct
from enum import IntEnum, unique # need for Errors
from .parser import YivoParser
from collections import namedtuple
from collections import UserDict
from colorama import Fore

def make_Struct(payload):
    """
    [ 0, 1, 2, 3,4,    5:-2, -1]
    [h0,h1,LN,HN,T, payload, CS]
    Header: h0, h1
    N: payload length
       N = (HN << 8) + LN, max data bytes is 65,536 Bytes
         HN: High Byte
         LN: Low Byte
    T: packet type or MsgID

    Only insert the payload format and this returns: Struct(f"<2cHB{payload}B")
    """
    return Struct(f"<2cHB{payload}B")


Value = namedtuple("Value", "fmt cls")

class MsgInfo(UserDict):
    def __setitem__(self, key, value):
        if isinstance(key, int) == False or key > 255:
            raise Exception("Key must be an int <= 255")
        if isinstance(value, tuple) == False:
            raise Exception("value must be tuple")
        if isinstance(value[0], str) == False:
            raise Exception("value must contain a string and class name", value[0])
        if isinstance(value[1], type) == False:
            raise Exception("value must contain a string and class name:", value[1])

        fmt = Struct(f"<2cHB{value[0]}B")
        cls = value[1]

        value = Value(fmt, cls)
        super().__setitem__(key, value)

@unique
class Errors(IntEnum):
    NONE             = 0
    INVALID_HEADER   = 1
    INVALID_LENGTH   = 2
    INVALID_CHECKSUM = 4
    INVALID_COMMAND  = 8
    INVALID_MSGID    = 16
    NO_DATA          = 32

    @staticmethod
    def str(val):
        if (val == Errors.NONE): return "NONE"
        elif (val == Errors.INVALID_HEADER): return "INVALID_HEADER"
        elif (val == Errors.INVALID_LENGTH): return "INVALID_LENGTH"
        elif (val == Errors.INVALID_CHECKSUM): return "INVALID_CHECKSUM"
        elif (val == Errors.INVALID_COMMAND): return "INVALID_COMMAND"
        elif (val == Errors.INVALID_MSGID): return "INVALID_MSGID"
        elif (val == Errors.NO_DATA): return "NO_DATA"
        return f"UNKNOWN({val})"



# def checksum(size,msgid,msg):
#     if size == 0 and msg == None:
#         return msgid

#     # a,b = struct.pack('H', size)
#     a = 0x00FF & size
#     b = size >> 8

#     cs = (a ^ b)^msgid
#     # msg = [cs] + msg
#     # cs = reduce(xor, msg)
#     for m in msg:
#         cs ^= m
#     # print("cs", cs, cs.to_bytes(1,'little'))
#     return cs

def checksum(msg):
    cs = 0
    for m in msg:
        cs ^= m
    # print("cs", cs, cs.to_bytes(1,'little'))
    return cs & 0xFF

def chunk(msg):
    size = msg[2] + (msg[3] << 8) # messages sent little endian
    msgid = msg[4]

    if size == 0:
        payload = None
    else:
        payload = msg[5:-1]

    cs = msg[-1]
    return size, msgid, payload, cs

def num_fields(sensor):
    """Returns the number of fields in a message"""
    return len(sensor._fields)

class Yivo:
    # [ 0, 1, 2, 3,4, ..., -1]
    # [h0,h1,LN,HN,T, ..., CS]
    # Header: h0, h1
    # N = (HN << 8) + LN, max data bytes is 65,536 Bytes
    #   HN: High Byte
    #   LN: Low Byte
    # T: packet type or MsgID
    pack_cs = Struct("<B")

    def __init__(self, database, h0=b'$', h1=b'K'):
        """
        Message header can be changed (not sure why) if you need to
        by setting a new h0 and h1. They must be binary characters.
        """
        if not isinstance(h0, bytes) or not isinstance(h1, bytes):
            raise Exception(f"Invalid header bytes: {h0}({type(h0)}) {h1}({type(h1)})")
        self.header = (h0,h1,)

        if isinstance(database, MsgInfo) == False:
            raise Exception("database must be a MsgInfo")

        self.msgInfo = database
        self.valid_msgids = [int(x) for x in database.keys()]

        self.parser = YivoParser(self.header)
        self.data = None

    def pack(self, msgID, data):
        """
        Given a MsgID and a tuple of data, returns a yivo message packet
        """
        # if data is None:
        #     msg = struct.pack("<2chBB",*self.header, 0, msgID, msgID)
        # else:
        fmt, _ = self.msgInfo[msgID]
        sz = fmt.size - 6
        msg = fmt.pack(*self.header, sz, msgID, *data, 0)
        # cs = checksum(sz,msgID,msg[5:-1])
        cs = checksum(msg[2:-1])
        msg = msg[:-1] + self.pack_cs.pack(cs) #cs.to_bytes(1,'little')
        return msg

    def dump(self, msg):
        if msg is None:
            return
        size, msgid, payload, cs = chunk(msg)
        payload = [x for x in payload]
        print(f"{Fore.CYAN}==============================================")
        print(f"{Fore.GREEN}Msg: {msg}{Fore.CYAN}")
        print(f"Size: {size}   payload actual size: {len(payload)}")
        print(f"msgid: {msgid}")
        print(f"payload: {payload}")
        print(f"checksum: {cs}   calc checksum: {checksum(msg[2:-1])}")
        print(f"==================================================={Fore.RESET}")

    def unpack(self, msg=None):
        if msg is None:
            return self.__unpack()
        self.data = msg
        return self.__unpack()

    def __unpack(self):
        """
        Unpacks a binary yivo packet

        Returns:
            Errors
            Message
        """
        msg = self.data
        if msg is None:
            return Errors.NO_DATA, None

        err = self.valid_msg(msg)
        if err != Errors.NONE:
            self.dump(msg)
            return err, None

        msgid = msg[4]
        try:
            fmt, obj = self.msgInfo[msgid]
        except KeyError:
            return Errors.INVALID_MSGID, None

        info = fmt.unpack(msg)
        # print(f"{Fore.YELLOW}{info}{Fore.RESET}")
        val = obj(*info[4:-1])

        return Errors.NONE, val

    def valid_msg(self, msg):
        size, msgid, payload, cs = chunk(msg)

        a = ord(self.header[0])
        b = ord(self.header[1])
        if (msg[0] != a) or (msg[1] != b):
            # print(msg[:2], self.header)
            return Errors.INVALID_HEADER

        if msgid not in self.valid_msgids:
            # print(f"invalid id: {msgid}")
            return Errors.INVALID_MSGID

        if (size == 0) or (size != len(payload)):
            # print(len(payload),"!=", size)
            return Errors.INVALID_LENGTH
        # print(size, len(payload))

        # if checksum(size, msgid, payload) != cs:
        if checksum(msg[2:-1]) != cs:
            # print("checksum failure", cs, "!=", checksum(size, msgid, payload))
            # self.dump(msg)
            return Errors.INVALID_CHECKSUM
        # print(cs, checksum(size, msgid, payload))

        try:
            fmt, obj = self.msgInfo[msgid]
        except KeyError:
            return Errors.INVALID_MSGID

        return Errors.NONE

    def parse(self, c):
        if self.parser.parse(c):
            self.data, msgid = self.parser.get_info()
            return msgid
        return 0
