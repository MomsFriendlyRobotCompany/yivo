###############################################
# The MIT License (MIT)
# Copyright (c) 2020 Kevin Walchko
# see LICENSE for full details
##############################################
from enum import IntEnum

ReadState_t = IntEnum("ReadState_t", [
    "NONE_STATE",
    "H0_STATE",
    "H1_STATE",
    "S0_STATE",
    "S1_STATE",
    "TYPE_STATE",
    "DATA_STATE",
    "CS_STATE"]
)

class YivoParser:

    def __init__(self, header):
        self.readState = ReadState_t.NONE_STATE
        self.buff = None
        self.header = header
        self.buffer_msgid = 0

    def get_info(self):
        data = b''.join(self.buff)
        msgid = self.buffer_msgid
        self.reset()
        return data, msgid

    # def checksum(self, msg):
    #     cs = 0
    #     for m in msg:
    #         cs ^= ord(m)
    #     # print("cs", cs, cs.to_bytes(1,'little'))
    #     return cs

    def reset(self):
        self.buff = None
        self.readState = ReadState_t.NONE_STATE
        self.buffer_msgid = 0
        self.payload_size = 0

    def parse(self, c):
        if c == None or len(c) == 0:
            return False

        ret = False
        # print(c)
        if self.readState == ReadState_t.NONE_STATE:
            if c == self.header[0]:
                self.buff = []
                self.buff.append(c) # h0
                self.readState = ReadState_t.H0_STATE
                self.payload_size = 0
                # print("h0")
            return False
        elif self.readState == ReadState_t.H0_STATE:
            if c == self.header[1]:
                self.buff.append(c) # h1
                self.readState = ReadState_t.H1_STATE
                # print("h1")
                return False
            self.reset()
            return False
        elif self.readState == ReadState_t.H1_STATE:
            # c = ord(c)
            self.buff.append(c) # s0
            self.readState = ReadState_t.S0_STATE
            self.payload_size = ord(c)
            return False
            # print("s0")
        elif self.readState == ReadState_t.S0_STATE:
            # c = ord(c)
            self.buff.append(c) # s1
            self.payload_size |= ord(c) << 8
            # print(f">> payload size:", self.payload_size)
            self.readState = ReadState_t.S1_STATE
            return False
            # printp("s1")
            # print(f"size: {self.payload_size}")
        elif self.readState == ReadState_t.S1_STATE:
            # c = ord(c)
            self.buff.append(c) # type
            self.buffer_msgid = ord(c)
            self.readState = ReadState_t.TYPE_STATE
            return False
            # printp("t")
        elif self.readState == ReadState_t.TYPE_STATE:
            self.buff.append(c) # data0
            self.payload_size -= 1
            self.readState = ReadState_t.DATA_STATE
            # print("d0")
            return False
        elif self.readState == ReadState_t.DATA_STATE:
            self.buff.append(c) # data1-dataN
            self.payload_size -= 1
            if self.payload_size == 0:
                self.readState = ReadState_t.CS_STATE
            return False
        elif self.readState == ReadState_t.CS_STATE:
            # c = ord(c)
            self.buff.append(c) # cs
            # cs = self.checksum(self.buff[2:-1])
            self.readState = ReadState_t.NONE_STATE
            return True

        return False