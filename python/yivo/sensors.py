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


SensorData = namedtuple("SensorData","header size unpack cls name")


BNO85 = SensorData(0xA2, 4*(3+3+3+4+1)+1, Struct('<14f').unpack, ImuAGMQT,"BNO085")
BNO80 = SensorData(0xA1, 4*(3+3+3+4+1)+1, Struct('<14f').unpack, ImuAGMQT,"BNO080")
BNO55 = SensorData(0xA0, 4*(3+3+3+4+1)+1, Struct('<14f').unpack, ImuAGMQT,"BNO055")

BMP390 = SensorData(0xB3, 4*2+1, Struct('<2f').unpack, TemperaturePressure, "BMP390")
LPS22 = SensorData(0xB2, 4*2+1, Struct('<2f').unpack, TemperaturePressure, "LPS22")
DPS310 = SensorData(0xB1, 4*2+1, Struct('<2f').unpack, TemperaturePressure, "DPS310")

# 0xC0
TFMINI = SensorData(0xF7, 5, Struct('<f').unpack, Range,"TFmini")

SOX = SensorData(0xD1, 4*(3+3+1)+1, Struct('<7f').unpack, ImuAGT,"LSM6DSOX")
S33 = SensorData(0xD1, 4*(3+3+1)+1, Struct('<7f').unpack, ImuAGT,"LSM6DS33")

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
    S33
]
