#!/bin/sh

cp ../interface.pl .
$CXX sample.cpp -g -std=c++11 -I../include -L../lib -lggpe -lboost_system-mt -lboost_timer-mt -lodbc -lgmp -lmysqlclient -lboost_filesystem-mt -lboost_regex-mt -lreadline -lYap

./a.out breakthrough.kif 1000
./a.out skirmish.kif 100
