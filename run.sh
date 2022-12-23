#!/bin/bash

VAR=$(./test.sh)

if [ $? == 0 ]; then
  echo "Pass: Program exited zero"
  exit 0
else
  echo "Fail: Program did not exit zero"
  exit 1
fi