#!/bin/bash

test_data=./test_data/random_pattern.txt
time ./SW.out -data_path $test_data -data_num 100 > exe_SW.log
