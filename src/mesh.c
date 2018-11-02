#include "tallybench_header.h"

Coord sample_random_location( Reactor_Mesh * RM, unsigned long * seed )
{
	// Pick a random assembly
	int assembly_id = rni(seed) % RM->valid_assemblies;
	Assembly_Mesh * AM = &RM->assemblies[assembly_id];

	// Sample a location randomly within assembly
	Coord loc;
	loc.x = AM->lower_left.x + rn(seed) * AM->N * AM->pin_pitch;
	loc.y = AM->lower_left.y + rn(seed) * AM->N * AM->pin_pitch;

	return loc;
}


long find_pin_id( Reactor_Mesh * RM, int assembly_id, Coord p )
{
	Assembly_Mesh * AM = &RM->assemblies[assembly_id];

	// Convert particle location into assembly frame of reference
	p.x -= AM->lower_left.x;
	p.y -= AM->lower_left.y;

	// Find coordinates (intentional floor via cast)
	int x_idx = p.x / AM->pin_pitch;
	int y_idx = p.y / AM->pin_pitch;

	long pin_id = y_idx * AM->N + x_idx;

	return pin_id;
}

long find_assembly_id( Reactor_Mesh * RM, Coord p )
{
	// Find coordinates (intentional floor via cast)
	int x_idx = p.x / RM->assembly_pitch;
	int y_idx = p.y / RM->assembly_pitch;

	// Lookup Assembly id
	long assembly_id = RM->assembly_ids[y_idx][x_idx];

	return assembly_id;
}

Reactor_Mesh * build_reactor_mesh(void)
{
	Reactor_Mesh * RM = malloc(sizeof(Reactor_Mesh));

	RM->N = 17;
	RM->lower_left.x = 0.0;
	RM->lower_left.y = 0.0;
	RM->assembly_pitch = 17*1.26;

	RM->assembly_ids = imatrix(RM->N, RM->N); 

	// Define pattern that assemblies will fit in reactor
	int locations[17][17] = {
		{0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
		{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
		{0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
		{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
		{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
		{0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
		{0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0}
	};

	int id = 0;

	// Copy pattern to unique assemby IDs
	for( int i = 0 ; i < RM->N; i++ )
	{
		for( int j = 0 ; j < RM->N; j++ )
		{
			if( locations[i][j] == 0 )
				RM->assembly_ids[i][j] = -1;
			else
			{
				RM->assembly_ids[i][j] = id++;
			}
		}
	}

	// Allocate room for actual number of assemblies
	RM->assemblies = malloc(id * sizeof(Assembly_Mesh));
	RM->valid_assemblies = id;
	id = 0;
	RM->total_spatial_bins = 0;
	Coord lower_left = {0, 0};
	for( int i = 0 ; i < RM->N; i++ )
	{
		for( int j = 0 ; j < RM->N; j++ )
		{
			// For a valid assembly, 
			if( RM->assembly_ids[i][j] != -1 )
			{
				RM->assemblies[id].N = RM->N;
				RM->assemblies[id].pin_pitch = 1.26;
				RM->assemblies[id].material_ids = imatrix(RM->N, RM->N);
				RM->assemblies[id].lower_left = lower_left;

				// Fill assembly with random materials (from realistic distribution)
				unsigned long seed = 42;
				for( int k = 0; k < RM->N; k++ )
					for( int l = 0; l < RM->N; l++ )
					{
						RM->assemblies[id].material_ids[k][l] = pick_mat(&seed);
						(RM->total_spatial_bins)++;
					}
				id++;
			}
			lower_left.x += RM->assembly_pitch;
		}
		lower_left.x = 0;
		lower_left.y += RM->assembly_pitch;
	}

	return RM;
}
