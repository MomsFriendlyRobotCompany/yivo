###############################################
# The MIT License (MIT)
# Copyright (c) 2020 Kevin Walchko
# see LICENSE for full details
##############################################
from struct import Struct
from enum import IntEnum, Enum, unique # need for Errors
from .parser import YivoParser
from collections import namedtuple
# from collections import UserDict
from colorama import Fore

def make_Struct(payload):
    """
    Wraps the payload format inside the header/footer format
    yivo uses.

    [ 0, 1, 2, 3,4,5, 0 - N  ]
    [h0,h1,LN,HN,T,CS,payload]
    Header: h0, h1
    N: payload length
       N = (HN << 8) + LN, max data bytes is 65,536 Bytes
         HN: High Byte
         LN: Low Byte
    T: packet type or MsgID
    CS: checksum of payload

    Only insert the payload format and this returns: Struct(f"<2cHBB{payload}B")
    """
    return Struct(f"<2cHBB{payload}")


# Value = namedtuple("Value", "fmt cls")

# class MsgInfo(UserDict):
#     def __setitem__(self, key, value):
#         if isinstance(key, int) == False or key > 255 or key < 1:
#             raise Exception("Key must be an int from 1 - 255")
#         if isinstance(value, tuple) == False:
#             raise Exception("value must be tuple")
#         if isinstance(value[0], str) == False:
#             raise Exception("value must contain a format string", value[0])
#         if isinstance(value[1], type) == False:
#             raise Exception("value must contain a class name:", value[1])

#         # fmt = Struct(f"<2cHB{value[0]}B")
#         # fmt = value[0]
#         # cls = value[1]
#         fmt, cls = value
#         print(f">> {fmt}")

#         value = Value(Struct("<" + fmt), cls)
#         super().__setitem__(key, value)

#     def get_msgsize(self, msg_id):
#         s = super().__getitem__(msg_id)[0]
#         return s.size
#         # return s

# This is better as an Enum, becuase it prints
# out in a human readable format instead of just
# a number
@unique
class Errors(Enum):
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

# This needs to be an IntEnum because it
# is used to access the serialized data
@unique
class YivoParts(IntEnum):
    HDR0 = 0
    HDR1 = 1
    SZ0 = 2
    SZ1 = 3
    ID = 4
    CS = 5
    PL = 6
    
YIVO_OVERHEAD = 6

def checksum(msg):
    cs = 0
    for m in msg[YivoParts.PL:]:
        cs ^= m
    return cs & 0xFF


def checksum_payload(payload):
    cs = 0
    for m in payload:
        cs ^= m
    return cs & 0xFF

def chunk(msg):
    """
    Breaks up a message into chunks:
    - size: payload size only
    - msgid: message id
    - payload: payload data serialized
    - cs: checksum
    """
    size = msg[YivoParts.SZ0] + (msg[YivoParts.SZ1] << 8) # messages sent little endian
    msgid = msg[YivoParts.ID]
    cs = msg[YivoParts.CS]
    payload = msg[YivoParts.PL:]

    return size, msgid, payload, cs

# def num_fields(data):
#     """Returns the number of fields in a namedtuple message"""
#     return len(data._fields)

class YivoPkt:
    """
    [ 0, 1, 2, 3,4, 5, ...]
    [h0,h1,LN,HN,T,CS, ...]
    Header: h0, h1 = b"$K"
    N = (HN << 8) + LN, max data bytes is 65,536 Bytes
      HN: High Byte
      LN: Low Byte
    T: packet type or MsgID
    header_fmt = Struct("2cHBB")
    """

    def __init__(self, msg_id, fmt, obj):
        """
        Message header can be changed (not sure why) if you need to
        by setting a new h0 and h1. They must be binary characters.
        """
        self.fmt = Struct(f"<2cHBB{fmt}")
        self.payload_fmt = Struct(fmt)
        self.payload_size = self.fmt.size - YIVO_OVERHEAD
        self.msg_id = msg_id
        self.obj = obj
        print(f">> total {msg_id} size: {self.fmt.size}")

    def get_msgsize(self):
        return self.fmt.size

    def get_payload_size(self):
        return self.payload_size

    def pack(self, data):
        """
        Returns a yivo message packet
        """
        sz = self.payload_size
        cs = checksum_payload(data)
        # print(f">> fmt: {self.fmt.format}  data: {data}")
        msg = self.fmt.pack(b'$', b'K', sz, self.msg_id, cs, *data)
        # print(f">> {msg}")
        return msg

    def dump(self, msg):
        if msg is None:
            return
            
        size, msgid, payload, cs = chunk(msg)

        m = [x for x in msg]
        print(f"==============================================")
        print(f"Header: {Fore.GREEN}{m[:6]}")
        print(f" Start: {msg[:2]}")
        print(f" Size: {size}")
        print(f" msgid: {msgid}")
        print(f" checksum: {cs}{Fore.RESET}")
        print(f"Payload: {Fore.MAGENTA}{m[6:]}")
        print(f" payload actual size: {len(payload)}")
        print(f" calc checksum: {checksum(msg)}")
        print(f" format: {self.fmt.format[6:]}{Fore.RESET}")
        print(f"==============================================")

    def unpack(self, msg):
        """
        Unpacks either the full message (header + payload)
        or just the payload.
        """
        func = None
        if msg[:2] == b'$K':
            func = self.__unpack_msg
        else:
            func = self.__unpack_payload

        return func(msg)

    def __unpack_msg(self, msg):
        err = self.valid_msg(msg)
        # print(f">> err: {err}")
        if err != Errors.NONE:
            # self.dump(msg)
            # print(f"unpack ERROR: {err}")
            return None
        info = self.fmt.unpack(msg)
        val = self.obj(*info[5:])
        return val

    def __unpack_payload(self, msg):
        # print(f"unpack msg: {msg}")
        info = self.payload_fmt.unpack(msg)
        val = self.obj(*info)
        return val

    def valid_msg(self, msg):
        """
        Checks message to make sure it is valid, returns
        True if the message is correct, False otherwise.
        """
        size, msgid, payload, cs = chunk(msg)

        hdr0 = ord(b'$')
        hdr1 = ord(b'K')

        if (msg[YivoParts.HDR0] != hdr0) or (msg[YivoParts.HDR1] != hdr1):
            # print(msg[:2],msg[YivoParts.HDR0],msg[YivoParts.HDR1],hdr0,hdr1)
            return Errors.INVALID_HEADER

        if (size == 0) or (size != len(payload)):
            # print(len(payload),"!=", size)
            return Errors.INVALID_LENGTH

        if checksum(msg) != cs:
            # print("checksum failure", cs, "!=", checksum(size, msgid, payload))
            return Errors.INVALID_CHECKSUM

        return Errors.NONE
