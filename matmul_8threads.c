#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 1024
#define NR_OF_THREADS 8

static double a[SIZE][SIZE];
static double b[SIZE][SIZE];
static double c[SIZE][SIZE];

struct thread_data
{
	int start_i;
	int start_j;
	int end_i;
	int end_j;
};

pthread_t threads[NR_OF_THREADS];
struct thread_data t_data[NR_OF_THREADS];

static void init_matrix()
{
	int i, j;

	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			/* Simple initialization, which enables us to easy check
			 * the correct answer. Each element in c will have the same 
			 * value as SIZE after the matmul operation.
			 */
			a[i][j] = 1.0;
			b[i][j] = 1.0;
        	}
	}
}

static void *row_calculation_blocked(void *args)
{
	struct thread_data *data = (struct thread_data *)args;
	int i, j, k;

	for (i = data->start_i; i < data->end_i; i++) {
		for (j = data->start_j; j < data->end_j; j++) {
			c[i][j] = 0.0;
			for (k = 0; k < SIZE; k++) {
				c[i][j] = c[i][j] + a[i][k] * b[k][j];
			}
		}
	}

	return 0;
}

static void init_threads_matmul_blocked()
{
	int i;

	// Block wise 
	for(i = 0; i < NR_OF_THREADS*0.5; i++) {
		int k = i*2;
		t_data[k].start_i = 0;
		t_data[k].end_i = SIZE*0.5;
		t_data[k].start_j = SIZE/(NR_OF_THREADS*0.5) * i;
		t_data[k].end_j = SIZE/(NR_OF_THREADS*0.5) * (i+1);
		int t0 = pthread_create(&threads[k], NULL, row_calculation_blocked, &t_data[k]);

		t_data[k+1].start_i = SIZE*0.5;
		t_data[k+1].end_i = SIZE;
		t_data[k+1].start_j = t_data[k].start_j;
		t_data[k+1].end_j = t_data[k].end_j;
		int t1 = pthread_create(&threads[k+1], NULL, row_calculation_blocked, &t_data[k+1]);
	}
}

static void print_matrix(void)
{
	int i, j;

	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			printf(" %7.2f", c[i][j]);
		}
		printf("\n");
	}
}

void join_threads()
{
	int i;

	for (i = 0; i < NR_OF_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
}

int main(int argc, char **argv)
{
	init_matrix();

	init_threads_matmul_blocked();
	join_threads();

	//print_matrix();

	return 0;
}
