# echo 1 && echo aaa > a && sleep 3 && echo 1end
# echo 2 && echo bbb > b && sleep 4 && echo 2end
# echo 3 && sleep 3 && grep a < a
# echo 4 && sleep 3 && grep a < a && grep b < b
# true && echo good && echo good again || bad input
# false && echo bad || echo end test 1 
# sleep 2 && echo wakeup
# sleep 2 || echo wakeup
# #ls | echo hello && grep test
# ls | grep test | grep p
# ls > b
# grep test < b > c
# echo testtest | grep test < b
# ls && echo testtest | grep test
# (ls && echo testtest) | grep test
# (ls | grep test) && echo hello
# (false; true) || echo hello
# echo nefpre && false;echo hello&& echo last
# (echo hello && ls) > b
# gcc adfs || echo hello
# sort < b | grep p
# echo hi || (false ;echo hi2) && echo end
# echo hi && sleep 10 && echo bye
# echo begin && sleep 3 && echo end
