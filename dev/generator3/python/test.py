# #!/usr/bin/env python3
# from msg_parts import dtypes
# from tokenize import const_re, field_re
# from pprint import pprint
# from tokenize import parse_proto_file
# # from format import FormaterC
# from jinja2 import Environment
# from jinja2.loaders import FileSystemLoader
# import pathlib

# tmp_dir = pathlib.Path(__file__).resolve().parent/"templates"
# env = Environment(loader=FileSystemLoader(tmp_dir))

# # data = [
# #     "constant char bob[] = \"tom12 . 34/#@\"; // frame name in poland 12",
# #     "constant uint32 tom = 32;",
# #     "constant int8 bob; // this is a test",
# #     "constant kevin12.Vector3 package_tom = 3; // hi",
# #     "constant float sam[3] = [1.2,0.003,-4.5];",
# #     # "constant char bob;",
# #     "constant bool tom = false;",
# # ]

# # for d in data:
# #     matches = const_re.match(d)
# #     if matches is not None:
# #         m = matches.groups()[0]
# #         # print(m)
# #         f = field_re.match(m).groups()
# #         # print(f)
# #         f = Field(*f)
# #         print(f)
# #         # print(f"found {len(m)}: {m}")

# inline = """
# package kevin;

# import "geometric_msgs";
# import "how/now/brown/cow.proto";

# define bob = 22; // define test
# define sally = bob / M_PI * 34.0; // equation

# constant uint8 bob = 33; // const test

# enum BatteryHealth {
#   // Power supply health constants
#   unknown = 0;
#   GOOD = 1;
#   OVERHEAT = 2;
#   DEAD = 3;
#   OVERVOLTAGE = 4;
#   UNSPEC_FAILURE = 5;
#   COLD = 6;
#   WATCHDOG_TIMER_EXPIRE = 7;
#   SAFETY_TIMER_EXPIRE = 8;
# }

# enum BATTERY_TECHNOLOGY {
#   // Power supply technology (chemistry) constants
#   // add some other comments
#   // and this too
#   UNKNOWN = 0; // Unknown battery technology
#   NIMH = -1;    // Nickel-Metal Hydride battery
#   LION = -2;    // Lithium-ion battery
#   LIPO = 3;    // Lithium Polymer battery
#   LIFE = 4;    // Lithium Iron Phosphate battery
#   NICD = 5;    // Nickel-Cadmium battery
#   LIMN = 6;    // Lithium Manganese Dioxide battery
#   TERNARY = 7; // Ternary Lithium battery
#   VRLA = 8;    // Valve Regulated Lead-Acid battery
# }


# type vector2_t {
#   float x = 0;
#   float y = 0;
# }

# type vector3_t {
#   float x = 0;
#   float y = 0;
#   float z = 0;
# }

# type quaternion_t {
#   float w = 1;
#   float x = 0;
#   float y = 0;
#   float z = 0;
# }

# type timestamp_t {
#   // Represents seconds of UTC time since Unix epoch
#   // Non-negative fractions of a second at nanosecond resolution. Negative
#   // second values with fractions must still have non-negative nanos values
#   // that count forward in time. Must be from 0 to 999,999,999
#   // inclusive.
#   uint64 seconds;
#   uint32 nanos;
# }

# message bob_t:102 {
#   uint8 something;
#   uint32 else = 9;
# }

# message dummy_t: 99 {
#   float a[10] = {1,2,3,4,5,6,7,8,9,0};
#   float b = -1E-99;
#   float c; // c
#   kevin.vector3_t d; // test
#   kevin.quaternion_t q; // orientation
#   bool f;
# }
# """

# proto_dict = parse_proto_file(inline)
# proto_dict["filename"] = "test_file"
# pprint(proto_dict)
# # for m in proto_dict["messages"]:
# #   print(m)

# print("====================================")
# # fmr = FormaterC()
# # fmr.toC(proto_dict)


# src = env.get_template("msg.c.jinja")
# content = src.render(proto_dict)
# print(content)


# hdr = env.get_template("msg.h.jinja")
# content = hdr.render(proto_dict)
# print(content)

# pprint(dtypes)

