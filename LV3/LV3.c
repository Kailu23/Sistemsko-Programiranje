#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#define N 4000
#define NTHRD 16

typedef struct _matrixWorkOrder {
	float* matrix;
	int low;
	int high;
} matrixWorkOrder;

void printMatrix(float* matrix, int low, int high) {
	for (int i = low; i < high; i++)
	{
		for (int j = 0; j < high; j++)
		{
			printf("%f ", matrix[i * N + j]);
			//printf("%f ", *(matrix + i*N + j));
		}
		printf("\n");
	}
}

float firstFormulae(int i, int j) {
	float sum = 0;
	float value;

	for (int k = 0; k < i; k++) {
		value = k * sin(j) - j * cos(k);
		sum += value;
	}

	return sum;
}

void sequentialFill(float* matrix, int size) {
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
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
	for (int i = low; i < high; i++)
	{
		for (int j = 0; j < N; j++)
		{
			matrix[i * N + j] = firstFormulae(i, j);
		}
	}
	//printf("\n\n");
	//printMatrix(matrix, low, high);
	//printf("\n\n");
}

void multithreaded(matrixWorkOrder* workOrder) {
	
		multithreadedSegment(workOrder->matrix, workOrder->low, workOrder->high);

}

int main() {
	float *A = (float*)malloc(N*N*sizeof(float));

	time_t start, end;
	float seqSum = 0, threadedSum = 0;

	printf("Starting sequential...\n");
	time(&start);
	
	sequentialFill((float*)A, N);
	//printMatrix((float*)A, 0, N);
	seqSum = sequentialSum((float*)A, N);
	
	time(&end);
	
	printf("Time: %ld\n", end - start);
	
	printf("\n\n");
	
	printf("Starting threaded...\n");

	pthread_t workers[NTHRD];

	matrixWorkOrder* workOrder = (matrixWorkOrder*)malloc(sizeof(matrixWorkOrder));
	int  segmentSize = (int)ceil((double)N / NTHRD);

	float* B = (float*)malloc(N * N * sizeof(float));

	workOrder->matrix = (float*)B;

	time(&start);
	for (int part = 1; part <= NTHRD; part++)
	{
		workOrder->high = (segmentSize * part) - 1;
		workOrder->low = workOrder->high - segmentSize + 1;
		if (workOrder->high >= N)
			workOrder->high = N - 1;

		pthread_create(&workers[part - 1], NULL, (void*)multithreaded, (void*)workOrder);
	}

	for (int i = 0; i < NTHRD; i++)
	{
		pthread_join(workers[i], NULL);
	}
	time(&end);

	printf("Time: %ld\n", end - start);

	threadedSum = sequentialSum((float*)A, N);

	printf("Sequential sum is %f\nMultithreaded sum is %f\n", seqSum, threadedSum);
	printf("Precision is %f\n", (float)threadedSum/seqSum);

	free(A);
	free(B);
	free(workOrder);
	return 0;
}
