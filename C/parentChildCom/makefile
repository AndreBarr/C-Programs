# Andre Barrientos
# PID: 5443095
#
# Description: Makefile to compile and execute a program.
# 	Makefile complies a C program in build.
#	Resets the directory for a clean compilation next time
#	around.

CC = gcc

#The Program (parentChildCom.c)
FILE_ONE = parentChildCom.c
OUT_EXEC_ONE = parentChildCom.out

#Sets up the compilation for the program (parentChildCom.c)
build:	$(FILE_ONE)
	$(CC) -o $(OUT_EXEC_ONE) $(FILE_ONE)

#'Cleans' the directory
clean:
	rm -f *.o core
