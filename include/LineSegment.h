#pragma once

#include "Vertex.h"
#include "Vector.h"

namespace Service
{
	namespace Modeling
	{
		namespace Geometry
		{
			class LineSegment
			{
			public:
				LineSegment() { }
				LineSegment(Vertex a, Vertex b)
				{
					vertices[0] = a;
					vertices[1] = b;
					vector = vertices[1] - vertices[0];
				}

				inline Vertex* LineSegment::GetVertices()
				{
					return vertices;
				}

				inline Vector LineSegment::GetVector()
				{
					return vector;
				}

				inline bool LineSegment::IsIntersection(const LineSegment &line) const
				{
					//From initial point of the first line to second line points
					LineSegment lines1[] = { LineSegment(vertices[0], line.vertices[0]), 
											 LineSegment(vertices[0], line.vertices[1]) };

					//From initial point of the second line to first line points
					LineSegment lines2[] = { LineSegment(line.vertices[0], vertices[0]), 
											 LineSegment(line.vertices[0], vertices[1]) };

					//Single component not equal zero only
					float result1[2] = { (vector.Ny * lines1[0].vector.Nz - vector.Nz * lines1[0].vector.Ny) +
										 (vector.Nz * lines1[0].vector.Nx - vector.Nx * lines1[0].vector.Nz) +
										 (vector.Nx * lines1[0].vector.Ny - vector.Ny * lines1[0].vector.Nx),

										 (vector.Ny * lines1[1].vector.Nz - vector.Nz * lines1[1].vector.Ny) +
										 (vector.Nz * lines1[1].vector.Nx - vector.Nx * lines1[1].vector.Nz) +
										 (vector.Nx * lines1[1].vector.Ny - vector.Ny * lines1[1].vector.Nx) };

					float result2[2] = { (line.vector.Ny * lines2[0].vector.Nz - line.vector.Nz * lines2[0].vector.Ny) +
										 (line.vector.Nz * lines2[0].vector.Nx - line.vector.Nx * lines2[0].vector.Nz) +
										 (line.vector.Nx * lines2[0].vector.Ny - line.vector.Ny * lines2[0].vector.Nx),

										 (line.vector.Ny * lines2[1].vector.Nz - line.vector.Nz * lines2[1].vector.Ny) +
										 (line.vector.Nz * lines2[1].vector.Nx - line.vector.Nx * lines2[1].vector.Nz) +
										 (line.vector.Nx * lines2[1].vector.Ny - line.vector.Ny * lines2[1].vector.Nx) };

					bool flags[2] = { false, false };

					if ((result1[0] > 0 && result1[1] < 0) || (result1[0] < 0 && result1[1] > 0))
						flags[0] = true;

					if ((result2[0] > 0 && result2[1] < 0) || (result2[0] < 0 && result2[1] > 0))
						flags[1] = true;

					if (flags[0] && flags[1])
						return true;

					return false;
				}

				inline LineSegment& LineSegment::ProjectToXY()
				{
					vertices[0].z = 0.0f;
					vertices[1].z = 0.0f;
					vector = vertices[1] - vertices[0];

					return *this;
				}

				inline LineSegment& LineSegment::ProjectToXZ()
				{
					vertices[0].y = 0.0f;
					vertices[1].y = 0.0f;
					vector = vertices[1] - vertices[0];

					return *this;
				}

				inline LineSegment& LineSegment::ProjectToYZ()
				{
					vertices[0].x = 0.0f;
					vertices[1].x = 0.0f;
					vector = vertices[1] - vertices[0];

					return *this;
				}

			private:
				Vertex vertices[2];
				Vector vector;
			};
		}
	}
}