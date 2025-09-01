from yivo import *
from collections import namedtuple
import pytest

# make some messages we want to send/receive
A = namedtuple("A","x y")
B = namedtuple("B", "x y z t")

ya = YivoPkt(11, "2i", A) # 2 * 4 = 8, 8+6 = 14
yb = YivoPkt(16, "4i", B) # 4 * 4 = 16, 16+6 = 24

def test_packet():
    size = ya.get_msgsize()
    assert size == (14), f"sizes don't match {size} != 14"
    size = yb.get_msgsize()
    assert size == (22), f"sizes don't match {size} != 22"

    size = ya.get_payload_size()
    assert size == (8), f"sizes don't match {size} != 8"
    size = yb.get_payload_size()
    assert size == (16), f"sizes don't match {size} != 16"

    a = A(1,2)
    pkt = ya.pack(a)

    assert pkt[0] == ord('$'), f"header fail: {pkt[0]} != '$'"
    assert pkt[1] == ord('K'), f"header fail: {pkt[1]} != 'K'"
    assert pkt[4] == 11, f"header fail: {pkt[4]} != 11"

    aa = ya.unpack(pkt)
    assert a == aa, f"{a} != {aa}"


    b = B(1,2,-3,-4)
    pkt = yb.pack(b)

    bb = yb.unpack(pkt)
    assert b == bb, f"{b} != {bb}"

def test_parser():
    pars = YivoParser()

    a = A(1,2)
    apkt = ya.pack(a)

    b = B(1,2,-3,-4)
    bpkt = yb.pack(b)
    # print(f"bpkt: {bpkt}")

    # make a buffer containing multiple messages for the parser
    # to find
    buffer = 128 * [b'\x00']
    buffer[3:3+len(apkt)] = apkt
    buffer[20:20+len(bpkt)] = bpkt
    buffer[70:70+len(apkt)] = apkt

    msg_cnt = 0
    for x in buffer:
        msgid = pars.parse(x)
        if msgid > 0:
            msg_cnt += 1
            # print(f"found {msgid}")

        if msgid == ya.msg_id:
            args = pars.get_msg()
            # print(f"args: {args}")
            aa = ya.unpack(args)
            # print(f"aa: {aa}")
            assert a == aa, f"{a} != {aa}"

        elif msgid == yb.msg_id:
            args = pars.get_msg()
            bb = yb.unpack(args)
            assert b == bb, f"{b} != {bb}"

    assert msg_cnt == 3, f"msg_cnt({msg_cnt}) != 3"
