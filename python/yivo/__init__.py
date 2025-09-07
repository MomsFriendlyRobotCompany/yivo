###############################################
# The MIT License (MIT)
# Copyright (c) 2020 Kevin Walchko
# see LICENSE for full details
##############################################
from importlib.metadata import version

from .packet import YivoPkt
# from .packet import Errors
from .parser import YivoParser
from .crc8 import calc_checksum

__copyright__ = 'Copyright (c) 2020 Kevin Walchko'
__license__ = 'MIT'
__author__ = 'Kevin J. Walchko'
__version__ = version("yivo")
