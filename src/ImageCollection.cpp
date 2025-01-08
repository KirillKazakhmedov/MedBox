#include "ImageCollection.h"

using namespace Service;

ImageCollection::ImageCollection(short size)
{
	this->size = size;
	this->images = new Image[size];
}

void ImageCollection::Add(Image image)
{
	if (count == -1)
		count++;

	if (!IsFull())
	{
		images[count] = image;
		count++;
	}
}

void ImageCollection::Clear()
{
	if (count != -1)
	{
		for (short i = 0; i < count; ++i)
		{
			Image image = images[i];

			short** data = image.Data;
			for (short j = 0; j < image.Columns; ++j)
				delete[] data[j];

			delete[] data;
		}

		delete[] images;
		count = -1;
	}
}

short ImageCollection::GetCount()
{
	return this->count;
}

Image* ImageCollection::GetImages()
{
	return this->images;
}

bool ImageCollection::IsEmpty()
{
	if (count == -1)
		return true;

	else
		return false;
}

bool ImageCollection::IsFull()
{
	if (count == size)
		return true;

	else
		return false;
}
