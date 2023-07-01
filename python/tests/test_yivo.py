from yivo import *
from collections import namedtuple
import pytest
from numpy import pi, allclose

def test_pack_n_unpack():

    # make some messages we want to send/receive
    A = namedtuple("A","x y")
    B = namedtuple("B", "x y z t")

    msgdb = {
        1: (make_Struct("2f"), A), # 2 floats
        2: (make_Struct("4f"), B)  # 4 floats
    }

    yivo = Yivo(msgdb)
    # print(yivo.msgInfo)

    for msgid, (fmt, obj) in list(msgdb.items()):
        testsz = num_fields(obj)
        # print(f"testz: {testsz}")
        msg = yivo.pack(msgid, [pi]*testsz)
        # print(msgid, msg, obj)

        err,this_id,data = yivo.unpack(msg)
        if err > 0:
            assert False, print(err, this_id, data)
            continue
        assert allclose(tuple(data), tuple([float(x) for x in [pi]*testsz]))

        msg = [chr(x).encode("latin1") for x in msg]
        for i, c in enumerate(msg):
            ok, msgid, msg = yivo.parse(c)
            # print(i, ok, c, ord(c))

        assert ok, print("final: ", ok, msgid, msg)
        assert allclose(tuple(msg), tuple([float(x) for x in [pi]*testsz]))