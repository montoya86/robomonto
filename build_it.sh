#!/usr/bin/env bash

export ROBOMONGO_CMAKE_PREFIX_PATH="/home/jmontoya/devel/Qt/5.9.3/gcc_64/;/home/jmontoya/CLionProjects/robomongo/robomongo-shell;/home/jmontoya/CLionProjects/anotheropenssl/openssl-1.0.1u"

./bin/clean
./bin/configure
./bin/pack
./bin/install
#./bin/run