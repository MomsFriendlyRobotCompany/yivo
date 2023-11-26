from yivo import *
from collections import namedtuple
import pytest
from numpy import pi, allclose

def test_pack_n_unpack():

    # make some messages we want to send/receive
    A = namedtuple("A","x y")
    B = namedtuple("B", "x y z t")

    msgdb = MsgInfo()
    msgdb[1] = ("2f", A) # 2 floats
    msgdb[2] = ("4f", B) # 4 floats

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
            assert allclose(tuple(data), tuple([1,2]))
        else:
            assert allclose(tuple(data), tuple([1,2,3,4]))

        msg = [chr(x).encode("latin1") for x in msg]
        for i, c in enumerate(msg):
            mid = yivo.parse(c)
            print(i, mid, c, ord(c))

        assert mid == msgid, print("invalid id", mid,"!=",msgid)

        err, omsg = yivo.unpack()

        assert err == 0, print("final: ", ok, msgid, msg)
        assert allclose(tuple(imsg), tuple(omsg))