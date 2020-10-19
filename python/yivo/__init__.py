###############################################
# The MIT License (MIT)
# Copyright (c) 2020 Kevin Walchko
# see LICENSE for full details
##############################################

try:
    from importlib.metadata import version
except ImportError:
    from importlib_metadata import version

__copyright__ = 'Copyright (c) 2020 Kevin Walchko'
__license__ = 'MIT'
__author__ = 'Kevin J. Walchko'
__version__ = version("yivo")
