#include "tallybench_header.h"

void save_tallies( double *** tallies, int assemblies, int bins_per_assembly, int isotopes )
{
	char * fname = "tallies.dat";
	printf("Saving tallies to %s...", fname);
	fflush(stdout);
	FILE * fp = fopen(fname, "w");
	for( int a = 0; a < assemblies; a++ )
	{
		for( int b = 0; b < bins_per_assembly; b++ )
		{
			for( int i = 0; i < isotopes; i++ )
			{
				fprintf(fp, "%.2le", tallies[a][b][i]);
				if( i < isotopes -1 )
					fprintf(fp, " ");
			}
			if( a < assemblies - 1 || b < bins_per_assembly - 1 )
				fprintf(fp, "\n");
		}
	}
	fclose(fp);
	printf(" done!\n");
}

// Prints program logo
void logo(int version)
{
	border_print();
	printf(
			"             _______    _ _       ______                    _     \n"
			"            (_______)  | | |     (____  \\                  | |    \n"
			"                _ _____| | |_   _ ____)  )_____ ____   ____| |__  \n"
			"               | (____ | | | | | |  __  (| ___ |  _ \\ / ___)  _ \\ \n"
			"               | / ___ | | | |_| | |__)  ) ____| | | ( (___| | | |\n"
			"               |_\\_____|\\_)_)__  |______/|_____)_| |_|\\____)_| |_|\n"
			"                           (____/                                 \n\n"
		  );
	border_print();
	center_print("Developed at Argonne National Laboratory", 79);
	char v[100];
	sprintf(v, "Version: %d", version);
	center_print(v, 79);
	border_print();
}

// Prints Section titles in center of 80 char terminal
void center_print(const char *s, int width)
{
	int length = strlen(s);
	int i;
	for (i=0; i<=(width-length)/2; i++) {
		fputs(" ", stdout);
	}
	fputs(s, stdout);
	fputs("\n", stdout);
}

void print_results( Inputs in, int mype, double runtime, int nprocs, unsigned long long vhash )
{
	// Print output
	border_print();
	center_print("RESULTS", 79);
	border_print();

	// Print the results
	printf("Threads:      %d\n", in.threads);
	printf("Runtime:      %.3lf seconds\n", runtime);
	printf("Tallies/s:    %.2le\n", (double)in.total_tallies/runtime);
	printf("Checksum:     %llu", vhash);
	unsigned long long expected = 103752;
	if( in.default_problem == MEDIUM )
		expected = 566873;
	else if( in.default_problem == LARGE )
		expected = 283547;
	if( vhash == expected )
		printf(" (passed)\n");
	else
		printf(" (warning - unverified hash)\n");

	border_print();
}

void print_inputs(Inputs in, int nprocs, int version )
{
	// Calculate Estimate of Memory Usage
	double mem_tot = estimate_mem_usage( in );
	logo(version);
	center_print("INPUT SUMMARY", 79);
	border_print();
	if( in.simulation_method == EVENT_BASED )
		printf("Simulation Method:            Event Based\n");
	else
		printf("Simulation Method:            History Based\n");
	char * prob_size = "Small";
	if( in.default_problem == MEDIUM )
		prob_size = "Medium";
	else if( in.default_problem == LARGE )
		prob_size = "Large";
	printf("Problem Size:                 %s\n", prob_size);
	printf("Materials:                    %d\n", 12);
	printf("Total Nuclides:               %d\n", in.isotopes);
	printf("Reactor Assemblies:           %d\n", in.assemblies);
	printf("Spatial Bins per Assembly:    %d\n", in.bins_per_assembly);
	printf("Total Spatial bins:           "); fancy_int((unsigned long) in.assemblies * (unsigned long) in.bins_per_assembly);
	printf("Total Tally bins:             "); fancy_int((unsigned long) in.assemblies * (unsigned long) in.bins_per_assembly * in.isotopes);
	if( in.simulation_method == HISTORY_BASED )
	{
	printf("Particle Histories:           "); fancy_int(in.particles);
	printf("Tally Events per Particle:    "); fancy_int(in.events_per_particle);
	}
	printf("Total Tally Events:           "); fancy_int(in.total_tallies);
	printf("Threads:                      %d\n", in.threads);
	printf("Est. Memory Usage (MB):       %.0lf\n", mem_tot);
}

void border_print(void)
{
	printf(
			"==================================================================="
			"=============\n");
}

