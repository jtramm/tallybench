#ifndef __TALLYBENCH_HEADER_H__
#define __TALLYBENCH_HEADER_H__

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<math.h>
#include<assert.h>

#ifdef OPENMP
#include<omp.h>
#endif

#define EVENT_BASED 1
#define HISTORY_BASED 2

#define SMALL 1
#define MEDIUM 2
#define LARGE 3

typedef struct{
	int  threads;
	int isotopes; // 321
	int assemblies; // 241
	int bins_per_assembly; // Small: 17 x 17 = 289 Medium: x 50 = 14,450
	int particles;
	int events_per_particle; //34
	int total_tallies;
	int simulation_method; // Event or History
	int default_problem;
	int save_tallies;
	int axial_regions;
} Inputs;

typedef struct{
	double x;
	double y;
	double z;
} Coord;

typedef struct{
	int N;
	int ** material_ids;
	double pin_pitch;
	Coord lower_left;
} Assembly_Mesh;

typedef struct{
	int N;
	int ** assembly_ids;
	double assembly_pitch;
	Assembly_Mesh * assemblies;
	Coord lower_left;
	int valid_assemblies;
	int total_spatial_bins;
	int axial_regions;
	double height;
} Reactor_Mesh;

// Function Prototypes

// simulation.c
void run_history_based_simulation(Inputs in, double *** restrict tallies, int * restrict num_nucs, int ** restrict mats, double ** restrict concs, int ** restrict spatial_mats, Reactor_Mesh * restrict RM, unsigned long ** restrict tally_hits);

// mesh.c
long find_pin_id( Reactor_Mesh * RM, int assembly_id, Coord p );
long find_assembly_id( Reactor_Mesh * RM, Coord p );
long find_axial_id( Reactor_Mesh * RM, Coord p );
Reactor_Mesh * build_reactor_mesh(int axial_regions);
Coord sample_random_location( Reactor_Mesh * RM, unsigned long * seed );

// materials.c
int * load_num_nucs(long isotopes);
int ** load_mats( int * num_nucs, long isotopes );
double ** load_concs( int * num_nucs, unsigned long * seed );
int pick_mat( unsigned long * seed );
int ** initialize_spatial_mats(Inputs in, int **mats);

// io.c
void save_tallies( double *** tallies, int assemblies, int bins_per_assembly, int isotopes );
void logo(int version);
void center_print(const char *s, int width);
void print_results( Inputs in, int mype, double runtime, int nprocs, unsigned long long vhash );
void print_inputs(Inputs in, int nprocs, int version );
void border_print(void);
void fancy_int( long a );
void print_CLI_error(void);
Inputs read_CLI( int argc, char * argv[] );

// utils.c
double pairwise_sum_dbl( double * v, long len );
double parallel_pairwise_sum_dbl( double * v, long len, int nthreads);
double parallel_sum( double * v, long len );
int **imatrix(size_t m, size_t n);
double *** d3darr_contiguous(size_t l, size_t m, size_t n);
unsigned long ** ul2arr_contiguous(size_t m, size_t n);
unsigned long *** ul3darr_contiguous(size_t l, size_t m, size_t n);
double rn(unsigned long * seed);
int rni(unsigned long * seed);
double estimate_mem_usage( Inputs in );
double get_time(void);

#endif
