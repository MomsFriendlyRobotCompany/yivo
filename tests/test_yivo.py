from yivo import *
import pytest


def test_pack_n_unpack():
    rr = Yivo()
    for k,v in list(Yivo.msgInfo.items()):
        fmt, obj = v
        testsz = num_fields(obj)
        msg = rr.pack(k, range( testsz ))

        err,_,data = rr.unpack(msg)
        if err > 0:
            assert False
            continue
        assert tuple(data) == tuple([float(x) for x in range(testsz)])