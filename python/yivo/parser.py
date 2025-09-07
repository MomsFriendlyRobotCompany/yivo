###############################################
# The MIT License (MIT)
# Copyright (c) 2020 Kevin Walchko
# see LICENSE for full details
##############################################
from enum import IntEnum, Enum
from enum import unique
from .crc8 import crc8_table

DEBUG = False

@unique
class State(Enum):
    HDR0 = 0
    HDR1 = 1
    SZ0 = 2
    SZ1 = 3
    ID = 4
    CS = 5
    DATA = 6

class YivoParser:
    def __init__(self):
        self.header = b'$K'
        self.reset()

    # def get_info(self):
    #     data = b''.join(self.buff)
    #     msgid = self.msgid
    #     self.reset()
    #     return data, msgid

    def get_msg(self):
        # args = bytes(self.buff[:self.index])
        args = bytes(self.buff)
        if DEBUG: print(f"get_msg[{len(args)}]: {args}")
        return args

    def reset(self):
        self.buff = None
        self.state = State.HDR0
        self.msgid = 0
        self.payload_size = 0
        self.index = 0
        self.msg_cs = 0
        self.calc_crc = 0

    def parse(self, c):
        if c == None:
            return 0

        if self.state == State.HDR0:
            if c == self.header[0]:
                self.reset()
                self.state = State.HDR1
                if DEBUG: print(self.state)
        elif self.state == State.HDR1:
            if c == self.header[1]:
                self.state = State.SZ0
                if DEBUG: print(self.state)
            else: self.reset()
        elif self.state == State.SZ0:
            self.state = State.SZ1
            self.payload_size = c
            self.calc_crc = crc8_table[self.calc_crc ^ c]
            if DEBUG: print(self.state)
        elif self.state == State.SZ1:
            self.payload_size |= c << 8
            self.buff = self.payload_size * [0]
            self.calc_crc = crc8_table[self.calc_crc ^ c]
            if DEBUG: print(f"payload size: {self.payload_size}")
            self.state = State.ID
        elif self.state == State.ID:
            if c == 0:
                self.reset()
                return 0
            self.msgid = c
            self.calc_crc = crc8_table[self.calc_crc ^ c]
            self.state = State.CS
            if DEBUG: print(f"{self.state} id: {self.msgid}")
        elif self.state == State.CS:
            self.msg_cs = c
            self.state = State.DATA
            if DEBUG: print(f"{self.state} cs: {self.msg_cs}")
        elif self.state == State.DATA:
            self.buff[self.index] = c # data1-dataN
            self.calc_crc = crc8_table[self.calc_crc ^ c]
            self.index += 1
            if self.payload_size == self.index:
                self.state = State.HDR0
                if DEBUG: print(f"{self.state}  {self.msg_cs} == {self.calc_crc}")
                if self.msg_cs == self.calc_crc:
                    return self.msgid
                else:
                    self.reset()

        return 0