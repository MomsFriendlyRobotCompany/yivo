###############################################
# The MIT License (MIT)
# Copyright (c) 2020 Kevin Walchko
# see LICENSE for full details
##############################################
from struct import Struct
from colorama import Fore
from collections import namedtuple
from enum import IntEnum, Enum, unique # need for Errors
from .parser import YivoParser
from .crc8 import calc_checksum, crc8_table

DEBUG = False

db_entry = namedtuple("db_entry", "pl_fmt cls")

@unique
class Msg(IntEnum):
    HDR0 = 0
    HDR1 = 1
    SZ0 = 2
    SZ1 = 3
    ID = 4
    CS = 5
    PL = 6

# This is better as an Enum, becuase it prints
# out in a human readable format instead of just
# a number
@unique
class Errors(Enum):
    NONE             = 0
    NO_DATA          = -3
    INVALID_LENGTH   = -4
    INVALID_HEADER   = -5
    INVALID_MSGID    = -6
    INVALID_CHECKSUM = -7

    def __eq__(self, val):
        return self.value == val

    def __ne__(self, val):
        return self.value != val

    # @staticmethod
    # def str(val):
    #     if (val == Errors.NONE): return "NONE"
    #     elif (val == Errors.INVALID_HEADER): return "INVALID_HEADER"
    #     elif (val == Errors.INVALID_LENGTH): return "INVALID_LENGTH"
    #     elif (val == Errors.INVALID_CHECKSUM): return "INVALID_CHECKSUM"
    #     elif (val == Errors.INVALID_COMMAND): return "INVALID_COMMAND"
    #     elif (val == Errors.INVALID_MSGID): return "INVALID_MSGID"
    #     elif (val == Errors.NO_DATA): return "NO_DATA"
    #     return f"UNKNOWN({val})"
    
YIVO_OVERHEAD = 6  # Header ($K), size (2 bytes), ID, checksum
YIVO_HEADER_0 = ord('$')
YIVO_HEADER_1 = ord('K')


