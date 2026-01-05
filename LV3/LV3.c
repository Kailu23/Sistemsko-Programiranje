#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#define N 500
#define NTHRD 8

typedef struct _matrixWorkOrder {
	float* matrix;
	int low;
	int high;
} matrixWorkOrder;

void printMatrix(float* matrix, int size) {
	float sum = 0;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			sum += matrix[i * N + j];
			printf("%f ", matrix[i * N + j]);
			//printf("%f ", *(matrix + i*N + j));
		}
		printf("\n");
	}

	printf("\nSum is %f\n", sum);

}

void sequential(float* matrix, int size) {
	float sum = 0;
	float value;

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			for (int k = 0; k < i; k++)
			{
				value = k * sin(j) - j * cos(k);
				sum += value;
			}
			matrix[i * N + j] = sum;
			sum = 0;
		}
	}
}
void multithreadedSegment(float* matrix, int low, int high) {
	float sum = 0;
	float value;

	for (int i = low; i < high; i++)
	{
		for (int j = 0; j < N; j++)
		{
			for (int k = 0; k < i; k++)
			{
				value = k * sin(j) - j * cos(k);
				sum += value;
			}
			matrix[i * N + j] = sum;
			sum = 0;
		}
	}
}

void multithreaded(matrixWorkOrder* workOrder) {
	
		multithreadedSegment(workOrder->matrix, workOrder->low, workOrder->high);
		free(workOrder);

}
int main() {
	float A[N][N];
	
	sequential(A, N);
	printMatrix(A, N);

	int high, low;
	pthread_t workers[NTHRD];

	matrixWorkOrder* workOrder = (matrixWorkOrder*)malloc(sizeof(matrixWorkOrder));
	int  segmentSize = (int)ceil((double)N / NTHRD);

	workOrder->matrix = A;

	time_t start, end;
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

	printf("Time: %lld", end - start);
	return 0;
}