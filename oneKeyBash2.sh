#!/bin/bash

#程序所做的工作是一键式编译，一键式链接，一键式运行，和一键式结果测试
#在bash中 source oneKeyBash2.sh 此脚本，则其中的命令皆可用
#注意，更改程序目录时，只需更改 programPath 变量即可

function CompilerProgram ()
{
	g++ -c ${code}/route.cpp -o ${code}/route.o -I${include} -Wall 2>${code}/error.txt 
	if [ -s "${code}/error.txt" ]; then
		vim ${code}/error.txt
	else
		echo "No error"
		rm -f ${code}/error.txt
	fi
}

function RunProgram()
{
	tempCase="${case}$1"
	${code}/../bin/future_net ${tempCase}/topo.csv ${tempCase}/demand.csv ${tempCase}/result.csv
}

function RunTest()
{
	tempCase="${case}$1"
	${programPath}/test-program/rules_test_input ${tempCase}/topo.csv ${tempCase}/demand.csv ${tempCase}/result.csv
}


export programPath=~/Huawei/huawei/
export code="${programPath}/SDK-gcc/future_net"
export case="${programPath}/test-case/case"
export include="${code}/lib"

alias f7="clear;g++ -c ${code}/route.cpp -o ${code}/route.o -I${include} -Wall"
alias wf7="CompilerProgram"
alias runbuild="${code}/../batch.sh"
alias rungo="RunProgram"
alias runtest="RunTest"
alias rundebug="gdb ${code}/../bin/future_net"
