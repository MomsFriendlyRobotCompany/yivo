###############################################
# The MIT License (MIT)
# Copyright (c) 2020 Kevin Walchko
# see LICENSE for full details
##############################################
import re
from colorama import Fore
from enum import Enum
from .msg_parts import Field, Message, EnumField, Enumeration, Define
from .msg_parts import dtypes, VarInfo


class State(Enum):
  none = 0
  type = 1
  enum = 2
  message = 3

# Regular expressions for parsing
define_re = re.compile(r'define\s+(.+)\s*=\s*(.+)\s*;\s*(?:\/\/\s*(.*))?')
package_re = re.compile(r'package\s+([\w.]+)\s*;')
import_re = re.compile(r'import\s+"([^"]+)"\s*;')
# message_start_re = re.compile(r'message\s+(\w+)\s*:\s*(\d+)\s*{')
message_start_re = re.compile(r'message\s+(\w+)\s*{')
enum_start_re = re.compile(r'enum\s+(\w+)\s*{')
type_start_re = re.compile(r"type\s+(\w+)\s*{")
field_re = re.compile(r"\s*(\w+\.)?([\w]+)\s+(\w+)\[?(\d+)?\]?\s*=?\s*(.+)?\s*;\s*(?:\/\/\s*(.*))?")
const_re = re.compile(r"constant\s+(.+)")
enum_value_re = re.compile(r'\s*(\w+)\s*=\s*(-?\d+)\s*;\s*(?:\/\/\s*(.*))?')
comment_re = re.compile(r'^\s*//\s*(.*)')


def parse_proto_file(file, verbose=False):
    # Initialize the dictionary to store the parsed contents
    proto_dict = {
      "package": "",
      "imports": set(),
      "messages": [],
      "enums": [],
      "types": [],
      "defines": [],
      "constants": [],
    }

    state = State.none

    for lineno, line in enumerate(file.split('\n')):
        if verbose:
            print(f"{lineno+1}: {line}")

        line = line.strip()

        # Skip empty lines
        if not line:
            continue

        # Check for package
        package_match = package_re.match(line)
        if package_match:
            pkg = package_match.group(1)
            proto_dict["package"] = pkg

            if pkg not in dtypes:
                dtypes[pkg] = {}
                continue

        # Check for define (var, value, comment)
        define_match = define_re.match(line)
        if define_match:
            name = define_match.group(1)
            value = define_match.group(2)
            comment = define_match.group(3)
            define = Define(name, value, comment)
            proto_dict["defines"].append(define)
            continue
        
        # Check for define (package, name, array_size, default, comment)
        const_match = const_re.match(line)
        if const_match:
            field_match = field_re.match(const_match.group(1))
            if field_match:
                f = field_match.groups()
                # print(f)
                f = Field(*f)
                # print(f)
                proto_dict["constants"].append(f)
            continue

        # Check for imports
        import_match = import_re.match(line)
        if import_match:
            proto_dict["imports"].add(import_match.group(1))
            continue

        # Check for message start
        message_match = message_start_re.match(line)
        if message_match:
            name = message_match.group(1)
            # id = message_match.group(2)
            # msg = Message(name, id)
            msg = Message(name)
            proto_dict["messages"].append(msg)
            state = State.message
            continue

        # Check for enum start
        enum_match = enum_start_re.match(line)
        if enum_match:
            name = enum_match.group(1)
            proto_dict["enums"].append(Enumeration(name))
            state = State.enum
            continue

        type_match = type_start_re.match(line)
        if type_match:
            current_type = type_match.group(1)
            msg = Message(current_type)
            proto_dict["messages"].append(msg)
            state = State.type
            continue

        # Check for message/type or enum end
        if line == '}' and (state != State.none):
            if state == State.type:
              pkg = proto_dict["package"]
              newType = proto_dict["messages"][-1]
              msgname = newType.name
              dtypes[pkg][msgname] = VarInfo(
                msgname.lower(),
                msgname,
                newType.size,
                newType.fmt.replace('<',''),
                True)
            elif state == State.enum:
                # pass
                pkg = proto_dict["package"]
                newType = proto_dict["enums"][-1]
                msgname = newType.name
                dtypes[pkg][msgname] = VarInfo(
                    msgname.lower(),
                    msgname,
                    1,
                    "i",
                    True)

            state = State.none
            continue

        # Check for comments
        comment_match = comment_re.match(line)
        if comment_match:
          comm = comment_match.group(1)
          if state == State.message:
            proto_dict["messages"][-1].comments.append(comm)
          elif state == State.type:
            proto_dict["types"][-1].comments.append(comm)
          elif state == State.enum:
            proto_dict["enums"][-1].comments.append(comm)
          continue

        # Check for fields inside a message
        if state == State.message:
            field_match = field_re.match(line)
            if field_match:
                f = field_match.groups()
                # print(f)
                if f[2] == "id":
                    proto_dict["messages"][-1].id = int(f[4])
                    # print(proto_dict["messages"][-1].id)
                else:
                    f = Field(*f)
                    # print(f)
                    proto_dict["messages"][-1].append_field(f)

            else:
                print(f"{Fore.RED}ERROR, invalid line[{lineno+1}]: {line}{Fore.RESET}")

        if state == State.type:
            field_match = field_re.match(line)
            if field_match:
                f = field_match.groups()
                # print(f)
                f = Field(*f)
                # print(f)
                proto_dict["messages"][-1].append_field(f)

            else:
                print(f"{Fore.RED}ERROR, invalid line[{lineno+1}]: {line}{Fore.RESET}")

        # Check for enum values inside an enum
        if state == State.enum:
            enum_value_match = enum_value_re.match(line)
            if enum_value_match:
                name = enum_value_match.group(1)
                value = int(enum_value_match.group(2))
                comment = enum_value_match.group(3)

                f = EnumField(name, value, comment)
                # proto_dict["enums"][-1]["values"].append(f)
                proto_dict["enums"][-1].fields.append(f)

    for m in proto_dict["messages"]:
      m.calcsize()
      m.fmt = combine_consecutive(m.fmt)

    # for t in proto_dict["types"]:
    #   t.calcsize()

    if verbose:
        print("parse_proto_file:")
        for k,v in proto_dict.items():
            print(k)
            if isinstance(v, list):
                for i in v: print(f"{i}")
            else: print(f"{v}")
            

    return proto_dict

def combine_consecutive(s):
    if not s:
        return ""
    
    result = []
    count = 1
    current_char = s[0]
    
    for i in range(1, len(s)):
        if s[i] == current_char:
            count += 1
        else:
            if count > 1:
                result.append(str(count))
            result.append(current_char)
            current_char = s[i]
            count = 1
    
    # Append the last character and its count (if > 1)
    if count > 1:
        result.append(str(count))
    result.append(current_char)
    
    return "".join(result)