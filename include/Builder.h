#pragma once

#include "Triangle.h"
#include "LookUpTable.h"
#include "grid.h"
#include "ImageCollection.h"
#include "CellSize.h"

#include <list>
#include <iostream>

using namespace std;
using namespace Service::Imaging;

namespace Service
{
	namespace Modeling
	{
		class Builder {

		private:
			short iso_surface;
			Imaging::Segmentation segmentation_mark;
			bool standartMC;
			GridCell cell;
			Image* images;
			float dx, dy, dz;
			short _case, _config, _subconfig, cell_size;

			Vertex getIntersection(short);
			void getVertices(short, short *);
			short getNodeCaseNumber();
			void addTriangles(list<Triangle> &, const __int8[], short);
			bool testFace(__int8);
			bool testInterior(__int8);

			inline void addAdditionalVertex()
			{
				cell.additional_vertex = Vertex(0, 0, 0);
				short count_ = 0;

				for (short i = 0; i < 8; ++i)
				{
					cell.additional_vertex += cell.vertex[i];
					count_++;
				}
				cell.additional_vertex = cell.additional_vertex / static_cast<float>(count_);
			}


		public:
			Builder(ImageCollection*, short, bool, const short size = 1);
			Builder(ImageCollection*, bool, const short size = 1);

			bool Build(short, short, short);
			list<Triangle> getTriangles();
		};
	}
}
