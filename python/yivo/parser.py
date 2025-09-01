###############################################
# The MIT License (MIT)
# Copyright (c) 2020 Kevin Walchko
# see LICENSE for full details
##############################################
from enum import IntEnum, Enum
from enum import unique


@unique
class ReadState(Enum):
    STATE_H0 = 0
    STATE_H1 = 1
    STATE_SZ0 = 2
    STATE_SZ1 = 3
    STATE_ID = 4
    STATE_CS = 5
    STATE_DATA = 6

class YivoParser:

    def __init__(self):
        self.readState = ReadState.STATE_H0
        self.buff = None
        self.header = b'$K'
        self.buffer_msgid = 0
        self.index = 0
        self.cs = 0

    def get_info(self):
        data = b''.join(self.buff)
        msgid = self.buffer_msgid
        self.reset()
        return data, msgid

    def get_msg(self):
        # data = b''.join(self.buff)
        args = bytes(self.buff[:self.index])
        # args = b''.join(args)
        print(f"get_msg[{len(args)}]: {args}")
        return args

    # def checksum(self, msg):
    #     cs = 0
    #     for m in msg:
    #         cs ^= ord(m)
    #     # print("cs", cs, cs.to_bytes(1,'little'))
    #     return cs

    def reset(self):
        self.buff = None
        self.readState = ReadState.STATE_H0
        self.buffer_msgid = 0
        self.payload_size = 0
        self.index = 0
        self.cs = 0

    def parse(self, c):
        if c == None:
            return 0

        # ret = False
        # print(c)
        if self.readState == ReadState.STATE_H0:
            if c == self.header[0]:
                self.reset()
                self.readState = ReadState.STATE_H1
                print("h0")
        elif self.readState == ReadState.STATE_H1:
            if c == self.header[1]:
                self.readState = ReadState.STATE_SZ0
                print("h1")
            else: self.reset()
        elif self.readState == ReadState.STATE_SZ0:
            self.readState = ReadState.STATE_SZ1
            self.payload_size = c
            print("s0")
        elif self.readState == ReadState.STATE_SZ1:
            self.payload_size |= c << 8
            self.buff = self.payload_size * [0]
            print(f"payload size:", self.payload_size)
            self.readState = ReadState.STATE_ID
        elif self.readState == ReadState.STATE_ID:
            self.buffer_msgid = c
            self.readState = ReadState.STATE_CS
            print(f"id: {self.buffer_msgid}")
        elif self.readState == ReadState.STATE_CS:
            self.cs = c
            self.readState = ReadState.STATE_DATA
            print(f"cs: {self.cs}")
        elif self.readState == ReadState.STATE_DATA:
            self.buff[self.index] = c # data1-dataN
            self.index += 1
            if self.payload_size == self.index:
                self.readState = ReadState.STATE_H0
                print("done")
                return self.buffer_msgid

        return 0