class YivoPkt:
    """
    This packet formatter handles one type of message
    
    [ 0, 1, 2, 3, 4, 5, ...]
    [h0,h1,LN,HN,ID,CS, ...]
    Header: h0, h1 = b"$K"
    N = (HN << 8) + LN, max data bytes is 65,536 Bytes
      HN: High Byte
      LN: Low Byte
    ID: packet type or MsgID
    header_fmt = Struct("2cHBB")
    """

    # def __init__(self, msg_id, fmt, obj):
    def __init__(self):
        """
        Message header can be changed (not sure why) if you need to
        by setting a new h0 and h1. They must be binary characters.
        """
        # if msg_id < 1 or msg_id > 0xFF:
        #     raise ValueError("Invalid msg_id")

        # remove little endian off of payload
        # if fmt[0] == '<':
        #     fmt = fmt[1:]
        # self.fmt = Struct(f"<2cHBB{fmt}")

        # add back in little endian for payload format
        # if fmt.find('<') == -1:
        #     fmt = '<' + fmt

        # self.payload_fmt = Struct(fmt)
        # self.payload_size = self.payload_fmt.size
        # self.msg_id = msg_id
        # self.obj = obj
        # if DEBUG: print(f">> total {msg_id} size: {self.fmt.size}")

        self.db = {}

    def register_msg(self, msg_id, fmt, cls):
        if msg_id < 1 or msg_id > 0xFF:
            raise ValueError("Invalid msg_id")

        # remove little endian off of payload
        if fmt[0] != '<':
            fmt = '<' + fmt

        self.db[msg_id] = db_entry(Struct(fmt), cls)

    def register_db(self, db):
        self.db = db

    def get_msgsize(self, msgid):
        return self.db[msgid].pl_fmt.size + YIVO_OVERHEAD

    def get_payload_size(self):
        return self.db[msgid].pl_fmt.size

    def get_cls(self, msgid):
        return self.db[msgid].cls

    def pack(self, msgid, data):
        """
        Returns a yivo message packet
        """
        if msgid not in self.db:
            return None

        payload_fmt, _ = self.db[msgid]
        sz = payload_fmt.size
        payload = payload_fmt.pack(*data)
        msg = bytearray(sz + YIVO_OVERHEAD)
        
        # msg = bytearray(self.payload_size + YIVO_OVERHEAD)
        # sz = self.payload_size
        # payload = self.payload_fmt.pack(*data)
        hi = (sz >> 8) & 0xFF
        lo = sz & 0xFF

        # calc checksum
        crc = 0x00
        crc = crc8_table[crc ^ lo]
        crc = crc8_table[crc ^ hi]
        # crc = crc8_table[crc ^ self.msg_id]
        crc = crc8_table[crc ^ msgid]
        for b in payload:  # Payload
            crc = crc8_table[crc ^ b]
        crc = crc & 0xFF

        msg[0] = YIVO_HEADER_0
        msg[1] = YIVO_HEADER_1
        msg[2] = lo # Low byte
        msg[3] = hi # High byte
        msg[4] = msgid 
        # msg[4] = self.msg_id
        msg[5] = crc # checksum
        msg[6:] = payload

        return msg

    def dump(self, msg):
        if msg is None:
            return
            
        size, msgid, payload, cs = self.chunk(msg)
        pl_fmt, obj = self.db[msgid]

        m = [x for x in msg]
        print(f"==============================================")
        print(f"Class: {obj.__class__.__name__}")
        print(f"Header: {Fore.GREEN}{m[:Msg.PL]}")
        print(f" Start: {msg[:Msg.SZ0]}")
        print(f" Size: {size}")
        print(f" msgid: {msgid}")
        print(f" checksum: {cs}{Fore.RESET}")
        print(f"Payload: {Fore.MAGENTA}{m[Msg.PL:]}")
        print(f" payload actual size: {len(payload)}")
        print(f" calc checksum: {calc_checksum(msg)}")
        print(f" format: {pl_fmt.format}{Fore.RESET}")
        print(f"==============================================")

    def unpack(self, msg):
        """
        Unpacks either the full message (header + payload)
        or just the payload.
        """
        # func = None
        # if msg[:2] == b'$K':
        #     func = self.__unpack_msg
        # else:
        #     func = self.__unpack_payload

        # return func(msg)

    # def __unpack_msg(self, msg):
        err = self.valid_msg(msg)
        # print(f">> err: {err}")
        if err != Errors.NONE:
            self.dump(msg)
            # print(f"unpack ERROR: {err}")
            return None
        # info = self.fmt.unpack(msg)
        # val = self.obj(*info[5:])
        msgid = msg[Msg.ID]
        pl_fmt, obj = self.db[msgid]
        info = pl_fmt.unpack(msg[Msg.PL:])
        val = obj(*info)
        return val

    # def __unpack_payload(self, msg):
    #     # print(f"unpack msg: {msg}")
    #     info = self.payload_fmt.unpack(msg)
    #     val = self.obj(*info)
    #     return val

    def chunk(self, msg):
        size = msg[Msg.SZ0] + (msg[Msg.SZ1] << 8) # messages sent little endian
        msgid = msg[Msg.ID]
        cs = msg[Msg.CS]
        payload = msg[Msg.PL:]

        return size, msgid, payload, cs

    def valid_msg(self, msg):
        """
        Checks message to make sure it is valid, returns
        True if the message is correct, False otherwise.
        """
        if msg is None:
            return Errors.NO_DATA

        size, msgid, payload, cs = self.chunk(msg)

        hdr0 = YIVO_HEADER_0 #ord(b'$')
        hdr1 = YIVO_HEADER_1 #ord(b'K')

        if (msg[Msg.HDR0] != hdr0) or (msg[Msg.HDR1] != hdr1):
            if DEBUG:  print(msg[:2],msg[Msg.HDR0],msg[Msg.HDR1],hdr0,hdr1)
            return Errors.INVALID_HEADER

        if (size == 0) or (size != len(payload)):
            if DEBUG: print(len(payload),"!=", size)
            return Errors.INVALID_LENGTH

        if calc_checksum(msg) != cs:
            if DEBUG: print("checksum failure", cs, "!=", checksum(size, msgid, payload))
            return Errors.INVALID_HEADER

        return Errors.NONE
