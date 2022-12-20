#!/usr/bin/expect
set timeout 2
spawn ./a.out 
expect -- "msh>"
send \"ls\r\"
expect {
  \"^msh  test_script\" { exit 0 }
  default {  exit 1 }
}
exit 1