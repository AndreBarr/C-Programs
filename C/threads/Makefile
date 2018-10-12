CC = gcc

FILE_ONE = threads.c
OUT_EXEC_ONE = threads.out

build: $(FILE_ONE)
	$(CC) -pthread -o $(OUT_EXEC_ONE) $(FILE_ONE)

clean:
	rm -f *.o core
