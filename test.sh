#!/usr/bin/expect 
set timeout 2
spawn ./a.out 
expect -- "msh>"
send "ls\r"
expect {
  "^ ls\r\na.out  LICENSE\tMakefile  msh.c  README.md  test.sh  Useful-Examples" { exit 0 }
  default {  exit 1 }
}
exit 1