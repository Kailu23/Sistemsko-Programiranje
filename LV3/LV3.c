#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#define N 1024
#define NTHRD 16

#ifdef _WIN32
#define FETCHTIME() clock()
#else
#include <sys/times.h>
#include <sys/types.h>
#include <unistd.h>
#define FETCHTIME() times(NULL)
#endif

int clocks_per_sec;


typedef struct _matrixWorkOrder {
	float* matrix;
	int low;
	int high;

	float* previousColumnAverage;
	float partialColumnSum;

	int canContinue;
	int* finishedThreads;
	pthread_cond_t* workerCondition;
	pthread_cond_t* masterCondition;
	pthread_mutex_t* conditionMutex;
} matrixWorkOrder;


void printMatrix(float* matrix, int low, int high) {
	for (int i = low; i <= high; i++) {
		for (int j = 0; j < N; j++) {
			printf("%f ", matrix[i * N + j]);
			//printf("%f ", *(matrix + i*N + j));
		}
		printf("\n");
	}
}

float firstFormulae(int i, int j) {
	float sum = 0;
	float value;

	for (int k = 0; k <= i; k++) {
		value = k * sin(j) - j * cos(k);
		sum += value;
	}

	return sum;
}

void sequentialFill(float* matrix, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			matrix[i * N + j] = firstFormulae(i, j);
		}
	}
}

float sequentialSum(float* matrix, int size) {
	float sum = 0;

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			sum += matrix[i * N + j];
		}
	}
	return sum;
}

void multithreadedSegment(float* matrix, int low, int high) {
	for (int i = low; i <= high; i++) {
		for (int j = 0; j < N; j++) {
			matrix[i * N + j] = firstFormulae(i, j);
		}
	}
	//printf("\n\n");
	//printMatrix(matrix, low, high);
	//printf("\n\n");
}

//For problem 2

void sequentialFill2(float mat[N * N]) {
	float previousColumnAverage = 0;
	for (int j = 0; j < N; j++) {
		float columnSum = 0;
		for (int i = 0; i < N; i++) {
			float value = firstFormulae(i, j);
			mat[i * N + j] = previousColumnAverage + value;
			columnSum += mat[i * N + j];
		}
		previousColumnAverage = columnSum / N;
	}
}

void multithreadedSegment2(void* workOrder) {
	matrixWorkOrder* order = (matrixWorkOrder*)workOrder;

	for (int j = 0; j < N; j++) {
		order->partialColumnSum = 0;
		for (int i = order->low; i <= order->high; i++) {
			order->matrix[i * N + j] = *order->previousColumnAverage + firstFormulae(i, j);
			order->partialColumnSum += order->matrix[i * N + j];
		}
		pthread_mutex_lock(order->conditionMutex);
		*order->finishedThreads += 1;
		order->canContinue = 0;
		pthread_cond_signal(order->masterCondition);
		while (!order->canContinue) {
			pthread_cond_wait(order->workerCondition, order->conditionMutex);
		}
		pthread_mutex_unlock(order->conditionMutex);
	}
}

void multithreaded(void* workOrder) {
	matrixWorkOrder* order = (matrixWorkOrder*)workOrder;

	multithreadedSegment(order->matrix, order->low, order->high);
}

