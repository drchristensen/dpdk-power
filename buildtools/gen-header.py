#! /usr/bin/env python3
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2023 Stephen Hemminger <stephen@networkplumber.org>

"""
Script to read a text file and convert it into a header file.
"""
import sys
import os


def main():
    '''program main function'''
    print(f'/* File autogenerated by {sys.argv[0]} */')
    for path in sys.argv[1:]:
        name = os.path.basename(path)
        print()
        print(f'/* generated from {name} */')
        with open(path, "r") as f:
            array = name.replace(".", "_")
            print(f'static const char {array}[] = ' + '{')
            line = f.readline()

            # make sure empty string is null terminated
            if not line:
                print('    ""')

            while line:
                s = repr(line)
                print('    {}'.format(s.replace("'", '"')))
                line = f.readline()
            print('};')


if __name__ == "__main__":
    main()