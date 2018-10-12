# Andre Barrientos
# PID: 5443095
#
# Description: Makefile to compile and execute programs.
# 	Makefile complies two C programs. One in build1 and the
# 	other in build2. Label ALL executes these two builds.
#	Resets the directory for a clean compilation next time
#	around.

CC = gcc

#The First Program (validDates.c)
FILE_ONE = validDates.c
OUT_EXEC_ONE = validDates.out

#The Second Program (formatDates.c)
FILE_TWO = formatDates.c
OUT_EXEC_TWO = formatDates.out

#Executes both builds
all: build1 build2

#Sets up the compilation for program one (validDates.c)
build1:	$(FILE_ONE)
	$(CC) -o $(OUT_EXEC_ONE) $(FILE_ONE)

#Sets up the compulation for program two (formatDates.c)
build2:	$(FILE_TWO)
	$(CC) -o $(OUT_EXEC_TWO) $(FILE_TWO)

#'Cleans' the directory
clean:
	rm -f *.o core
