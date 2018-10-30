#include "tallybench_header.h"

void run_history_based_simulation(Inputs in, double *** restrict tallies, int * restrict num_nucs, int ** restrict mats, double ** restrict concs, int ** restrict spatial_mats)
{

	// Particle History Loop
	#pragma omp parallel for schedule(dynamic, 100)
	for( int p = 0; p < in.particles; p++ )
	{
		unsigned long seed = ((unsigned long) p+ (unsigned long)1)* (unsigned long) 13371337;

		double weight = 1.0;

		// Tally Loop
		for( int e = 0; e < in.events_per_particle; e++ )
		{
			// TODO: Use real ray tracer to determine location?

			// Determine which assembly it is in
			int assembly = rni(&seed) % in.assemblies; 

			// Determine which bin it is in
			int bin = rni(&seed) % in.bins_per_assembly;

			// Determine which material it is in
			//int mat = pick_mat(&seed); 
			int mat = spatial_mats[assembly][bin];

			// Pick phi
			double phi = rn(&seed);

			// Score Accumulator
			double total_score = 0.0;

			// Nuclides Loop
			for( int n = 0; n < num_nucs[mat] ; n++ )
			{
				// Find isotope index
				int idx = mats[mat][n]; 

				// TODO: look this up based on material and nuclide (?)
				double micro_xs = rn(&seed);

				// Look up nuclide density in material
				double rho = concs[mat][n];

				// Compute Score
				double score = micro_xs * rho * phi * weight;
				//printf("micro_xs = %lf rho = %lf phi = %lf weight = %lf\n", micro_xs, rho, phi, weight);
				//printf("tallying score = %lf\n", score);

				// Normalize Score by number of tallies
				//score /= in.total_tallies;

				// Tally score to global array
				#pragma omp atomic
				tallies[assembly][bin][idx] += score;

				// Accumulate score
				total_score += score;
			}

			// Reduce particle weight based on score
			weight *= 1.0/total_score;
		}	
	}
}
