#!/bin/bash

(
    cd src
    ./compile.py ex
)


rm public_bundle.tar.gz

rm interaction/test.py
rm interaction/exploit
cp src/test.py interaction/
cp src/exploit interaction/

rm service/stub
rm service/flag
cp src/stub service/
cp src/flag service/

rm stub
cp src/stub stub
