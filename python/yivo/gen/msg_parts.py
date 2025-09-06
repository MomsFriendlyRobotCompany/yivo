###############################################
# The MIT License (MIT)
# Copyright (c) 2020 Kevin Walchko
# see LICENSE for full details
##############################################
from collections import namedtuple
from colorama import Fore
import struct
import re

VarInfo = namedtuple("VarInfo","c py size fmt complex")

array_re = re.compile(r'\s*\{([\d.,\-eE\s]+)?\}\s*')

dtypes = {
    "std": {
        "uint8": VarInfo("uint8_t", "int",1, "B", False),
        "uint16": VarInfo("uint16_t", "int",2, "H", False),
        "uint32": VarInfo("uint32_t", "int", 4, "I", False),
        "uint64": VarInfo("uint64_t", "int",8, "Q", False),
        "int8": VarInfo("int8_t", "int",1, "b", False),
        "int16": VarInfo("int16_t", "int", 2, "h", False),
        "int32": VarInfo("int32_t", "int", 4, "i", False),
        "int64": VarInfo("int64_t", "int", 8, "q", False),
        "float": VarInfo("float", "float", 4, "f", False),
        "double": VarInfo("double", "float", 8, "d", False),
        "char": VarInfo("char", "int",1, "c", False),
        "bool": VarInfo("bool", "bool",1, "?", False),
    }
}

class EnumField:
  def __init__(self, name, value, comment):
    self.name = name
    self.value = value
    self.comment = comment

  def __str__(self):
    return f"{self.name} = {self.value} // {self.comment}"

  def __repr__(self):
    return str(self)

class Enumeration:
  def __init__(self, name):
    self.name = name
    self.fields = []
    self.comments = []

  def __str__(self):
    ret = []
    ret.append(f"Enum: {self.name}")
    ret.append(f"comments: {self.comments}")
    for f in self.fields:
      ret.append(str(f))
    ret.append(" ")

    return "\n".join(ret)

  def __repr__(self):
    return str(self)

class Define:
  def __init__(self, name, value, comment):
    self.name = name
    self.value = value
    self.comment = comment

  def __str__(self):
    return f"define {self.name} = {self.value}; // {self.comment}"

  def __repr__(self):
    return str(self)


class Field:
    
    def __init__(self, package, dtype, name, array_size, default, comments):
    
        if package is not None:
            package = package.replace(".","")
        else:
            package = "std"

        if (package not in dtypes) or (dtype not in dtypes[package]):
            print("\n\n")
            for k,v in dtypes.items():
                print(k)
                for kk,vv in v.items():
                    print(f" - {kk}: {vv}")
            print("\n\n")
            raise Exception(f"Unknown: {package} or {dtype}")
            
        self.package = package
        self.dtype = dtype
        self.ctype = dtypes[package][dtype].c
        # self.pytype = dtypes[package][dtype].py
        self.name = name

        if dtypes[package][dtype].py.find("_e") > 0:
            self.pytype = "int"
        else:
            self.pytype = dtypes[package][dtype].py

        # print(f">> {self.pytype}")

        if array_size is None:
          self.array_size = array_size
        else:
          self.array_size = int(array_size)

        if (array_size is not None) and (default is not None):
            self.default = self.get_array(default, dtypes[package][dtype])
        elif (dtype == "char") and (default is not None):
            self.array_size = len(default)
            self.default = default
        elif (dtype == "char") and (array_size is None):
            raise Exception("char data type needs to define array size")
        elif default:
            self.default = default
        else:
            self.default = None
                
        self.comments = comments

    def get_bool(self, value):
        value = value.lower()
        if value == "true": return True
        elif value == "false": return False
        raise Exception("Invalid value: {value}")

    def get_array(self, string, atype):
        if atype != float and atype != int:
            return []
        try:
            # print(type(string))
            a = array_re.match(string)
            # print(a,flush=True)
            g = a.group(1)
            nums = g.split(',')
            # print(nums)
            array = []
            for n in nums:
                array.append(atype(n))
            return array
        except:
            print(f"{string} filed get array")
            return []

    def __repr__(self):
      return str(self)

    def __str__(self):
        if self.comments is None:
            comments = ""
        else:
            comments = f" {Fore.GREEN}// {self.comments}{Fore.RESET}"

        if self.default is None:
            default = ""
        else:
            default = f" = {Fore.LIGHTWHITE_EX}{self.default}{Fore.RESET}"
            
        if self.array_size is None:
            array = ""
        else:
            array = f"{Fore.YELLOW}[{self.array_size}]{Fore.RESET}"
        
        return f"{Fore.CYAN}{self.package}.{Fore.BLUE}{self.dtype}{Fore.RESET} {self.name}{array}{default};{comments}"


class Message:

  def __init__(self, name, msg_id=0):
    self.fields = []
    self.constants = []
    self.id = int(msg_id)
    self.name = name
    self.size = 0
    self.comments = []
    self.extlibs = set()
    self.fmt = "<"

  def __repr__(self):
    return str(self)

  def calcsize(self):
    # print(self.fmt)
    self.size = struct.calcsize(self.fmt)

  def append_field(self, field):
    if field.package != "std":
      self.extlibs.add(field.package)

    fmt = dtypes[field.package][field.dtype].fmt
    if field.array_size is not None:
      self.fmt += f"{field.array_size}{fmt}"
      # package, dtype, name, array_size, default, comments
      name = f"{self.name}_{field.name}_SIZE".upper()
      self.constants.append(Field("std","uint32",name,None,field.array_size,f"{field.name} array size"))
    else:
      self.fmt += fmt

    self.fields.append(field)

  def __str__(self):
    ret = []
    if self.id > 0:
      ret.append(f"{self.name} [{self.id}] -----------------")
    else:
      ret.append(f"{self.name} -----------------------------")
    ret.append(f"  size: {self.size} format: {self.fmt}")
    ret.append(f"  extlibs: {self.extlibs}")
    ret.append(f"  constants: {self.constants}")
    ret.append(f"  comments: {self.comments}")
    for f in self.fields:
      ret.append(f"  {str(f)}")
    ret.append(" ")

    return "\n".join(ret)








