###############################################
# The MIT License (MIT)
# Copyright (c) 2020 Kevin Walchko
# see LICENSE for full details
##############################################

import pathlib

def find(path, fname):
    """Given a path, this will recursively search for a file (bob.txt) or
    pattern (\*.txt). It returns an array of found file paths."""
    fn = []
    for p in pathlib.Path(path).rglob(fname):
        fn.append(p)
    return fn
