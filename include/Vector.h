#pragma once

#include <math.h>
#include "Vertex.h"

namespace Service 
{
	namespace Modeling
	{
		namespace Geometry
		{

			class Vector 
			{
			public:
				float Nx, Ny, Nz;

				Vector() { }
				Vector(Vertex v[])
				{
					this->Nx = (v[1].y - v[0].y) * (v[2].z - v[0].z) - (v[1].z - v[0].z) * (v[2].y - v[0].y);
					this->Ny = (v[1].z - v[0].z) * (v[2].x - v[0].x) - (v[1].x - v[0].x) * (v[2].z - v[0].z);
					this->Nz = (v[1].x - v[0].x) * (v[2].y - v[0].y) - (v[1].y - v[0].y) * (v[2].x - v[0].x);
				}

				Vector(float values[])
				{
					this->Nx = values[0];
					this->Ny = values[1];
					this->Nz = values[2];
				}

				Vector(float Nx, float Ny, float Nz)
				{
					this->Nx = Nx;
					this->Ny = Ny;
					this->Nz = Nz;
				}

				inline void Vector::Normalize()
				{
					float length = GetLength();

					if (length != 0.0f)
					{
						this->Nx = this->Nx / length;
						this->Ny = this->Ny / length;
						this->Nz = this->Nz / length;
					}
				}

				inline float Vector::GetLength()
				{
					return (float)sqrt((Nx * Nx) + (Ny * Ny) + (Nz * Nz));
				}

				//Inline operators--------------------------------
				inline Vector& operator=(const Vertex &right)
				{
					this->Nx = right.x;
					this->Ny = right.y;
					this->Nz = right.z;

					return *this;
				}

				inline Vector operator+(const Vector &right) const
				{
					Vector n;

					n.Nx = Nx + right.Nx;
					n.Ny = Ny + right.Ny;
					n.Nz = Nz + right.Nz;

					return n;
				}

				inline Vector& operator=(const Vector &right)
				{
					this->Nx = right.Nx;
					this->Ny = right.Ny;
					this->Nz = right.Nz;

					return *this;
				}
			};
		}
	}
}