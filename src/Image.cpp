#include "Image.h"

using namespace Service;

Image::Image() { }

Image::Image(short rows, short columns, short** data)
{
	Rows = rows;
	Columns = columns;
	Data = data;
}

Image::Image(short rows, short columns)
{
	Rows = rows;
	Columns = columns;

	Data = new short*[rows];
	for (short i = 0; i < columns; i++)
		Data[i] = new short[columns];
}
