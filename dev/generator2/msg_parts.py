
from colorama import Fore


class MsgParts:
    """
    Breaks a message format appart and stores the results so it can be
    converted into other languages. Supported languages:
    - python
    - C/C++
    """
    def __init__(self):
        self.comments = []  # comments in body of message prototype
        self.fields = []    # variables in message
        self.includes = []  # included message headers/modules
        self.c_funcs = []   # custom C functions
        self.py_funcs = []  # custom Python functions
        self.enums = []     # enums
        self.msg_size = 0   # size of message in bytes
        self.file = None    # filename for naming the message
        self.id = 0         # message id number
        self.namespace = None # cpp namespace

    def __repr__(self):
        return str(self)

    def __str__(self):
        ret = f"{Fore.YELLOW}------------------------------\n"
        ret += f"File: {self.file}\n"
        if self.namespace is not None:
            ret += f"Namespace: {self.namespace}\n"
        ret += f"------------------------------\n{Fore.RESET}"
        ret += f"{Fore.CYAN}Comments:\n{Fore.RESET}"
        ret += f"{Fore.GREEN}"
        for c in self.comments:
            ret += f" {c}\n"
        ret += f"{Fore.RESET}"

        ret += f"\n{Fore.CYAN}Fields:\n{Fore.RESET}"
        for f in self.fields:
            ret += f" {f}\n"

        ret += f"\n{Fore.CYAN}Python Functions:\n{Fore.RESET}"
        for f in self.py_funcs:
            ret += f" {f}\n"

        ret += f"\n{Fore.CYAN}C Functions:\n{Fore.RESET}"
        for f in self.c_funcs:
            ret += f" {f}\n"

        ret += f"\n{Fore.CYAN}Includes:\n{Fore.RESET}"
        ret += f"{Fore.BLUE}"
        for i in self.includes:
            ret += f" {i}\n"
        ret += f"{Fore.RESET}\n"

        ret += f"\n{Fore.CYAN}Enums:\n{Fore.RESET}"
        for f in self.enums:
            ret += f" {f}\n"

        ret += f"{Fore.CYAN}\nMessage Size:{Fore.RESET} {self.msg_size}\n"
        return ret
