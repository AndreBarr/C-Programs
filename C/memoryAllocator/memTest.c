#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "mem.h"

#define ERROR NULL
#define NO_ERROR 0

void* ptr;

int Mem_Init(int size, int policy)
{
	int pageSize = getpagesize();
	int roundedSize = (int) ceil((double) size / pageSize) * pageSize;

	int fd = open("/dev/zero", O_RDWR);
	void* addrPtr = mmap(NULL, roundedSize, PROT_READ| PROT_WRITE, MAP_PRIVATE, fd, 0);

	if (addrPtr == MAP_FAILED)
	{
		perror("mmap");
		return ERROR;
	}
	else
	{
		return NO_ERROR;
	}
}

void* Mem_Alloc(int size)
{
	return NULL;
}

int Mem_Free(void* ptr)
{
	return 1;
}

int Mem_IsValid(void* ptr)
{
	return 1;
}

int Mem_GetSize(void* ptr)
{
	return 1;
}

float Mem_GetFragmentation()
{
	return 1.0;
}
