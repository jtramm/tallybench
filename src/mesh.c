#include "tallybench_header.h"

typedef struct{
	double x;
	doubly y;
} Coord;

typedef struct{
	int N;
	int ** material_ids;
	double pin_pitch;
	Coord lower_left;
} Assembly_mesh;

typedef struct{
	int N;
	int ** assembly_ids;
	double assembly_pitch;
	Assembly_Mesh * assemblies;
	Coord lower_left;
} Reactor_Mesh;

Reactor_Mesh * build_reactor_mesh(void)
{
	Reactor_Mesh * RM = malloc(sizeof(Reactor_Mesh));

	RM->N = N;
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

	printf("allocating space for %d assemblies...\n", id);

	// Allocate room for actual number of assemblies
	RM->assemblies = malloc(id * sizeof(Assembly_Mesh));
	id = 0;
	Coord lower_left = {0, 0};
	for( int i = 0 ; i < RM->N; i++ )
	{
		for( int j = 0 ; j < RM->N; j++ )
		{
			// For a valid assembly, 
			if( RM->assembly_ids[i][j] != -1 )
			{
				RM->assemblies[id].N = RN->N;
				RM->assemblies[id].pin_pitch = 1.26;
				RM->assemblies[id].material_ids = imatrix(RN->N, RN->N);
				RM->assemblies[id].lower_left = lower_left;

				// Fill assembly with random materials (from realistic distribution)
				unsigned long seed = 42;
				for( int k = 0; k < RN->N; k++ )
					for( int l = 0; l < RN->N; l++ )
						RM->assemblies[id].materials_ids[k][l] = pick_mat(&seed);
			}
			id++;
			lower_left.x += RM->assembly_pitch;
		}
		lower_left.x = 0;
		lower_left.y += RM->assembly_pitch;
	}


}
