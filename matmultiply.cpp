#include <pthread.h> 	// pthread_t, pthread_join, pthread_create, pthread_exit
#include <iostream>		// cout 
#include <cstdlib>		// atoi, srand, rand, exit, EXIT_SUCCESS
#include <ctype.h>		// tolower
#include <semaphore.h>  // sem_init, sem_post, sem_wait, sem_t

using namespace std;

// Global Variables

int step = 0;			  // step through value for loop
sem_t bin_sem;			  // binary semaphore

// struct to pass in number of 
struct passed_args{
	float *MATA, *MATB, *MATC; // matrices
	int N; // N x N
	int numThreads;
};
void* matMultiply(void *args);

void funcA(float *MATA, float *MATB, float *MATC, int N);
void funcB(float *MATA, float *MATB, float *MATC, int N);


int main(int argc, char* argv[]) {

	// check for correct amount of arguments
	if (argc != 3) {
		cout << "Usage Info:" << endl;
		cout << "./matMultiply s/m N" << endl;
		exit(EXIT_FAILURE);
	}
	// read from terminal
	char input = argv[1][0]; 
	int N = atoi(argv[2]);

	srand(time(NULL)); // make it more random
	int row = N, col = N; // just for my own clarification

	// Allocate memory on the heap
	float* MATA = new float[row * col];
	float* MATB = new float[row * col];
	float* MATC = new float[row * col];
	// -----------------------------

	// Fill with random values
	for (int i = 0; i < row * col; i++) {
		MATA[i] = rand();
		MATB[i] = rand();
	}

	// do pthread things
	sem_init(&bin_sem, 0, 0); // initialize unnamed semaphore
	// to lower incase upper case is used
	if (tolower(input, locale()) == 's') {
		funcA(MATA, MATB, MATC, N);
	}
	// to lower just in case upper case used
	else if (tolower(input, locale()) == 'm') {
		funcB(MATA, MATB, MATC, N);
	}
	else
		cout << "Invalid input!" << endl;

	sem_destroy(&bin_sem); // get rid of semaphore

	// free allocated memory and call destructors
	delete [] MATA;
	delete [] MATB;
	delete [] MATC;
	exit(EXIT_SUCCESS);
}

void funcA(float *MATA, float *MATB, float *MATC, int N) {
	int row = N, col = N;
	struct passed_args pass;
	pass.MATA = MATA;
	pass.MATB = MATB;
	pass.MATC = MATC;
	pass.N = N;
	pthread_t threads;
	int numThreads = 1; //m single thread
	pass.numThreads = numThreads;
	
	// pass the struct 
	void *p = &pass;
	// create thread
	
	pthread_create(&threads, NULL, matMultiply , p);
	sem_post(&bin_sem); // bin_sem++
	// join thread
	pthread_join(threads, NULL);

}
void funcB(float *MATA, float *MATB, float *MATC, int N) {
	int row = N, col = N, numThreads;
	if(N > 9) // maximum 10 threads
		numThreads = 10;
	else // otherwise use N threads
		numThreads = N; 
	pthread_t threads[numThreads]; // thread array
	struct passed_args pass;
	pass.MATA = MATA;
	pass.MATB = MATB;
	pass.MATC = MATC;
	pass.N = N;
	pass.numThreads = numThreads;
	// create threads and pass in N
	for (int i = 0; i < numThreads; i++) {
		void *p = &pass;
		// create threads
		pthread_create(&threads[i], NULL, matMultiply, p);
		sem_post(&bin_sem); //bin_sem++
	}
	// join all threads
	for (int i = 0; i < numThreads; i++)
		// join threads
		pthread_join(threads[i], NULL); 
}


void* matMultiply(void*args) {
	//int N = *(int*)args; // read N
	struct passed_args* p = (struct passed_args*)args;
	int c = step++;
	float *MATA = p->MATA;
	float *MATB = p->MATB;
	float *MATC = p->MATC;
	int N = p->N;
	int numThreads = p->numThreads;
	sem_wait(&bin_sem); // bin_sem--
	int row = N, col = N;

	// let the thread do the work it needs to do for each thread
	for (int i = c * (N / numThreads); i < ((c + 1)*(N)) / numThreads; i++) {
		for (int j = 0; j < N; j++) {
			float sum = 0.0;
			for (int k = 0; k < N; k++)
				sum += MATA[i * col + k] * MATB[k * col + j];
			MATC[i * col + j] = sum; //  put it in the right spot
		}
	}
	pthread_exit(NULL); // exit the thread
}