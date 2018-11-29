#include "tallybench_header.h"

void run_history_based_simulation(Inputs in, double *** restrict tallies, int * restrict num_nucs, int ** restrict mats, double ** restrict concs, int ** restrict spatial_mats, Reactor_Mesh * restrict RM, unsigned long ** restrict tally_hits)
{

	// Particle History Loop
	#pragma omp parallel for schedule(static)
	for( int p = 0; p < in.particles; p++ )
	{
		unsigned long seed = ((unsigned long) p+ (unsigned long)1)* (unsigned long) 13371337;

		double weight = 1.0;

		// Tally Loop
		for( int e = 0; e < in.events_per_particle; e++ )
		{
			// Sample Cartesian location randomly from somewhere inside reactor mesh
			Coord location = sample_random_location( RM, &seed );

			// Determine which assembly it is in
			int assembly = find_assembly_id( RM, location ); 

			// Determine which pin it is in
			int pin = find_pin_id( RM, assembly, location );

			// Determine which axial location of the pin it is in, and therefore
			// which final tally bin should be accessed
			int axial_id = find_axial_id(RM, location);
			int bin = pin * RM->axial_regions + axial_id;

			// Determine which material it is in (determined by pin)
			int mat = RM->assemblies[assembly].material_ids[0][pin];

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
				// This is covered by XSbench & RSBench (micro_xs would be cached)
				// In OpenMC, micro_xs is a global array (of length n_nuclides) that is
				// unique to each thread (i.e., threadprivate). In the event based model,
				// this would have to be changed so that the cache was stored along
				// with the particle. The data is stored as an array of structs.
				//
				// SO: in the history based model, I would just 
				//
				double micro_xs = rn(&seed);

				// Look up nuclide density in material
				double rho = concs[mat][n];

				// Compute Score
				// In OpenMC, this is computed as: micro_xs[nuclide_id].total * atom_density * flux
				// with flux = particle_weight * distance_travelled
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
			#ifdef TEST
			#pragma omp atomic
			tally_hits[assembly][bin]++;
			#endif

			// Reduce particle weight based on score
			weight *= 1.0/total_score;
		}	
	}
}
