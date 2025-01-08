#pragma once

#include "Vertex.h"
#include "Vector.h"

#include <limits>

namespace Service
{
	namespace Modeling
	{
		namespace Geometry
		{
			class Plane
			{
			public:
				float A, B, C, D;

				Plane() { }
				Plane(Vertex vertex, Vector normal)
				{
					this->A = normal.Nx;
					this->B = normal.Ny;
					this->C = normal.Nz;

					float x = vertex.x;
					float y = vertex.y;
					float z = vertex.z;

					this->D = -(A*x + B*y + C*z);
				}

				inline bool Plane::IsLies(const Vertex &vertex) const
				{
					float distance = GetDistance(vertex);

					if (distance == 0)
						return true;

					return false;
				}

				inline float Plane::GetDistance(const Vertex &vertex) const
				{
					float distance = A*vertex.x + B*vertex.y + C*vertex.z + D;

					return distance;
				}
			};
		}
	}
}