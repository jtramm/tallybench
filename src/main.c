#include "tallybench_header.h"

int main( int argc, char* argv[] )
{
	// =====================================================================
	// Initialization & Command Line Read-In
	// =====================================================================
	int version = 0;
	int mype = 0;
	double omp_start, omp_end;
	int nprocs = 1;

	unsigned long int seed = 42;

	// Process CLI Fields -- store in "Inputs" structure
	Inputs in = read_CLI( argc, argv );

	// Set number of OpenMP Threads
	omp_set_num_threads(in.threads); 

	// Print-out of Input Summary
	print_inputs( in, nprocs, version );

	// =====================================================================
	// Prepare Nuclide Energy Grids, Unionized Energy Grid, & Material Data
	// =====================================================================

	// Get material data
	int *num_nucs  = load_num_nucs(in.isotopes);
	int **mats     = load_mats(num_nucs, in.isotopes);
	double **concs = load_concs(num_nucs, &seed);
	double *** tallies = d3darr_contiguous(in.assemblies, in.bins_per_assembly, in.isotopes);

	// =====================================================================
	// Cross Section (XS) Parallel Lookup Simulation
	// =====================================================================

	border_print();
	center_print("SIMULATION", 79);
	border_print();
	printf("Simulating tally events...\n");

	omp_start = omp_get_wtime();

	// Run simulation
	if( in.simulation_method == EVENT_BASED )
	{
	}
	else if( in.simulation_method == HISTORY_BASED )
		run_history_based_simulation(in, tallies, num_nucs, mats, concs );

	printf("\n" );
	printf("Simulation complete.\n" );

	omp_end = omp_get_wtime();

	// =====================================================================
	// Output Results & Finalize
	// =====================================================================
	
	// Compute Verification Hash
	long total_bins = (long) in.assemblies * (long) in.bins_per_assembly * (long) in.isotopes;
	double total = pairwise_sum_dbl( tallies[0][0], total_bins );
	unsigned long long vhash = (unsigned long) total;
	vhash = vhash % 1000000;

	// Print / Save Results and Exit
	print_results( in, mype, omp_end-omp_start, nprocs, vhash );

	save_tallies(tallies, in.assemblies, in.bins_per_assembly, in.isotopes);  

	// Free stuff
	free(num_nucs);
	free(mats[0]);
	free(mats);
	free(concs[0]);
	free(concs);
	free(tallies[0][0]);
	free(tallies[0]);
	free(tallies);

	return 0;
}
