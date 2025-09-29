#!/usr/bin/env python3
import socket
import time
from collections import namedtuple
from yivo import *

PORT = 8080
SERVER = '127.0.0.1'
BUFFER_SIZE = 1024

# class Base:
#     def flatten(self, data):
#         """
#         (0, (1, 2, 3), (4, 5, 6)) -> (0, 1, 2, 3, 4, 5, 6)
#         """
#         if isinstance(data, tuple):
#             for x in data:
#                 yield from self.flatten(x)
#         else:
#             yield data

#     def to_tuple(self):
#         return tuple(self.flatten(self.astuple()))

#     # def __iter__(self):
#     #     # Return iterator object (self or a generator)
#     #     for item in self.to_tuple():
#     #         yield item

# class next_t:
#     def __init__(self, a,b,c):
#         self.a = a
#         self.b = b
#         self.c = c

# Base = namedtuple("Base", "id")
# class Base:
#     def __init__(self, id):
#         self.id = id

#     def flatten(self, data):
#         """
#         (0, (1, 2, 3), (4, 5, 6)) -> (0, 1, 2, 3, 4, 5, 6)
#         """
#         if isinstance(data, tuple):
#             for x in data:
#                 yield from self.flatten(x)
#         else:
#             yield data

#     def __iter__(self):
#         # f = self.__dict__.copy()
#         # fields = f.pop("id").keys()
#         # Return iterator object (self or a generator)
#         for item in self.fields:
#             yield self.__dict__[item]

# class test_t(Base):
#     def __init__(self, x, y):
#         super().__init__(10)
#         self.x = x
#         self.y = y
#         f = self.__dict__.copy()
#         f.pop("id")
#         self.fields = f.keys()

# a = test_t(1,2)

test_t = namedtuple("test_t","x y")


# ya = YivoPkt(10, "2i", test_t) # 2 * 4 = 8, 8+6 = 14
ya = YivoPkt() # 2 * 4 = 8, 8+6 = 14
ya.register_msg(10, "2i", test_t)

# Create UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Set 1-second timeout for receive operations
sock.settimeout(1.0)

# Send initial message to server to provide client address
initial_message = "init"
sock.sendto(initial_message.encode(), (SERVER, PORT))
print(f"Sent initial message: {initial_message}")

while True:
    try:
        a = test_t(1,2)
        pkt = ya.pack(10, a)
        sock.sendto(pkt, (SERVER, PORT))
        print(f"Sent: {len(pkt)}")

        # Receive message from server
        try:
            data, addr = sock.recvfrom(BUFFER_SIZE)
            b = ya.unpack(data)
            print(f"Received: {b}")
            ya.dump(data)
            
        except socket.timeout:
            print("Receive timed out after 1 second")
            
        # Brief delay to avoid flooding
        time.sleep(.5)
        
    except socket.error as e:
        print(f"Socket error: {e}")

sock.close()