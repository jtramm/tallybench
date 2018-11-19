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
	#ifdef OPENMP
	omp_set_num_threads(in.threads); 
	#endif

	// Print-out of Input Summary
	print_inputs( in, nprocs, version );

	// =====================================================================
	// Prepare Nuclide Energy Grids, Unionized Energy Grid, & Material Data
	// =====================================================================

	// Get material data
	int *num_nucs  = load_num_nucs(in.isotopes);
	int **mats     = load_mats(num_nucs, in.isotopes);
	double **concs = load_concs(num_nucs, &seed);
	Reactor_Mesh * RM = build_reactor_mesh(in.axial_regions);
	double *** tallies = d3darr_contiguous(RM->valid_assemblies, RM->assemblies[0].N * RM->assemblies[0].N * RM->axial_regions, in.isotopes);
	unsigned long ** tally_hits;
	#ifdef TEST
	tally_hits = ul2arr_contiguous(RM->valid_assemblies, RM->assemblies[0].N * RM->assemblies[0].N * RM->axial_regions);
	#endif
	//int ** spatial_mats = initialize_spatial_mats(in, mats);
	int ** spatial_mats = NULL;

	// =====================================================================
	// Parallel Tally Simulation
	// =====================================================================

	border_print();
	center_print("SIMULATION", 79);
	border_print();
	printf("Simulating tally events...\n");

	omp_start = get_time();

	// Run simulation
	if( in.simulation_method == EVENT_BASED )
	{
	}
	else if( in.simulation_method == HISTORY_BASED )
		run_history_based_simulation(in, tallies, num_nucs, mats, concs, spatial_mats, RM, tally_hits );

	printf("Simulation complete.\n" );

	omp_end = get_time();

	// =====================================================================
	// Output Results & Finalize
	// =====================================================================
	
	// Compute Verification Hash
	printf("Verifying results...\n");
	long total_bins = (long) RM->total_spatial_bins * (long) in.isotopes;
	double total = parallel_pairwise_sum_dbl( tallies[0][0], total_bins, in.threads );
	//double total = parallel_sum(tallies[0][0], total_bins);
	unsigned long long vhash = (unsigned long) total;
	vhash = vhash % 1000000;

	// Print / Save Results and Exit
	print_results( in, mype, omp_end-omp_start, nprocs, vhash );

	if( in.save_tallies )
		save_tallies(tallies, in.assemblies, in.bins_per_assembly, in.isotopes);  

	// Print distribution test results, if specified
	#ifdef TEST
	FILE * fp = fopen("distro.dat", "w");
	for( long i = 0; i < (long) in.assemblies * (long) 17 * (long) 17 * (long) in.axial_regions; i++ )
		fprintf(fp, "%lu\n", tally_hits[0][i]);
	fclose(fp);
	#endif


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
