#!/usr/bin/env python3
from jinja2 import Environment
from jinja2.loaders import FileSystemLoader
import re
import argparse
import pathlib
from collections import namedtuple
import logging
from colorama import Fore
import sys
import json

from .format import create_python, create_c_header
from .format import env, write_file
from .format import var_types, VarInfo
from .msg_parts import MsgParts

# logging: DEBUG ERROR INFO
logging.basicConfig(format='>> %(message)s', level=logging.ERROR)
logger = logging.getLogger(__name__)

Field = namedtuple("Field", "type array_size var comment")

class Enums:
    def __init__(self, name, size=None):
        self.name = name
        self.size = size
        self.values = []

def tokenize(file):
    # Parse a template file into a dictionary of tokens
    # that are processed into C++ or Python message headers
    #
    # regex
    # ([\w]+) *([\w]+) *([#\w_ ]*)
    #
    # Everything is either:
    # - comment: starts with #
    # - include
    # - enum: start "<enum" and end "enum>"
    # - c function: start "<c" and end "c>"
    # - python function: start "<p" and end "p>"
    # - variable: <type> <name> <comment>
    # - array: <type[N]> <name> <comment>

    with file.open() as fd:
        lines = fd.read()

    mp = MsgParts()
    mp.file = file
    c_func_open = False
    py_func_open = False
    enum_open = False
    enums = None
    reg = re.compile(r'([\w\[\]]+) *([\w]+) *([#\w_ ]*)')

    for line in lines.split('\n'):
        line = line.rstrip().lstrip()
        if len(line) == 0: continue

        if line.find("<c") == 0:
            c_func_open = True
            continue
        if line.find("c>") == 0:
            c_func_open = False
            continue

        if c_func_open:
            mp.c_funcs.append(line)
            continue

        if line.find("<p") == 0:
            py_func_open = True
            continue
        if line.find("p>") == 0:
            py_func_open = False
            continue

        if py_func_open:
            mp.py_funcs.append(line)
            continue

        if line.find("<enum") == 0:
            # <enum name size
            toks = line.split(" ")
            enums = Enums(toks[1].upper())
            if len(toks) == 3: enums.size = toks[2]
            enum_open = True
            continue
        if line.find("enum>") == 0:
            enum_open = False
            mp.enums.append(enums)
            enums = None
            continue
        if enum_open:
            enums.values.append(line.upper())
            continue

        if line.find("namespace") >=0:
            mp.namespace = line.split(" ")[1]
            continue

        if line[0] == "#":
            mp.comments.append(line)
            continue

        toks = reg.findall(line)[0]
        logger.info(f"{toks}")

        if toks[0] == "include":
            mp.includes.append(toks[1])
            continue

        typ, var, comment = toks
        if len(comment) == 0: comment = "# "

        # find array brackets in typ
        array_size = 0
        left = typ.find('[')
        right = typ.find(']')
        if left > 0 and right > 0: # array found
            array_size = int(typ[left+1:right])
            typ = typ[:left]
            mp.msg_size += var_types[typ].size*array_size
            comment += f"size: {var_types[typ].size} * {array_size}"
        else: # scalar
            try:
                mp.msg_size += var_types[typ].size
                comment += f"size: {var_types[typ].size}"
            except:
                print(f"{Fore.RED}*** Invalid keyword in line: {line} ***{Fore.RESET}")
                continue

        args = Field(typ, array_size, var, comment)
        mp.fields.append(args)
        # else:
        #     logger.error(f"invalid line: {subline} -> {toks}")

    return mp

def main(files):
    license = ""
    if "license" in files.keys():
        license = files["license"]
        files.pop("license", None)

    output = "."
    if "output" in files.keys():
        output = files["output"]
        files.pop("output", None)
    output = pathlib.Path(output)
    cppdir = output/"cpp"
    cppdir.mkdir(parents=True, exist_ok=True)
    pydir = output/"python"
    pydir.mkdir(parents=True, exist_ok=True)

    namespace = None
    if "namespace" in files.keys():
        namespace = files["namespace"]
        files.pop("namespace", None)


    # create the base headers for messages ------------
    ids = {}
    for k,v in files.items():
        ids[k] = v.split('.')[0]

    baseinfo = [
        ("base.py.jinja", ids, "py"),
        ("base.cpp.jinja", ids, "cpp"),
    ]

    for template, ids, ext in baseinfo:
        tmpl = env.get_template(template)
        info = {
            "license_notice": license,
            "ids": ids,
            "namespace": namespace
        }
        content = tmpl.render(info)
        if ext == "py": filename = output/"python/base.py"
        elif ext == "cpp": filename = output/"cpp/base.hpp"
        else:
            print(f"{Fore.RED}*** Invalid ext: {ext} ***{Fore.RESET}")
            sys.exit(1)

        write_file(filename, content)

    # create messages from each template ---------------
    path = pathlib.Path(".").absolute()
    for msgid, file in files.items():
        file = path/file
        msg_parts = tokenize(file)
        msg_parts.id = msgid
        msg_parts.license_notice = license
        msg_parts.namespace = namespace
        print(msg_parts)

        fmt = ""
        for v in msg_parts.fields:
            if v.array_size > 0:
                fmt += str(v.array_size) + var_types[v.type].fmt
            else:
                fmt += var_types[v.type].fmt

        var_types[file.stem] = VarInfo(file.stem+"_t", file.stem+"_t", msg_parts.msg_size,fmt,True)

        create_c_header(msg_parts, cppdir)
        create_python(msg_parts, pydir)

def run():
    parser = argparse.ArgumentParser(
        prog='ProgramName',
        description='What the program does',
        epilog='Text at the bottom of help')
    parser.add_argument('filename')
    parser.add_argument('-l', '--license', help='license', default="MIT")
    parser.add_argument('-n', '--namespace', help='namespace', default="yivo")
    parser.add_argument('-o', '--output', help='output location', default=".")
    args = vars(parser.parse_args())
    print(args)

    with open(args["filename"]) as fd:
        data = json.load(fd)

    if "license" not in data.keys():
        data["license"] = args["license"]

    if "output" not in data.keys():
        data["output"] = args["output"]

    if "namespace" not in data.keys():
        data["namespace"] = args["namespace"]

    main(data)

if __name__ == "__main__":
    info = {
        "namespace": "foobar",
        "license": "MIT Kevin Walchko (c) 2023",
        "output": "test",
        1: "messages/vec.yivo",
        2: "messages/quat.yivo",
        4: "messages/imu.yivo",
        5: "messages/cal.yivo"
    }
    main(info)
