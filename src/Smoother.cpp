#include <iostream>
#include <fstream>
#include <set>
#include <numeric>
#include <string>

#include "Smoother.h"
#include "IndexedVertex.h"

using namespace std;
using namespace Service::Smoothing;

Smoother::Smoother()
{ }

Smoother::Smoother(Mesh* mesh)
{
	this->mesh= mesh;
}

void Smoother::TaubinSmooth(const float lambda, const float mu, const unsigned short iterations)
{
	cout << "Smoother use Taubin lambda|mu algorithm ";
	cout << "(inverse neighbour count weighting)" << "\n";

	for (unsigned short i = 0; i < iterations; ++i)
	{
		cout << "Iteration " << i + 1 << " of " << iterations << "\n";

		laplaceSmooth(lambda);
		laplaceSmooth(mu);
	}
	rebuildMesh();
}

void Smoother::laplaceSmooth(const float scale)
{
	if (mesh != nullptr)
	{
		vector<Vertex>& vertices = mesh->GetUniqueVertices();
		vector<vector<unsigned int>>& vertex_to_vertex_indices = mesh->GetVertexListToVertexIndices();

		Vertex *displacements = new Vertex[vertices.size()];
		for (unsigned int i = 0; i < vertices.size(); ++i)
			displacements[i] = Vertex(0, 0, 0);

		// Get per-vertex displacement.
		for (size_t i = 0; i < vertices.size(); i++)
		{
			// Skip rogue vertices (which were probably made rogue during a previous
			// attempt to fix mesh cracks).
			if (0 == vertex_to_vertex_indices[i].size())
				continue;

			const float weight = 1.0f / static_cast<float>(vertex_to_vertex_indices[i].size());

			for (size_t j = 0; j < vertex_to_vertex_indices[i].size(); j++)
			{
				size_t neighbour_j = vertex_to_vertex_indices[i][j];
				displacements[i] += (vertices[neighbour_j] - vertices[i])*weight;
			}
		}

		for (unsigned int i = 0; i < vertices.size(); i++)
			vertices[i] += (displacements[i] * scale);

		delete[] displacements;
	}

	else
	{
		cout << "'MarchingCube' class object is empty!";
	}
}

void Smoother::rebuildMesh()
{
	if (mesh != nullptr)
	{
		vector<Indexed_Triangle>& tr = mesh->GetTrianglesWithIndexedVertices();
		vector<Vertex>& vertices = mesh->GetUniqueVertices();
		list<Triangle>& triangles = mesh->GetTriangleList();

		unsigned int tri_index = 0;
		for (list<Triangle>::iterator triangle = triangles.begin(); triangle != triangles.end(); ++triangle)
		{
			triangle->v[0] = vertices[tr[tri_index].vertex_indices[0]];
			triangle->v[1] = vertices[tr[tri_index].vertex_indices[1]];
			triangle->v[2] = vertices[tr[tri_index].vertex_indices[2]];

			triangle->RecalculateTriangleNormal();
			triangle->RecalculateTriangleQuality();

			tri_index++;
		}
		mesh->RecalculateQuality();
	}

	else
	{
		cout << "'Mesh' class object is empty!";
	}
}

Mesh* Smoother::GetSmoothedMesh()
{
	return this->mesh;
}