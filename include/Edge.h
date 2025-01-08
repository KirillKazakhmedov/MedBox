#pragma once

#include "Vertex.h"
#include "IndexedVertex.h"
#include "LineSegment.h"

using namespace Service::Modeling::Geometry;

namespace Service
{
	namespace Modeling
	{
		namespace Geometry
		{
			class Edge : public LineSegment
			{
			public:
				unsigned int vertex_indices[2];
				Vertex center;
				unsigned int id;

				Edge() : LineSegment() { }
				Edge(Indexed_Vertex &a, Indexed_Vertex &b) : LineSegment(a, b)
				{
					if (a.index < b.index)
					{
						vertex_indices[0] = a.index;
						vertex_indices[1] = b.index;
					}

					else
					{
						vertex_indices[0] = b.index;
						vertex_indices[1] = a.index;
					}

					center.x = (a.x + b.x) * 0.5f;
					center.y = (a.y + b.y) * 0.5f;
					center.z = (a.z + b.z) * 0.5f;
				}

				inline bool Edge::operator==(const Edge &right)
				{
					if (vertex_indices[0] == right.vertex_indices[0] && vertex_indices[1] == right.vertex_indices[1])
						return true;

					else
						return false;
				}

				inline bool Edge::operator<(const Edge &right) const
				{
					if (vertex_indices[0] < right.vertex_indices[0])
						return true;
					else if (vertex_indices[0] > right.vertex_indices[0])
						return false;

					if (vertex_indices[1] < right.vertex_indices[1])
						return true;
					else if (vertex_indices[1] > right.vertex_indices[1])
						return false;

					return false;
				}
			};
		}
	}
}
