#!/usr/bin/env python

import sys
import socket
import subprocess
import os
PIPE = subprocess.PIPE


def test_input(tinput, expected, expected_end=None):
    if len(sys.argv)>2:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((IP, PORT))
        s.sendall(tinput)
        buf = b""
        while(True):
            try:
                c = s.recv(1)
            except ConnectionResetError:
                break
            if c == b"":
                break
            buf += c
        s.close()
    else:
        p = subprocess.Popen("./stub", stdout=PIPE, stdin=PIPE, stderr=PIPE)
        res = p.communicate(tinput)
        buf = res[0]
    print("OUTPUT:", repr(buf))
    assert(buf.startswith(expected))
    if expected_end:
        sbuf = buf
        sbuf = sbuf.replace(b"\x00",b"")
        sbuf = sbuf.strip()
        assert(sbuf.endswith(expected_end))


assert len(sys.argv)>1

if len(sys.argv)>2:
    IP = sys.argv[2]
    PORT = int(sys.argv[3])

print("="*3, sys.argv[1])
if sys.argv[1] == "ex" or sys.argv[1] == "all":
    with open("exploit", "rb") as fp:
        cc = fp.read()
    test_input(cc, b"Give me some code!\nThe length of the first instruction is:\n"+b"OOO{", expected_end=b"}")

if sys.argv[1] == "exr":
    try:
        os.unlink("exploit")
    except OSError:
        pass
    p = subprocess.Popen(["./exploit.py", "exploit"], stdout=PIPE, stdin=PIPE, stderr=PIPE)
    res = p.communicate()
    print(res[0].decode("utf-8"))
    print(res[1].decode("utf-8"))
    with open("exploit", "rb") as fp:
        cc = fp.read()
    test_input(cc, b"Give me some code!\nThe length of the first instruction is:\n"+b"OOO{", expected_end=b"}")

if sys.argv[1] == "b1" or sys.argv[1] == "all":
    test_input(b"ax\n", b'Give me some code!\ninvalid hex\n')

if sys.argv[1] == "b2" or sys.argv[1] == "all":
    test_input(b"ebfe\n", b'Give me some code!\nThe length of the first instruction is:\n0\n')

if sys.argv[1] == "b3" or sys.argv[1] == "all":
    test_input(b"cc\n", b'Give me some code!\nThe length of the first instruction is:\n')

if sys.argv[1] == "b4" or sys.argv[1] == "all":
    test_input(b"48BC77665544332211FF\n", b'Give me some code!\nThe length of the first instruction is:\n000000000000000a\n')

