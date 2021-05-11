#!/usr/bin/env python


import subprocess
import os
import sys

PIPE = subprocess.PIPE


def rmf(fname):
    try:
        os.unlink(fname)
    except OSError:
        pass

rmf("a.o")
rmf("a.bin")
rmf("buf.h")
p = subprocess.Popen("nasm -f elf64 -o a.o a.asm".split(), stdout=PIPE, stderr=PIPE)
res = p.communicate()
print(res)
assert len(res[1])==0
p = subprocess.Popen("objcopy -O binary a.o a.bin".split(), stdout=PIPE, stderr=PIPE)
print(p.communicate())
with open("a.bin", "rb") as fp:
    cc = fp.read()
tstr = b"\nunsigned char BUF[] = " + b'"%s"' % (b"".join(br"\x"+hex(c)[2:].encode("utf-8") for c in cc)) + b";\n"
tstr += b"\nunsigned long BUFLEN = " + str(len(cc)).encode("utf-8") + b";\n\n"
print(tstr.decode("utf-8"))
with open("buf.h", "wb") as fp:
    fp.write(tstr)

rmf("handler.o")
rmf("handler.bin")
rmf("handler.h")
p = subprocess.Popen("nasm -f elf64 -o handler.o handler.asm".split(), stdout=PIPE, stderr=PIPE)
res = p.communicate()
print(res)
assert len(res[1])==0
p = subprocess.Popen("objcopy -O binary handler.o handler.bin".split(), stdout=PIPE, stderr=PIPE)
print(p.communicate())
with open("handler.bin", "rb") as fp:
    cc = fp.read()
tstr = b"\nunsigned char HANDLERCODE[] = " + b'"%s"' % (b"".join(br"\x"+hex(c)[2:].encode("utf-8") for c in cc)) + b";\n"
tstr += b"\nunsigned long HANDLERCODELEN = " + str(len(cc)).encode("utf-8") + b";\n\n"
print(tstr.decode("utf-8"))
with open("handler.h", "wb") as fp:
    fp.write(tstr)

rmf("filter.raw")
rmf("filter_mod.raw")
rmf("filter.h")
p = subprocess.Popen("seccomp-tools asm -f raw -o filter.raw filter.seccomp".split(), stdout=PIPE, stderr=PIPE)
res = p.communicate()
print(res)
print(res[1].decode("utf-8"))
with open("filter.raw", "rb") as fp:
    cc = fp.read()
t = b"\x20\x00\x00\x00\x04"
r = cc.rfind(t)
ccn = cc[:r]+b"\x20\x00\x00\x00\x08"+cc[r+len(t):]
with open("filter_mod.raw", "wb") as fp:
    fp.write(ccn)
tstr = b"\nunsigned char FILTER[] = " + b'"%s"' % (b"".join(br"\x"+hex(c)[2:].encode("utf-8") for c in ccn)) + b";\n"
print(tstr.decode("utf-8"))
with open("filter.h", "wb") as fp:
    fp.write(tstr)


if len(sys.argv)>1 and sys.argv[1]=="ex":
    rmf("exploit")
    p = subprocess.Popen("./exploit.py exploit".split(), stdout=PIPE, stderr=PIPE)
    res = p.communicate()
    print(res)
    assert(os.path.exists("exploit"))


rmf("stub")

p = subprocess.Popen("gcc  -masm=intel  -O0 -Wl,-z,now stub.c -o stub".split(), stdout=PIPE, stderr=PIPE)
res = p.communicate()
print(res[0].decode("utf-8"))
print(res[1].decode("utf-8"))
p = subprocess.Popen("strip -s stub".split(), stdout=PIPE, stderr=PIPE)
res = p.communicate()







