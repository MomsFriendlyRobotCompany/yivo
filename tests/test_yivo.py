from yivo import *
import pytest


def test_pack_n_unpack():
    rr = Yivo()
    for k,v in list(Yivo.msgInfo.items()):
        # print(f"[ {Fore.GREEN}{k}{Fore.RESET} ]--------------------------------")
        fmt, obj = v
        # size = fmt.size - 6
        testsz = num_fields(obj)
        msg = rr.pack(k, range( testsz ))
        # print(f"  TX: {Fore.CYAN}{msg}{Fore.RESET}")
        # printMsg(msg)

        err,_,data = rr.unpack(msg)
        if err > 0:
            # print("Invalid msg:", err)
            assert False
            break
        # print(f"\n>> {data}\n")
        assert tuple(data) == tuple([float(x) for x in range(testsz)])