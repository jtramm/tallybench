#include "tallybench_header.h"

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
	// Calculate Lookups per sec
	int tallies_per_sec = (int) ((double) in.total_tallies / runtime);

	// Print output
	border_print();
	center_print("RESULTS", 79);
	border_print();

	// Print the results
	printf("Threads:     %d\n", in.threads);
	printf("Runtime:     %.3lf seconds\n", runtime);
	printf("Tallies:     "); fancy_int(in.total_tallies);
	printf("Tallies/s:   ");
	fancy_int(tallies_per_sec);

	border_print();
}

void print_inputs(Inputs in, int nprocs, int version )
{
	// Calculate Estimate of Memory Usage
	int mem_tot = estimate_mem_usage( in );
	logo(version);
	center_print("INPUT SUMMARY", 79);
	border_print();
	#ifdef VERIFICATION
	printf("Verification Mode:            on\n");
	#endif
	if( in.simulation_method == EVENT_BASED )
		printf("Simulation Method:            Event Based\n");
	else
		printf("Simulation Method:            History Based\n");

	printf("Materials:                    %d\n", 12);
	printf("Total Nuclides:               %d\n", in.isotopes);
	printf("Reactor Assemblies:           %d\n", in.assemblies);
	printf("Bins per Assembly:            %d\n", in.bins_per_assembly);
	if( in.simulation_method == HISTORY_BASED )
	{
	printf("Particle Histories:           "); fancy_int(in.particles);
	printf("Tally Events per Particle:    "); fancy_int(in.events_per_particle);
	}
	printf("Total Tallies:                "); fancy_int(in.total_tallies);
	printf("Threads:                      %d\n", in.threads);
	printf("Est. Memory Usage (MB):       "); fancy_int(mem_tot);
	border_print();
	center_print("INITIALIZATION - DO NOT PROFILE", 79);
	border_print();
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
	input.threads = omp_get_num_procs();

	// defaults to 355 (corresponding to H-M Large benchmark)
	input.isotopes = 355;

	// defaults to 500,000
	input.particles = 500000;

	input.assemblies = 241;
	input.bins_per_assembly = 17*17;

	// defaults to 34
	input.events_per_particle = 34;

	input.default_problem = SMALL;

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
		// particles (-p)
		else if( strcmp(arg, "-p") == 0 )
		{
			if( ++i < argc )
			{
				input.particles = atoi(argv[i]);
			}
			else
				print_CLI_error();
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
