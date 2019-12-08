#!/bin/sh
if [ $# -eq 1 ]; then
    cd ..
    cd ..
    mkdir build
    cd build
    cmake ..
    make
    mv 42sh ..
    cd ..
    mv 42sh src/test_suite
    cd src/test_suite
    virtualenv venv
    source venv/bin/activate
    pip install -r requirements.txt
else
    cd ..
    cd ..
    mkdir build
    cd build
    cmake ..
    make
    mv 42sh ..
    cd ..
    mv 42sh src/test_suite
    cd src/test_suite
    virtualenv venv
    ./venv/bin/activate
    pip install -r requirements.txt
    python3 testsuite.py 42sh
    deactivate
    rm 42sh
    rm -rf venv
    rm -rf ../../build
fi

