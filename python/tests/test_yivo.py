from yivo import *
from collections import namedtuple
import pytest

# make some messages we want to send/receive
A = namedtuple("A","x y")
B = namedtuple("B", "x y z t")

def test_packet():
    msgdb = MsgInfo()
    msgdb[11] = ("2i", A) # 2 ints
    msgdb[22] = ("4i", B) # 4 ints

    yivo = Yivo(msgdb)

    size = yivo.get_msgsize(11)
    assert size == (8+6), f"sizes don't match {size} != 14"
    size = yivo.get_msgsize(22)
    assert size == (6+16), f"sizes don't match {size} != 22"

    a = A(1,2)
    pkt = yivo.pack(11, a)

    assert pkt[0] == ord('$'), f"header fail: {pkt[0]} != '$'"
    assert pkt[1] == ord('K'), f"header fail: {pkt[1]} != 'K'"
    assert pkt[4] == 11, f"header fail: {pkt[4]} != 11"

def test_pack_n_unpack():
    msgdb = MsgInfo()
    msgdb[1] = ("2i", A) # 2 ints
    msgdb[2] = ("4i", B) # 4 ints

    yivo = Yivo(msgdb)

    for msgid, (fmt, obj) in list(msgdb.items()):
        imsg = None
        if msgid == 1: imsg = A(1,2)
        elif msgid == 2: imsg = B(1,2,3,4)
        else: assert False

        msg = yivo.pack(msgid, imsg)
        # print(msgid, msg)

        err, data = yivo.unpack(msg)
        if err > 0:
            assert False, print(err, data)
            continue

        if msgid == 1:
            assert tuple(data) == tuple([1,2])
        else:
            assert tuple(data) == tuple([1,2,3,4])

        msg = [chr(x).encode("latin1") for x in msg]
        for i, c in enumerate(msg):
            mid = yivo.parse(c)
            print(i, mid, c, ord(c))

        assert mid == msgid, print("invalid id", mid,"!=",msgid)

        err, omsg = yivo.unpack()

        assert err == 0, print("final: ", ok, msgid, msg)
        assert tuple(imsg) == tuple(omsg)