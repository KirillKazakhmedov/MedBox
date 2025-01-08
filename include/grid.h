#pragma once

#include "Vertex.h"

using namespace Service::Modeling::Geometry;

namespace Service 
{
	namespace Modeling 
	{
		class GridCell
		{
		public:
			Vertex additional_vertex;
			Vertex vertex[8];
			short value[8];
			bool nodeParity[8];

			GridCell() { }
			GridCell(Vertex vertex[8], short value[8])
			{
				for (short i = 0; i < 8; ++i)
				{
					this->vertex[i] = vertex[i];
					this->value[i] = value[i];
				}
			}
		};
	}
}