int main() {
    #ifdef _WIN32
	    clocks_per_sec = CLOCKS_PER_SEC;
    #else
	    clocks_per_sec = sysconf(_SC_CLK_TCK);
    #endif
	float* A = (float*)malloc(N * N * sizeof(float));
	if (A == NULL) {
		printf("Failed to allocate matrix A\n");
		exit(EXIT_FAILURE);
	}
	float* B = (float*)malloc(N * N * sizeof(float));
	if (B == NULL) {
		printf("Failed to allocate matrix B\n");
		exit(EXIT_FAILURE);
	}
	clock_t start, end;
	float seqSum = 0, threadedSum = 0;

	pthread_t workers[NTHRD];
	matrixWorkOrder workOrder[NTHRD];

	int  segmentSize = (int)ceil((double)N / NTHRD);

	//Zad 1 i  3
	printf("Starting sequential...\n");
	start = FETCHTIME();

	sequentialFill((float*)A, N);
	//printMatrix((float*)A, 0, N - 1);
	seqSum = sequentialSum((float*)A, N);

	end = FETCHTIME();

	printf("Sequential time: %ld ms\n", (end - start)* 100 / clocks_per_sec);

	printf("\n\n");

	printf("Starting threaded...\n");




	start = FETCHTIME();

	for (int part = 0; part < NTHRD; part++)
	{
		workOrder[part].matrix = B;
		workOrder[part].high = (segmentSize * (part + 1)) - 1;
		workOrder[part].low = workOrder[part].high - segmentSize + 1;
		if (workOrder[part].high >= N)
			workOrder[part].high = N - 1;

		pthread_create(&workers[part], NULL, (void*)multithreaded, &workOrder[part]);
	}

	// Zad 2

	/*start = FETCHTIME();

	sequentialFill2(A);

	end = FETCHTIME();

	seqSum = sequentialSum((float*)A, N);

	printf("Sequential time: %ld ms\n", (end - start) * 100 / clocks_per_sec);

	start = FETCHTIME();

	int finishedThreads = 0;
	float previousColumnAverage = 0;
	pthread_cond_t workerCondition;
	pthread_cond_init(&workerCondition, NULL);
	pthread_cond_t masterCondition;
	pthread_cond_init(&masterCondition, NULL);
	pthread_mutex_t conditionMutex;
	pthread_mutex_init(&conditionMutex, NULL);

	for (int part = 0; part < NTHRD; part++) {
		workOrder[part].matrix = B;
		workOrder[part].high = (segmentSize * (part + 1)) - 1;
		workOrder[part].low = workOrder[part].high - segmentSize + 1;
		if (workOrder[part].high >= N)
			workOrder[part].high = N - 1;

		workOrder[part].previousColumnAverage = &previousColumnAverage;
		workOrder[part].partialColumnSum = 0;

		workOrder[part].finishedThreads = &finishedThreads;
		workOrder[part].workerCondition = &workerCondition;
		workOrder[part].masterCondition = &masterCondition;
		workOrder[part].conditionMutex = &conditionMutex;


		pthread_create(&workers[part], NULL, (void*)multithreadedSegment2, &workOrder[part]);
	}

	for (int i = 0; i < N; i++) {
		pthread_mutex_lock(&conditionMutex);

		while (finishedThreads != NTHRD) {
			pthread_cond_wait(&masterCondition, &conditionMutex);
		}

		float previousColumnSum = 0;
		for (int j = 0; j < NTHRD; j++) {
			previousColumnSum += workOrder[j].partialColumnSum;
			workOrder[j].canContinue = 1;
		}
		threadedSum += previousColumnSum;

		previousColumnAverage = previousColumnSum / N;
		finishedThreads = 0;
		pthread_cond_broadcast(&workerCondition);
		pthread_mutex_unlock(&conditionMutex);
	}*/

	for (int i = 0; i < NTHRD; i++) {
		pthread_join(workers[i], NULL);
	}

	/*pthread_mutex_destroy(&conditionMutex);
	pthread_cond_destroy(&masterCondition);
	pthread_cond_destroy(&workerCondition);*/

	end = FETCHTIME();

	printf("Threaded time: %ld ms\n", (end - start) * 100 / clocks_per_sec);

	// Linija ispod je potrebna za zadatke 1 i 3, za zadatak 2 se mora zakomentirati
	threadedSum = sequentialSum((float*)B, N);

	printf("Sequential sum is %f\nMultithreaded sum is %f\n", seqSum, threadedSum);
	double accuracy = (double)threadedSum / seqSum > 1 ? (double)seqSum / threadedSum : (double)threadedSum / seqSum;
	printf("Accuracy is %.6lf%%\n", accuracy * 100);

	free(A);
	free(B);
	return 0;
}
