#include "tallybench_header.h"

void run_history_based_simulation(Inputs in, double *** tallies)
{

	// Particle History Loop
	for( int p = 0; p < in.particles; p++ )
	{
		unsigned long int seed = (p+1) * 13371337;

		double weight = 1.0;

		// Tally Loop
		for( int e = 0; e < in.events_per_particle; e++ )
		{
			// Determine which assembly it is in
			int assembly = rni(&seed) % in.assemblies; 

			// Determine which bin it is in
			int bin = rni(&seed) % in.bins_per_assembly;

			// Determine which material it is in
			// TODO: look this up based on assembly and bin
			int mat = pick_mat(&seed); 

			// Pick phi
			double phi = rn(&seed);

			// Score Accumulator
			double total_score = 0.0;

			// Nuclides Loop
			for( int n = 0; n < num_nucs[mat] ; n++ )
			{
				// Find isotope index
				int idx = mats[mat][n]; 

				// TODO: look this up based on material and nuclide
				double micro_xs = rn(&seed);

				// Look up nuclide density in material
				double rho = concs[mat][n];

				// Compute Score
				double score = micro_xs * rho * phi * weight;

				// Tally score to global array
				// TODO: make atomic
				tallies[assembly][bin][idx] += score;

				// Accumulate score
				total_score += score;
			}

			// Reduce particle weight based on score
			weight -= total_score;
		}	
	}
}
