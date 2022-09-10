###############################################
# The MIT License (MIT)
# Copyright (c) 2020 Kevin Walchko
# see LICENSE for full details
##############################################
from struct import Struct
from collections import namedtuple
# from .serialport import SensorData


ImuAGMQT = namedtuple("ImuAGMQT", "ax ay az wx wy wz mx my mz q temperature")
ImuAGMT = namedtuple("ImuAGMT", "ax ay az wx wy wz mx my mz temperature")
ImuAGT = namedtuple("ImuAGT", "ax ay az wx wy wz temperature")
MagneticField = namedtuple("MagneticField","mx my mz")
Range = namedtuple("Range", "range")
LaserScan = namedtuple("LaserScan", "ranges intensities")
TemperaturePressure = namedtuple("TemperaturePressure", "temperature pressure")
# Image = namedtuple("Image","rows cols depth frame")
Image = namedtuple("Image","frame")


SensorData = namedtuple("SensorData","header size unpack cls name")


BNO85 = SensorData(0xA2, 4*(3+3+3+4+1)+1, Struct('<14f').unpack, ImuAGMQT,"BNO085")
BNO80 = SensorData(0xA1, 4*(3+3+3+4+1)+1, Struct('<14f').unpack, ImuAGMQT,"BNO080")
BNO55 = SensorData(0xA0, 4*(3+3+3+4+1)+1, Struct('<14f').unpack, ImuAGMQT,"BNO055")

BMP390 = SensorData(0xB3, 4*2+1, Struct('<2f').unpack, TemperaturePressure, "BMP390")
LPS22 = SensorData(0xB2, 4*2+1, Struct('<2f').unpack, TemperaturePressure, "LPS22")
DPS310 = SensorData(0xB1, 4*2+1, Struct('<2f').unpack, TemperaturePressure, "DPS310")

# 0xC0
IR = SensorData(0xC1, 5, Struct('<f').unpack, Range,"IR")
TFMINI = SensorData(0xF7, 5, Struct('<f').unpack, Range,"TFmini")

# FIXME: this size is way greater than 1 byte: 256 < 4*768 ... could make 2 bytes (65536)
# WARNING: driver has a LOT of floating point operations in it ... BEARE on SW float ops CPU
MLX90640 = SensorData(0xDD, 4*768+1, Struct('<768f').unpack, Image,"MLX90640")

SOX = SensorData(0xD1, 4*(3+3+1)+1, Struct('<7f').unpack, ImuAGT,"LSM6DSOX")
S33 = SensorData(0xD0, 4*(3+3+1)+1, Struct('<7f').unpack, ImuAGT,"LSM6DS33")

SOXLIS = SensorData(0xF1, 4*(3+3+3+1)+1, Struct('<10f').unpack, ImuAGMT,"LSM6DSOXLIS")
S33LIS = SensorData(0xF0, 4*(3+3+3+1)+1, Struct('<10f').unpack, ImuAGMT,"LSM6DS33LIS")

sensorList = [
    BNO85,
    BNO80,
    BNO55,
    DPS310,
    TFMINI,
    SOXLIS,
    S33LIS,
    SOX,
    S33,
    MLX90640
]

sensorDict = {}
for s in sensorList:
    sensorDict[s.header] = s
