/*
 *  This file contains implementation of integration using Monte Carlo and Quadrature method
 *
 *  Author :
 *  Saurabh Korade (skorade1@binghamton.edu)
 *  State University of New York, Binghamton
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

//Structure of integral
struct integral_struct{
    int tid;			//Thread id for reference
    double a1;			//Lower range
    double b1;			//Upper range
    double n;			//Number of samples
    double answer;		//Partial answer
};

double a;
double b;
double N;
double c;
int n_threads;

//Thread function to calcuate the integral by Quadrature method
void* quadrature_integral_runner(void* arg){
	struct integral_struct *integral = (struct integral_struct*)arg;

	double x,epsilon, sum = 0;

	epsilon = (integral->b1 - integral->a1)/integral->n;

	for(int i=0 ; i< integral->n ; i++){
		x = a + (epsilon * i);
		sum +=(double)(sin(x)/x);
	}

	integral->answer = (sum / integral->n) * (integral->b1 - integral->a1);

	pthread_exit(0);
}

//Thread function to calcuate the integral by Monte Carlo method
void* mc_integral_runner(void* arg){

    struct integral_struct *integral = (struct integral_struct*)arg;

	double x,random,sum = 0;
	struct drand48_data randBuffer;
	srand48_r(time(NULL), &randBuffer);

    for(int i=0 ; i< integral->n ; i++)
    {
        //Generating uniformly distributed random number between lower and upper range of the integral
        int range = integral->b1 - integral->a1 + 1;
        drand48_r(&randBuffer, &random);
        x = (random * range) + integral->a1;

        //Substituting the random value in to function f(x)
        sum +=(double)(sin(x)/x);
    }

    integral->answer = (sum / integral->n) * (integral->b1 - integral->a1);

    pthread_exit(0);
}

int montecarlo(){
	double sum = 0;
	pthread_t tids[n_threads];

    struct integral_struct *integral= malloc(n_threads * sizeof(*integral));

    for(int i = 0; i<n_threads ; i++){
        integral[i].tid = i;

		//calculate new a and b for ever thread
		integral[i].a1 = a + (i * c);
        integral[i].b1 = integral[i].a1 + c;

		//compting equal samples in each thread
		integral[i].n = N / n_threads;

		//Creating attributes for the thread
        pthread_attr_t attr;
        pthread_attr_init(&attr);
	
        pthread_create(&tids[i] , &attr , mc_integral_runner , &integral[i]);
    }

    //Wait until thread has done its work and calculate the aggregate sum
    for(int i = 0; i<n_threads ; i++){
        pthread_join(tids[i],NULL);
        //printf("\n%f",integral[i].answer);

		//Aggregating the partial answers calculated by n_threads
        sum += integral[i].answer;
    }

    printf("%0.40f\n",sum);

	//Deallocate memory
    free(integral);

	return 0;
}

int quadrature(){
	double sum = 0;
	pthread_t tids[n_threads];

    struct integral_struct *integral= malloc(n_threads * sizeof(*integral));

    for(int i = 0; i<n_threads ; i++){
        integral[i].tid = i;

		//calculate new a and b for ever thread
		integral[i].a1 = a + (i * c);
        integral[i].b1 = integral[i].a1 + c;

		//compting equal samples in each thread
		integral[i].n = N / n_threads;

		//Creating attributes for the thread
        pthread_attr_t attr;
        pthread_attr_init(&attr);

        pthread_create(&tids[i] , &attr , quadrature_integral_runner , &integral[i]);
    }
	
    //Wait until thread has done its work and calculate the aggregate sum
    for(int i = 0; i<n_threads ; i++){
        pthread_join(tids[i],NULL);
        //printf("\n%f",integral[i].answer);

		//Aggregating the partial answers calculated by n_threads
        sum += integral[i].answer;
    }

    printf("%0.40f\n",sum);

	//Deallocate memory
    free(integral);

	return 0;
}

int main(int argc, char **argv){

	//Handling exception for incorrect number of arguments
    if(argc<4){
        printf("Usage: %s <a> <b> <number of samples> <number of threads>", argv[0]);
    }

    a = strtod(argv[1],NULL);
    b = strtod(argv[2],NULL);
    N = strtod(argv[3],NULL);
    n_threads = atoi(argv[4]);

	//Calculating c to distribute the range equally amongst the n_threads
    c = (double) (b - a)/n_threads;

	montecarlo();

	quadrature();

	return 0;
}