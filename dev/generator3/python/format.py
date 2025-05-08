from jinja2 import Environment
from jinja2.loaders import FileSystemLoader
import pathlib
from collections import namedtuple

# tmp_dir = pathlib.Path(__file__).resolve().parent/"templates"
# env = Environment(loader=FileSystemLoader(tmp_dir))

# c - C name
# py - python name
# size - size in bytes
# fmt - pack/unpack
# complex - for user defined types, more involved
# VarInfo = namedtuple("VarInfo","c py size fmt complex")

# var_types = {
#     "uint8": VarInfo("uint8_t", "int",1, "B", False),
#     "uint16": VarInfo("uint16_t", "int",2, "H", False),
#     "uint32": VarInfo("uint32_t", "int", 4, "I", False),
#     "uint64": VarInfo("uint64_t", "int",8, "Q", False),
#     "int8": VarInfo("int8_t", "int",1, "b", False),
#     "int16": VarInfo("int16_t", "int", 2, "h", False),
#     "int32": VarInfo("int32_t", "int", 4, "i", False),
#     "int64": VarInfo("int64_t", "int", 8, "q", False),
#     "float": VarInfo("float", "float", 4, "f", False),
#     "double": VarInfo("double", "float", 8, "d", False)
# }

# dtypes = {
#   "std": {
#     "uint8": VarInfo("uint8_t", "int",1, "B", False),
#     "uint16": VarInfo("uint16_t", "int",2, "H", False),
#     "uint32": VarInfo("uint32_t", "int", 4, "I", False),
#     "uint64": VarInfo("uint64_t", "int",8, "Q", False),
#     "int8": VarInfo("int8_t", "int",1, "b", False),
#     "int16": VarInfo("int16_t", "int", 2, "h", False),
#     "int32": VarInfo("int32_t", "int", 4, "i", False),
#     "int64": VarInfo("int64_t", "int", 8, "q", False),
#     "float": VarInfo("float", "float", 4, "f", False),
#     "double": VarInfo("double", "float", 8, "d", False)
#   }
# }

# def write_file(filename, content):
#     with filename.open("w", encoding="utf-8") as fd:
#         fd.write(content)
#         print(f"Wrote File: {filename}")

# def create_python(msg_parts, out_path):
#     comments = []
#     for c in msg_parts.comments:
#         comments.append(c)

#     func_args = []
#     for t, ars, v, c in msg_parts.fields:
#         func_args.append(v)

#     includes = []
#     for i in msg_parts.includes:
#         ii = f'from {i} import *'
#         includes.append(ii)

#     vars = []
#     for t, ars, v, c in msg_parts.fields:
#         line = f"{v}: {var_types[t].py}"
#         if c: line += f" {c}"
#         vars.append(line)

#     # print(">>", msg_parts.file.stem)

#     tmpl = env.get_template("msg.py.jinja")
#     info = {
#         "name": msg_parts.file.stem,
#         "vars": vars,
#         "includes": includes,
#         "msg_size": msg_parts.msg_size,
#         # "msg_size_type": "uint8_t",
#         "comments": comments,
#         "args": func_args,
#         "functions": msg_parts.py_funcs,
#         "enums": msg_parts.enums,
#         "format": var_types[ msg_parts.file.stem ].fmt,
#         "msgid": msg_parts.id,
#         "license_notice": msg_parts.license_notice
#     }
#     content = tmpl.render(info)

#     filename = msg_parts.file.stem + "_t.py"
#     filename = out_path/filename
#     write_file(filename, content)


# def create_c_header(msg_parts, out_path):
#     """
#     struct
#         - vars: string, "int bob[2];"

#     functions
#         - func_args: tuple(type, var_name), ("int","bob[2]")
#     """
#     comments = []
#     for c in msg_parts.comments:
#         c = c.replace("#", "//")
#         comments.append(c)

#     func_args = []
#     for t, ars, v, c in msg_parts.fields:
#         if var_types[t].complex: t = var_types[t].c + "&"
#         else: t = var_types[t].c
#         if ars > 0:
#             vv = f"{v}[{ars}]"
#         else:
#             vv = f"{v}"

#         func_args.append((t,vv,v))

#     includes = []
#     for i in msg_parts.includes:
#         ii = f'#include "{i}.h"'
#         includes.append(ii)

#     vars = []
#     for t, ars, v, c in msg_parts.fields:
#         if ars == 0: line = f"{var_types[t].c} {v};"
#         else: line = f"{var_types[t].c} {v}[{ars}];"
#         if c: line += f" {c.replace('#','//')}"
#         vars.append(line)

#     tmpl = env.get_template("msg.cpp.jinja")
#     info = {
#         "name": msg_parts.file.stem,
#         "vars": vars,
#         "includes": includes,
#         "msg_size": msg_parts.msg_size,
#         "msg_size_type": "uint8_t",
#         "comments": comments,
#         "args": func_args,
#         "functions": msg_parts.c_funcs,
#         "enums": msg_parts.enums,
#         "msgid": msg_parts.id,
#         "license_notice": msg_parts.license_notice,
#         "namespace": msg_parts.namespace
#     }
#     content = tmpl.render(info)

#     filename = msg_parts.file.stem + "_t.hpp"
#     filename = out_path/filename
#     write_file(filename, content)



#     # info = {
#     #     "name": msg_parts.file.stem,
#     #     "vars": vars,
#     #     "includes": includes,
#     #     "msg_size": msg_parts.msg_size,
#     #     "msg_size_type": "uint8_t",
#     #     "comments": comments,
#     #     "args": func_args,
#     #     "functions": msg_parts.c_funcs,
#     #     "enums": msg_parts.enums,
#     #     "msgid": msg_parts.id,
#     #     "license_notice": msg_parts.license_notice,
#     #     "namespace": msg_parts.namespace
#     # }

# class FormaterC :
#   extlibs = set()

#   def field(self, field): 
#     # if field.package not in ["std", None]:
#     #   self.ex_imports.append(field.package)

#     if field.comments is None:
#         comments = ""
#     else:
#         comments = f"// {field.comments}"

#     if field.default is None:
#         default = ""
#     else:
#         default = f" = {field.default}"
        
#     if field.array_size is None:
#         array = ""
#     else:
#         array = f"[{field.array_size}]"
    
#     return f"{field.dtype} {field.name}{array}{default}; {comments}"
  
#   def toC(self, data):

#     # info = {
#     #   # "name": msg_parts.file.stem,
#     #   "vars": vars,
#     #   "includes": includes,
#     #   "msg_size": msg_parts.msg_size,
#     #   "msg_size_type": "uint8_t",
#     #   "comments": comments,
#     #   "args": func_args,
#     #   "functions": msg_parts.c_funcs,
#     #   "enums": msg_parts.enums,
#     #   "msgid": msg_parts.id,
#     #   # "license_notice": msg_parts.license_notice
#     # }


#     info = {
#       "imports": [],
#       "enums": [],
#       "messages": []
#     }

#     if "enums" in data:
#       for e in data["enums"]:
#         print(e)

#     if "types" in data:
#       for t in data["types"]:
#         print(t)

#     if "messages" in data:
#       for msg in data["messages"]:
#         print(msg)
#         # self.extlibs.update(msg.extlibs)
#         # # print(msg)
#         # # print(f"{data["package"]}.{msg.name} {msg.id} ------")
#         # if msg.comments:
#         #   comm = [f"// {x}\n" for x in msg.comments]
#         #   print("".join(comm))
#         # for f in msg.fields:
#         #   # print(f"{f}")
#         #   print(self.field(f))
