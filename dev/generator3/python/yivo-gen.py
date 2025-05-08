#!/usr/bin/env python3
# import logging
from colorama import Fore
from msg_parts import dtypes
from tokenize import const_re, field_re
from pprint import pprint
from tokenize import parse_proto_file
# from format import FormaterC
from jinja2 import Environment
from jinja2.loaders import FileSystemLoader
import pathlib

tmp_dir = pathlib.Path(__file__).resolve().parent/"templates"
env = Environment(loader=FileSystemLoader(tmp_dir))

# logging: DEBUG ERROR INFO
# logging.basicConfig(format='>> %(message)s', level=logging.ERROR)
# logger = logging.getLogger(__name__)

# py_init = """
# # this file is auto generated
# from .base import fmt
# from .base import sizeof
# from .base import cls
# from .base import msg_id
# from .base import id2str
# """


def read_file(file_path):
    with open(file_path, 'r') as file:
        data = file.read()
    return data

def write_file(filename, content):
    with filename.open("w", encoding="utf-8") as fd:
        fd.write(content)
        print(f"Wrote File: {filename}")


def main():
  # file = read_file("msgs/geometry_msgs.proto")
  # file = read_file("msgs/test.proto")
  file = read_file("msgs/test2.proto")

  proto_dict = parse_proto_file(file)
  proto_dict["filename"] = "test_file"
  # pprint(proto_dict)
  # for m in proto_dict["messages"]:
  #   print(m)

  print("====================================")
  # fmr = FormaterC()
  # fmr.toC(proto_dict)


  src = env.get_template("msg.c.jinja")
  content = src.render(proto_dict)
  print(content)


  hdr = env.get_template("msg.h.jinja")
  content = hdr.render(proto_dict)
  print(content)

  py = env.get_template("msg.py.jinja")
  content = py.render(proto_dict)
  print(content)


if __name__ == "__main__":
  main()

# class Enums:
#     def __init__(self, name, size=None):
#         self.name = name
#         self.size = size
#         self.values = []

# def tokenize(file):
#     # Parse a template file into a dictionary of tokens
#     # that are processed into C++ or Python message headers
#     #
#     # regex
#     # ([\w]+) *([\w]+) *([#\w_ ]*)
#     #
#     # Everything is either:
#     # - comment: starts with #
#     # - include
#     # - enum: start "<enum" and end "enum>"
#     # - c function: start "<c" and end "c>"
#     # - python function: start "<p" and end "p>"
#     # - variable: <type> <name> <comment>
#     # - array: <type[N]> <name> <comment>

#     with file.open() as fd:
#         lines = fd.read()

#     mp = MsgParts()
#     mp.file = file
#     c_func_open = False
#     py_func_open = False
#     enum_open = False
#     enums = None
#     reg = re.compile(r'([\w\[\]]+) *([\w]+) *([#\w_ ]*)')

#     for line in lines.split('\n'):
#         line = line.rstrip().lstrip()
#         if len(line) == 0: continue

#         if line.find("<c") == 0:
#             c_func_open = True
#             continue
#         if line.find("c>") == 0:
#             c_func_open = False
#             continue

#         if c_func_open:
#             mp.c_funcs.append(line)
#             continue

#         if line.find("<p") == 0:
#             py_func_open = True
#             continue
#         if line.find("p>") == 0:
#             py_func_open = False
#             continue

#         if py_func_open:
#             mp.py_funcs.append(line)
#             continue

#         if line.find("<enum") == 0:
#             # <enum name size
#             toks = line.split(" ")
#             enums = Enums(toks[1].upper())
#             if len(toks) == 3: enums.size = toks[2]
#             enum_open = True
#             continue
#         if line.find("enum>") == 0:
#             enum_open = False
#             mp.enums.append(enums)
#             enums = None
#             continue
#         if enum_open:
#             enums.values.append(line.upper())
#             continue

#         if line.find("namespace") >=0:
#             mp.namespace = line.split(" ")[1]
#             continue

#         if line[0] == "#":
#             mp.comments.append(line)
#             continue

#         toks = reg.findall(line)[0]
#         logger.info(f"{toks}")

#         if toks[0] == "include":
#             mp.includes.append(toks[1])
#             continue

#         typ, var, comment = toks
#         if len(comment) == 0: comment = "# "

