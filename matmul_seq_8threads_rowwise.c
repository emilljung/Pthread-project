/***************************************************************************
 *
 * Sequential version of Matrix-Matrix multiplication
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 1024
#define NR_OF_THREADS 8
#define ROWS_PER_THREAD SIZE/NR_OF_THREADS

static double a[SIZE][SIZE];
static double b[SIZE][SIZE];
static double c[SIZE][SIZE];

struct thread_data
{
	int start_i;
	int end_i;
};

pthread_t threads[NR_OF_THREADS];
struct thread_data t_data[NR_OF_THREADS];

static void init_matrix(void)
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

static void *thread_matmul_seq(void *args)
{
	struct thread_data *data = (struct thread_data *)args;
	int i, j, k;

	for (i = data->start_i; i < data->end_i; i++) {
		for (j = 0; j < SIZE; j++) {
			c[i][j] = 0.0;
			for (k = 0; k < SIZE; k++) {
				c[i][j] = c[i][j] + a[i][k] * b[k][j];
			}
		}
	}
}

static void init_threads_matmul()
{
	int i;

	for(i = 0; i < NR_OF_THREADS; i++) {
		t_data[i].start_i = ROWS_PER_THREAD * i ;
		t_data[i].end_i = ROWS_PER_THREAD * (i+1);
		int t0 = pthread_create(&threads[i], NULL, thread_matmul_seq, &t_data[i]);
	}
}

static void print_matrix(void)
{
    int i, j;

    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++)
	    printf(" %7.2f", c[i][j]);
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

	init_threads_matmul();
	join_threads();

	//print_matrix();

	return 0;
}


