#pragma once

#include <list>
#include <vector>
#include "Triangle.h"
#include "IndexedTriangle.h"
#include "Mesh.h"

using namespace std;
using namespace Service::Modeling;

namespace Service
{
	namespace Smoothing
	{
		class Smoother
		{
		public:
			Smoother(Mesh*);
			Smoother();
			void TaubinSmooth(const float lambda, const float mu, const unsigned short iterations);
			Mesh* GetSmoothedMesh();

		private:
			Mesh* mesh;
			void rebuildMesh();
			void laplaceSmooth(const float scale);
		};
	}
}