# !/bin/bash

#程序所做的工作是一键式编译，一键式链接，一键式运行，和一键式结果测试
#在bash中 source oneKeyBash.sh 此脚本，则其中的命令皆可用
#注意，更改程序目录时，只需更改 programPath 变量即可

export programPath=~/Huawei/huawei/
export code="${programPath}/SDK-gcc/future_net"
export case0="${programPath}/test-case/case0"
export case1="${programPath}/test-case/case1"
export case3="${programPath}/test-case/case3"
export case4="${programPath}/test-case/case4"
export include="${code}/lib"

alias f7="clear;g++ -c ${code}/route.cpp -o ${code}/route.o -I${include} -Wall"
alias wf7="g++ -c ${code}/route.cpp -o ${code}/route.o -I${include} -Wall 2> ${code}/error.txt; [ -s \"${code}/error.txt\" ] && vim ${code}/error.txt  || rm -f ${code}/error.txt"
alias runbuild="${code}/../batch.sh"
alias rungo0="${code}/../bin/future_net ${case0}/topo.csv ${case0}/demand.csv ${case0}/result.csv"
alias rungo1="${code}/../bin/future_net ${case1}/topo.csv ${case1}/demand.csv ${case1}/result.csv"
alias rungo3="${code}/../bin/future_net ${case3}/topo.csv ${case3}/demand.csv ${case3}/result.csv"
alias rungo4="${code}/../bin/future_net ${case4}/topo.csv ${case4}/demand.csv ${case4}/result.csv"
alias runtest0="${programPath}/test-program/rules_test_input ${case0}/topo.csv ${case0}/demand.csv ${case0}/result.csv"
alias runtest1="${programPath}/test-program/rules_test_input ${case1}/topo.csv ${case1}/demand.csv ${case1}/result.csv"
alias runtest3="${programPath}/test-program/rules_test_input ${case3}/topo.csv ${case3}/demand.csv ${case3}/result.csv"
alias runtest4="${programPath}/test-program/rules_test_input ${case4}/topo.csv ${case4}/demand.csv ${case4}/result.csv"
alias rundebug="gdb ${code}/../bin/future_net"
