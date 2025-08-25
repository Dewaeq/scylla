"""
use this script to compile the html file to a c++ header file, encoded as a list of bytes
"""

import gzip
import os
import sys
import minify_html


if os.path.exists("compiled/html.h"):
    os.remove("compiled/html.h")

if len(sys.argv) != 2:
    print("Please enter the path to the html file")
    exit(-1)

input_path = sys.argv[1]

if not os.path.exists(input_path):
    print("Input file does not exist!")
    exit(-1)

with open(input_path, "rb") as input_file:
    data = input_file.read()
    minified = minify_html.minify(data.decode())
    out = gzip.compress(minified.encode())
    text = out.hex(" ")

    if not os.path.exists("compiled"):
        os.makedirs("compiled")
        
    input_file_name = os.path.basename(input_path).split(".")[0]
    with open(f"compiled/{input_file_name}_html.h", "w+") as output_txt:
        entries = text.split(" ")
        size = len(entries)

        output_txt.write(f"#ifndef {input_file_name.upper()}_HTML_H\n")
        output_txt.write(f"#define {input_file_name.upper()}_HTML_H\n\n")
        output_txt.write("#include <Arduino.h>\n\n")
        output_txt.write(f"#define {input_file_name.upper()}_HTML_SIZE {size}\n\n")
        output_txt.write(f"static const uint8_t {input_file_name}_html[] PROGMEM = {{")

        for i, val in enumerate(entries):
            buffer = ""
            if i % 50 == 0:
                buffer += "\n"
            buffer += "0x" + val
            if i < size:
                buffer += ","
            output_txt.write(buffer)

        output_txt.write("\n};\n\n")
        output_txt.write("#endif")

