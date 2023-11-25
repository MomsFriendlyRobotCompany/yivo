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
        self.buff = []
        self.header = header
        self.buffer_msgid = -1

    def get_info(self):
        data = b''.join(self.buff)
        msgid = self.buffer_msgid
        self.reset()
        return data, msgid

    # def checksum(self,size,msgid,msg):
    #     a = 0x00FF & size
    #     b = size >> 8

    #     cs = (a ^ b)^msgid
    #     for m in msg:
    #         cs ^= ord(m)
    #     # printp("cs", cs, cs.to_bytes(1,'little'))
    #     return cs

    # def checksum(self, msg):
    #     cs = 0
    #     for m in msg:
    #         cs ^= ord(m)
    #     # printp("cs", cs, cs.to_bytes(1,'little'))
    #     return cs

    def reset(self):
        self.buff = None
        self.readState = ReadState_t.NONE_STATE
        self.buffer_msgid = 0
        self.payload_size = 0

    def parse(self, c):
        ret = False
        # print(c)
        # print(">>", c, type(c), type(self.header[0]), c == self.header[0])
        if self.readState == ReadState_t.NONE_STATE:
            if c == self.header[0]:
                self.buff = []
                self.buff.append(c) # h0
                self.readState = ReadState_t.H0_STATE
                self.payload_size = 0
                # self.index = 0
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
            # else:
            #     self.readState = ReadState_t.NONE_STATE
        elif self.readState == ReadState_t.H1_STATE:
            self.buff.append(c) # s0
            self.readState = ReadState_t.S0_STATE
            self.payload_size = ord(c)
            return False
            # print("s0")
        elif self.readState == ReadState_t.S0_STATE:
            self.buff.append(c) # s1
            # cc = ord(c)
            # ccc = ord(self.buff[2])
            # self.payload_size = (cc << 8) | ccc
            self.payload_size |= ord(c) << 8
            # print(f">> payload size:", self.payload_size)
            self.readState = ReadState_t.S1_STATE
            return False
            # printp("s1")
            # print(f"size: {self.payload_size}")
        elif self.readState == ReadState_t.S1_STATE:
            self.buff.append(c) # type
            self.buffer_msgid = ord(c)
            self.readState = ReadState_t.TYPE_STATE
            return False
            # printp("t")
        elif self.readState == ReadState_t.TYPE_STATE:
            # c = ord(c)
            self.buff.append(c) # data0
            self.payload_size -= 1
            self.readState = ReadState_t.DATA_STATE
            # print("d0")
            return False
        elif self.readState == ReadState_t.DATA_STATE:
            # c = ord(c)
            self.buff.append(c) # data1-dataN
            self.payload_size -= 1
            if self.payload_size == 0:
                self.readState = ReadState_t.CS_STATE
            # self.index += 1
            # if self.index == self.payload_size:
            # print(len(self.buff), self.payload_size + 5)

            # if len(self.buff) == (self.payload_size + 5):
            #     self.readState = ReadState_t.CS_STATE
            #     # print(f"data buff size: {len(self.buff)}")
            # if self.payload_size < 0 or len(self.buff) > (self.payload_size + 5):
            #     self.reset()
            #     print(f"!!!!!!!!!!!!!!! WTF !!!!!!!!!!!!!!!!")
            return False
        elif self.readState == ReadState_t.CS_STATE:
            cc = ord(c)
            self.buff.append(c) # cs
            # cs = self.checksum(self.payload_size, self.buffer_msgid, self.buff[5:-1])
            # cs = self.checksum(self.buff[2:-1])
            # cs = self.checksum(self.payload_size, self.buffer_msgid, self.buff)
            self.readState = ReadState_t.NONE_STATE
            return True
            # if cs == cc:
                # ret = True
                # return True
            #     print("success")
            # else:
            #     print("checksum fail")

        return False