#         # find array brackets in typ
#         array_size = 0
#         left = typ.find('[')
#         right = typ.find(']')
#         if left > 0 and right > 0: # array found
#             array_size = int(typ[left+1:right])
#             typ = typ[:left]
#             mp.msg_size += var_types[typ].size*array_size
#             comment += f"size: {var_types[typ].size} * {array_size}"
#         else: # scalar
#             try:
#                 mp.msg_size += var_types[typ].size
#                 comment += f"size: {var_types[typ].size}"
#             except:
#                 print(f"{Fore.RED}*** Invalid keyword in line: {line} ***{Fore.RESET}")
#                 sys.exit(1)
#                 # continue

#         args = Field(typ, array_size, var, comment)
#         mp.fields.append(args)
#         # else:
#         #     logger.error(f"invalid line: {subline} -> {toks}")

#     return mp

# def main(files):
    # license = ""
    # if "license" in files.keys():
    #     license = files["license"]
    #     files.pop("license", None)

    # output = "."
    # if "output" in files.keys():
    #     output = files["output"]
    #     files.pop("output", None)
    # output = pathlib.Path(output)
    # cppdir = output/"cpp"
    # cppdir.mkdir(parents=True, exist_ok=True)
    # pydir = output/"python"
    # pydir.mkdir(parents=True, exist_ok=True)

    # namespace = None
    # if "namespace" in files.keys():
    #     namespace = files["namespace"]
    #     files.pop("namespace", None)


    # create the base headers for messages ------------
    # ids = {}
    # for k,v in files.items():
    #     f = pathlib.Path(v)
    #     ids[k] = f.stem

    # baseinfo = [
    #     ("base.py.jinja", ids, "py"),
    #     ("base.cpp.jinja", ids, "cpp"),
    # ]

    # for template, ids, ext in baseinfo:
    #     tmpl = env.get_template(template)
    #     info = {
    #         "license_notice": license,
    #         "ids": ids,
    #         "namespace": namespace
    #     }
    #     content = tmpl.render(info)
    #     if ext == "py": filename = output/"python/base.py"
    #     elif ext == "cpp": filename = output/"cpp/base.hpp"
    #     else:
    #         print(f"{Fore.RED}*** Invalid ext: {ext} ***{Fore.RESET}")
    #         sys.exit(1)

    #     write_file(filename, content)

    # create messages from each template ---------------
    # path = pathlib.Path(".").absolute()

    # write_file(pydir/"__init__.py", py_init)

    # for msgid, file in files.items():
    #     file = path/file
    #     msg_parts = tokenize(file)
    #     msg_parts.id = msgid
    #     msg_parts.license_notice = license
    #     msg_parts.namespace = namespace
    #     print(msg_parts)

    #     fmt = ""
    #     for v in msg_parts.fields:
    #         if v.array_size > 0:
    #             fmt += str(v.array_size) + var_types[v.type].fmt
    #         else:
    #             fmt += var_types[v.type].fmt

    #     var_types[file.stem] = VarInfo(file.stem+"_t", file.stem+"_t", msg_parts.msg_size,fmt,True)

    #     create_c_header(msg_parts, cppdir)
    #     create_python(msg_parts, pydir)

# def run():
#     parser = argparse.ArgumentParser(
#         prog='ProgramName',
#         description='What the program does',
#         epilog='Text at the bottom of help')
#     parser.add_argument('filename')
#     parser.add_argument('-l', '--license', help='license', default="MIT")
#     parser.add_argument('-n', '--namespace', help='namespace', default="yivo")
#     parser.add_argument('-o', '--output', help='output location', default=".")
#     args = vars(parser.parse_args())
#     print(args)

#     with open(args["filename"]) as fd:
#         data = json.load(fd)

#     if "license" not in data.keys():
#         data["license"] = args["license"]

#     if "output" not in data.keys():
#         data["output"] = args["output"]

#     if "namespace" not in data.keys():
#         data["namespace"] = args["namespace"]

#     main(data)

# if __name__ == "__main__":
#     info = {
#         "namespace": "foobar",
#         "license": "MIT Kevin Walchko (c) 2023",
#         "output": "test",
#         1: "messages/vec.yivo",
#         2: "messages/quat.yivo",
#         4: "messages/imu.yivo",
#         5: "messages/cal.yivo"
#     }
#     main(info)
