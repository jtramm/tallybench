#include "tallybench_header.h"

double *** d3darr_contiguous(size_t l, size_t m, size_t n)
{
    int i, j;
    double *inner = malloc(l*m*n*sizeof(double));
    double **middle = malloc(l*m*sizeof(double*));
    double ***matrix = malloc(l*sizeof(double**));

    for(i=0; i<l; i++){
        matrix[i] = middle;
        middle += m;
        for(j=0; j<m; j++){
            matrix[i][j] = inner;
            inner += n;
        }
    }
    return matrix;
}

// Park & Miller Multiplicative Conguential Algorithm
// From "Numerical Recipes" Second Edition
double rn(unsigned long * seed)
{
	double ret;
	unsigned long n1;
	unsigned long a = 16807;
	unsigned long m = 2147483647;
	n1 = ( a * (*seed) ) % m;
	*seed = n1;
	ret = (double) n1 / m;
	return ret;
}

double rni(unsigned long * seed)
{
	double ret;
	unsigned long n1;
	unsigned long a = 16807;
	unsigned long m = 2147483647;
	n1 = ( a * (*seed) ) % m;
	*seed = n1;
	ret = n1 % m;
}

unsigned int hash(char *str, int nbins)
{
	unsigned int hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c;

	return hash % nbins;
}

size_t estimate_mem_usage( Inputs in )
{
	size_t memtotal = 1;

	return memtotal;
}
