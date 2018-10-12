#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define PTHREAD_SYNC 1

int SharedVariable = 0;

#ifdef PTHREAD_SYNC
	pthread_barrier_t barr;
	pthread_mutex_t lock;
#endif

/**
* Will validate command-line input. Value must be greater than zero.
* @param argc is the number of command-line inputs
* @param *argv[] is the actual values of the command-line input
* @return if the input values are valid
*/
int validate(int argc, char *argv[])
{
	int isValid = 0;
	int numThreads = 0;

	if (argc != 2)
	{
		printf("%s\n", "Proper usage: ./threads.out numThreads");
	}
	else
	{
		//Position 0 holds the name of the file which we do not need
		numThreads = atoi(argv[1]);
		if (numThreads > 0)
		{
			isValid = 1;
		}
		else
		{
			printf("%s\n", "Value must be a positive integer.");
		}
	}
	return isValid;
}

void *SimpleThread(void *which) {
	int taskId = (long int) which;
	int num, val;
	for (num = 0; num<20; num++) {
		if (random() > RAND_MAX / 2) {
			usleep(500);
		}		
		#ifdef PTHREAD_SYNC
			pthread_mutex_lock(&lock);
		#endif

		val = SharedVariable;
		printf("*** thread %d sees value %d\n", taskId, val);
		SharedVariable = val + 1;

		#ifdef PTHREAD_SYNC
			pthread_mutex_unlock(&lock);
		#endif	
	}

	#ifdef PTHREAD_SYNC
		//the function returns the constant PTHREAD_BARRIER_SERIAL_THREAD to one thread
		//and 0 to the remaining threads
		int ret = pthread_barrier_wait(&barr);
		if (ret != 0 && ret != PTHREAD_BARRIER_SERIAL_THREAD) {
			printf("Could not wait on barrier.\n");
			exit(-1);
		}
	#endif

	val = SharedVariable;
	printf("Thread %d sees final value %d\n", taskId, val);

	pthread_exit(0);
}



int  main(int argc, char *argv[])
{
	if (validate(argc, argv)) {
		int numThreads = atoi(argv[1]);

		#ifdef PTHREAD_SYNC
			//returns 0 if successfull
			if (pthread_barrier_init(&barr, NULL, numThreads)) {
				printf("Could not create a barrier.\n");
				return -1;
			}
		#endif

		pthread_t tid[numThreads];
		int err;
		int threads = 0;
		for (;threads < numThreads; threads++) {
			//Had to cast to long int sinc void* is 64bit and in is 32bit
			err = pthread_create(&tid[threads], NULL, SimpleThread, (void *) (long int) threads);
			if (err != 0) {
				printf("\nThread can't be created :[%s]", strerror(err));
			}
		}

		int joinThread = 0;
		for (; joinThread < numThreads; joinThread++) {
			pthread_join(tid[joinThread], NULL);
		}
	}

	return 1;
}
