from yivo import *
from collections import namedtuple
import pytest
from numpy import pi, allclose

from yivo.generator.yivo_gen import main

def test_generator():
    info = {
        "namespace": "foobar",
        "license": "MIT Kevin Walchko (c) 2023",
        "output": "tmp",
        1: "tests/messages/vec.yivo",
        2: "tests/messages/quat.yivo",
        4: "tests/messages/imu.yivo",
        5: "tests/messages/cal.yivo"
    }

    main(info)

    from tmp.python.vec_t import vec_t
    from tmp.python.base import fmt, cls, sizeof, id2str, msg_id

    v = vec_t(1,2,3)

    assert fmt(v) == "fff"
    assert sizeof(v) == 12
    assert cls(v) == vec_t
    assert id2str(msg_id(v)) == "vec_t"

    from tmp.python.quat_t import quat_t

    q = quat_t(1,2,3,4)
    assert fmt(q) == "ffff"
    assert sizeof(q) == 16
    assert cls(q) == quat_t
    assert id2str(msg_id(q)) == "quat_t"