// Prints comma separated integers - for ease of reading
void fancy_int( long a )
{
	if( a < 1000 )
		printf("%ld\n",a);

	else if( a >= 1000 && a < 1000000 )
		printf("%ld,%03ld\n", a / 1000, a % 1000);

	else if( a >= 1000000 && a < 1000000000 )
		printf("%ld,%03ld,%03ld\n",a / 1000000,(a % 1000000) / 1000,a % 1000 );

	else if( a >= 1000000000 )
		printf("%ld,%03ld,%03ld,%03ld\n",
				a / 1000000000,
				(a % 1000000000) / 1000000,
				(a % 1000000) / 1000,
				a % 1000 );
	else
		printf("%ld\n",a);
}

void print_CLI_error(void)
{
	printf("Usage: ./tallybench <options>\n");
	printf("Options include:\n");
	printf("  -m <simulation method>   Simulation method (history, event)\n");
	printf("  -t <threads>             Number of OpenMP threads to run\n");
	printf("  -s <size>                Size of H-M Benchmark to run (small, large, XL, XXL)\n");
	printf("  -g <gridpoints>          Number of gridpoints per nuclide (overrides -s defaults)\n");
	printf("  -G <grid type>           Grid search type (unionized, nuclide, hash). Defaults to unionized.\n");
	printf("  -p <particles>           Number of particle histories\n");
	printf("  -l <lookups>             History Based: Number of Cross-section (XS) lookups per particle. Event Based: Total number of XS lookups.\n");
	printf("  -h <hash bins>           Number of hash bins (only relevant when used with \"-G hash\")\n");
	printf("Default is equivalent to: -m history -s large -l 34 -p 500000 -G unionized\n");
	printf("See readme for full description of default run values\n");
	exit(4);
}

Inputs read_CLI( int argc, char * argv[] )
{
	Inputs input;

	// defaults to the history based simulation method
	input.simulation_method = HISTORY_BASED;

	// defaults to max threads on the system	
	#ifdef OPENMP
	input.threads = omp_get_num_procs();
	#else
	input.threads = 1;
	#endif

	// defaults to 355 (corresponding to H-M Large benchmark)
	input.isotopes = 355;

	// defaults to 500,000
	input.particles = 500000;

	input.assemblies = 241;
	input.bins_per_assembly = 17*17;

	// defaults to 98
	input.events_per_particle = 98;

	input.default_problem = SMALL;

	input.save_tallies = 0;

	int custom_particles = 0;

	// Collect Raw Input
	for( int i = 1; i < argc; i++ )
	{
		char * arg = argv[i];

		// threads (-t)
		if( strcmp(arg, "-t") == 0 )
		{
			if( ++i < argc )
				input.threads = atoi(argv[i]);
			else
				print_CLI_error();
		}
		// isotopes (-i)
		else if( strcmp(arg, "-i") == 0 )
		{	
			if( ++i < argc )
			{
				input.isotopes = atoi(argv[i]);
			}
			else
				print_CLI_error();
		}
		// Simulation Method (-m)
		else if( strcmp(arg, "-m") == 0 )
		{
			char * sim_type;
			if( ++i < argc )
				sim_type = argv[i];
			else
				print_CLI_error();

			if( strcmp(sim_type, "history") == 0 )
				input.simulation_method = HISTORY_BASED;
			else if( strcmp(sim_type, "event") == 0 )
			{
				input.simulation_method = EVENT_BASED;
			}
			else
				print_CLI_error();
		}
		// Problem Size (-s)
		else if( strcmp(arg, "-s") == 0 )
		{
			char * prob_size;
			if( ++i < argc )
				prob_size = argv[i];
			else
				print_CLI_error();

			if( strcmp(prob_size, "small") == 0 )
			{
				input.default_problem = SMALL;
			}
			else if( strcmp(prob_size, "medium") == 0 )
			{
				input.default_problem = MEDIUM;
				if( !custom_particles )
					input.particles *= 10;
				input.bins_per_assembly = 17*17*35;
			}
			else if( strcmp(prob_size, "large") == 0 )
			{
				input.default_problem = LARGE;
				if( !custom_particles )
					input.particles *= 100;
				input.bins_per_assembly = 17*17*350;
			}
			else
				print_CLI_error();
		}
		// particles (-p)
		else if( strcmp(arg, "-p") == 0 )
		{
			if( ++i < argc )
			{
				input.particles = atoi(argv[i]);
				custom_particles = 1;
			}
			else
				print_CLI_error();
		}
		// save tallies to file (-f)
		else if( strcmp(arg, "-f") == 0 )
		{
			input.save_tallies = 1;
		}
		else
			print_CLI_error();
	}

	// Validate Input

	// Validate threads
	if( input.threads < 1 )
		print_CLI_error();

	// Validate n_isotopes
	if( input.isotopes < 1 )
		print_CLI_error();

	input.total_tallies = input.particles * input.events_per_particle;

	// Return input struct
	return input;
}
