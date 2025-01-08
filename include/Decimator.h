#pragma once

#include <list>
#include "Triangle.h"

using namespace Service::Modeling::Geometry;

namespace Service
{
	namespace Decimation
	{
		class Decimator
		{
		public:
			Decimator(std::list<Triangle>&);
			std::list<Triangle>& GetDecimatedMesh();

		private:
			std::list<Triangle>& triangles;
		};
	}
}
