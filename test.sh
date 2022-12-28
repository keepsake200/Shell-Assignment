#!/usr/bin/expect -d
set timeout 1
spawn ./msh
expect -- "msh>"
send "ls -v\r"
expect {
  "^ ls -v\r\nLICENSE  Makefile  README.md  Useful-Examples  msh  msh.c  run.sh  test.sh\r\nmsh>" { exit 0 }
  default {  exit 1 }
}
exit 1

