#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define KILO (1024)
#define MEGA (KILO*KILO)
#define MAX_ITEMS (64*MEGA)
#define NR_OF_THREADS 15 // To make 8 active threads. Check the report for more details

#define swap(v, a, b) {unsigned tmp; tmp=v[a]; v[a]=v[b]; v[b]=tmp;}

struct qsort_data
{
	int thread_number;
	int low;
	int high;
};

pthread_t threads[NR_OF_THREADS];
struct qsort_data t_qsort_data[NR_OF_THREADS];

static int *v;

static void print_array(void)
{
	int i;

	for (i = 0; i < MAX_ITEMS; i++) {
		printf("%d ", v[i]);
	}
	printf("\n");
}

static void init_array(void)
{
	int i;

	v = (int *) malloc(MAX_ITEMS*sizeof(int));
	for (i = 0; i < MAX_ITEMS; i++)
		v[i] = rand();
}

static unsigned partition(int *v, unsigned low, unsigned high, unsigned pivot_index)
{
	/* move pivot to the bottom of the vector */
	if (pivot_index != low) {
		swap(v, low, pivot_index);
	}

	pivot_index = low;
	low++;

	/* invariant:
	 * v[i] for i less than low are less than or equal to pivot
	 * v[i] for i greater than high are greater than pivot
	 */

	/* move elements into place */
	while (low <= high) {
		if (v[low] <= v[pivot_index]) {
			low++;
		}
		else if (v[high] > v[pivot_index]) {
			high--;
		}
		else {
			swap(v, low, high);
		}
	}

	/* put pivot back between two groups */
	if (high != pivot_index) {
		swap(v, pivot_index, high);
	}
	return high;
}

static void quick_sort(int *v, unsigned low, unsigned high)
{
	unsigned pivot_index;
    
	/* no need to sort a vector of zero or one element */
	if (low >= high) {
		return;
	}

	/* select the pivot value */
	pivot_index = (low+high)*0.5;

	/* partition the vector */
	pivot_index = partition(v, low, high, pivot_index);

	/* sort the two sub arrays */
	if (low < pivot_index) {
		quick_sort(v, low, pivot_index-1);
	}
	if (pivot_index < high) {
		quick_sort(v, pivot_index+1, high);
	}
}

static void *thread_quick_sort(void *args)
{
	struct qsort_data *data = (struct qsort_data *)args;
	int parentID = data->thread_number;
	int low = data->low;
	int high = data->high;
	int pivot_index;

	if(low >= high) {
		return NULL;
	}
	else {
		pivot_index = (low + high)*0.5;
		pivot_index = partition(v, low, high, pivot_index);
		int childID = parentID*2; // To give the child thread a unique ID

		// Create thread for the x side in -> [x, ... , x, pivot, y, ... , y]
		if(childID > NR_OF_THREADS-1) {
			quick_sort(v, low, pivot_index);
		}
		else {
			childID++;
			t_qsort_data[childID].thread_number = childID;
			t_qsort_data[childID].low = low;
			t_qsort_data[childID].high = pivot_index;

			int t0 = pthread_create(&threads[childID], NULL, thread_quick_sort, &t_qsort_data[childID]);
		}

		// Create thread for the y side in -> [x, ... , x, pivot, y, ... , y]
		if(childID > NR_OF_THREADS-1) {
			quick_sort(v, pivot_index, high);
		}
		else {
			childID++;
			t_qsort_data[childID].thread_number = childID;
			t_qsort_data[childID].low = pivot_index + 1;
			t_qsort_data[childID].high = high;

			int t1 = pthread_create(&threads[childID], NULL, thread_quick_sort, &t_qsort_data[childID]);
		}

		// Join the 2 child threads of the parent
		if (childID < NR_OF_THREADS) {
			int j0 = pthread_join(threads[childID-1], NULL);
			int j1 = pthread_join(threads[childID], NULL);
		}
	}

	return NULL;
}

int check_order() 
{
	int i = 0;
	for(i = 0; i < MAX_ITEMS-1; i++) {
		if(v[i] > v[i+1]) {
			printf("%d > %d, pos %d\n", v[i], v[i+1], i);
			//return 1;
		}
	}
	return -1;
}

int main(int argc, char **argv)
{
	v = (int *) malloc(MAX_ITEMS*sizeof(int));

	init_array();
	//print_array();

	int i = 0;
	t_qsort_data[i].thread_number = i;
	t_qsort_data[i].low = 0;
	t_qsort_data[i].high = MAX_ITEMS-1;
	int t0 = pthread_create(&threads[i], NULL, thread_quick_sort, &t_qsort_data[i]);
	int j0 = pthread_join(threads[i], NULL);

	//printf("Check order: %d\n", check_order());

	//print_array();

	free(v);
}

