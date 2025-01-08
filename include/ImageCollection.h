#pragma once

#include "Image.h"
#include "Segmentation.h"

namespace Service
{
	class ImageCollection
	{
	public:
		ImageCollection(short);

		Image* GetImages();
		short GetCount();
		void Add(Image);
		void Clear();
		bool IsEmpty();
		bool IsFull();

		float XLength, YLength, ZLength;
		Imaging::Segmentation SegmentationMark = Imaging::Segmentation::Null;

	private:
		short size;
		Image* images;
		short count = -1;
		/*double x_imagePosition;
		double y_imagePosition;
		double z_imagePosition;*/
	};
}
