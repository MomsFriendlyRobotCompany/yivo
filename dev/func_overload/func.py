#!/usr/bin/env python3

from functools import singledispatchmethod

class Bob:
    @singledispatchmethod
    def __init__(self, a):
        raise ValueError(f"error: {a}")

    @__init__.register(int)
    def _from_int(self, a):
        print(f"{self.__class__.__name__} int")

    @__init__.register(float)
    def _from_float(self, a):
        print("float")

    @__init__.register(tuple)
    def _from_float(self, a):
        print("tuple")

    @__init__.register(list)
    def _from_float(self, a, b, *, c=4): # only registers type of 1st arg
        print(f"list: {c}")

    @singledispatchmethod
    def func(self, a):
        print("func wrong?")

    @func.register
    def _(self, a: int):
        print("func int")

b = Bob(1)
bb = Bob(1.23)
bbb = Bob((1,2,3,))
bbbb = Bob([1,2,3], 3)
bbbb = Bob([1,2,3], 3, c=55)

b.func(1)
b.func("hi")