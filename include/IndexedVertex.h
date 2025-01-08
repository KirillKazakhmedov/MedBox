#pragma once

#include "Vertex.h"

namespace Service
{
	namespace Modeling
	{
		namespace Geometry
		{
			class Indexed_Vertex : public Vertex
			{
			public:
				unsigned int index;

				Indexed_Vertex() : Vertex()
				{
					this->index = 0;
				}

				Indexed_Vertex(float x, float y, float z, unsigned int index) : Vertex(x, y, z)
				{
					this->index = index;
				}

				inline bool operator<(const Indexed_Vertex &right) const
				{
					if (right.x > x)
						return true;
					else if (right.x < x)
						return false;

					if (right.y > y)
						return true;
					else if (right.y < y)
						return false;

					if (right.z > z)
						return true;
					else if (right.z < z)
						return false;

					return false;
				}
			};
		}
	}
}