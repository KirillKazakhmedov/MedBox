#include "filter.h"
#include "Recorder.h"
#include <Windows.h>
#include <thread>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;
using namespace Service;
using namespace Service::Imaging;
using namespace Service::Saving;

Filter::Filter(ImageCollection* collection)
{
	this->collection = collection;

	SYSTEM_INFO info;
	GetSystemInfo(&info);
	cores = (short)info.dwNumberOfProcessors;
}

void Filter::GaussianFilter(short kernel_size, float sigma)
{
	//Check valid kernel size
	if (!kernelSizeIsValid(kernel_size))
		return;

	vector<vector<float>> matrix = getGaussianKernel(kernel_size, sigma);
	short count = collection->GetCount();

	if (count <= 30)
	{
		thread thr(&Filter::gauss_function, this, matrix, 0, count);
		thr.join();
		return;
	}

	short piece = count / cores;
	thread* threads = new thread[cores];

	for (short i = 0; i < cores; ++i)
	{
		if (cores - i == 1)
			threads[i] = thread(&Filter::gauss_function, this, matrix, (i * piece), count);

		else
			threads[i] = thread(&Filter::gauss_function, this, matrix, i * piece, (i + 1) * piece);
	}

	for (short i = 0; i < cores; ++i)
		threads[i].join();
}

void Filter::MedianFilter(short kernelSize)
{
	if (!kernelSizeIsValid(kernelSize))
		return;

	short count = collection->GetCount();

	if (count <= 30)
	{
		thread thr(&Filter::median_function, this, kernelSize, 0, count);
		thr.join();
		return;
	}

	short piece = count / cores;
	thread* threads = new thread[cores];

	for (short i = 0; i < cores; ++i)
	{
		if (cores - i == 1)
			threads[i] = thread(&Filter::median_function, this, kernelSize, (i * piece), count);

		else
			threads[i] = thread(&Filter::median_function, this, kernelSize, i * piece, (i + 1) * piece);
	}

	for (short i = 0; i < cores; ++i)
		threads[i].join();
}

//This function apply mean filter for input image
void Filter::MeanFilter(short kernelSize)
{
	if (!kernelSizeIsValid(kernelSize))
		return;

	short count = collection->GetCount();

	if (count <= 30)
	{
		thread thr(&Filter::mean_function, this, kernelSize, 0, count);
		thr.join();
		return;
	}

	short piece = count / cores;
	thread* threads = new thread[cores];

	for (short i = 0; i < cores; ++i)
	{
		if (cores - i == 1)
			threads[i] = thread(&Filter::mean_function, this, kernelSize, (i * piece), count);

		else
			threads[i] = thread(&Filter::mean_function, this, kernelSize, i * piece, (i + 1) * piece);
	}

	for (short i = 0; i < cores; ++i)
		threads[i].join();
}

void Filter::ErosionFilter(short kernelSize)
{
	if (!kernelSizeIsValid(kernelSize))
		return;

	short count = collection->GetCount();

	if (count <= 30)
	{
		thread thr(&Filter::erosion_function, this, kernelSize, 0, count);
		thr.join();
		return;
	}

	short piece = count / cores;
	thread* threads = new thread[cores];

	for (short i = 0; i < cores; ++i)
	{
		if (cores - i == 1)
			threads[i] = thread(&Filter::erosion_function, this, kernelSize, (i * piece), count);

		else
			threads[i] = thread(&Filter::erosion_function, this, kernelSize, i * piece, (i + 1) * piece);
	}

	for (short i = 0; i < cores; ++i)
		threads[i].join();
}

void Filter::DilationFilter(short kernelSize)
{
	if (!kernelSizeIsValid(kernelSize))
		return;

	short count = collection->GetCount();

	if (count <= 30)
	{
		thread thr(&Filter::dilation_function, this, kernelSize, 0, count);
		thr.join();
		return;
	}

	short piece = count / cores;
	thread* threads = new thread[cores];

	for (short i = 0; i < cores; ++i)
	{
		if (cores - i == 1)
			threads[i] = thread(&Filter::dilation_function, this, kernelSize, (i * piece), count);

		else
			threads[i] = thread(&Filter::dilation_function, this, kernelSize, i * piece, (i + 1) * piece);
	}

	for (short i = 0; i < cores; ++i)
		threads[i].join();
}

void Filter::SegmentationBySettedThreshold(short iso_surface)
{
	collection->SegmentationMark = SettedThreshold;
	Image *images = collection->GetImages();
	short count = collection->GetCount();
	short rows = images[0].Rows;
	short columns = images[0].Columns;

	for (short k = 0; k < count; ++k)
	{
		for (short j = 0; j < rows; ++j)
		{
			for (short i = 0; i < columns; ++i)
			{
				if (images[k].Data[j][i] > iso_surface)
					images[k].Data[j][i] = 1;

				else
					images[k].Data[j][i] = 0;
			}
		}
	}
}

void Filter::gauss_function(vector<vector<float>> matrix, short start_index, short final_index)
{
	//Storage convolution matrix
	short kernelSize = matrix.size();

	float** kernel = new float*[kernelSize];
	for (short i = 0; i < kernelSize; ++i)
		kernel[i] = new float[kernelSize];

	for (short i = 0; i < kernelSize; ++i)
		for (short j = 0; j < kernelSize; ++j)
			kernel[i][j] = matrix[i][j];

	//Get extended images
	short*** extended_images = MakeExtendedImages(start_index, final_index, kernelSize);

	Image *images = collection->GetImages();
	short count = final_index - start_index;
	short rows = images[0].Rows;
	short columns = images[0].Columns;

	short sum = 0;

	//Apply Gaussian filter for image
	for (short p = 0; p < count; ++p)
	{
		for (short k = 0; k < rows; ++k)
		{
			for (short l = 0; l < columns; ++l)
			{
				for (short i = 0; i < kernelSize; ++i)
				{
					for (short j = 0; j < kernelSize; ++j)
					{
						sum += (short)(kernel[i][j] * extended_images[p][j + k][i + l]);
					}
				}
				images[p + start_index].Data[k][l] = sum;
				sum = 0;
			}
		}
		cout << "Image " << p + start_index << " was handled" << "\n";
	}

	for (short i = 0; i < kernelSize; ++i)
		delete[] kernel[i];

	delete[] kernel;

	size_t new_image_rows = rows + 2 * (kernelSize / 2);
	for (int k = 0; k < count; ++k)
	{
		for (int j = 0; j < new_image_rows; ++j)
			delete[] extended_images[k][j];

		delete[] extended_images[k];
	}
	delete[] extended_images;
}

void Filter::median_function(short kernelSize, short start_index, short final_index)
{
	//Initialize new rows and columns values
	Image *images = collection->GetImages();
	short count = final_index - start_index;
	short rows = images[0].Rows;
	short columns = images[0].Columns;

	//Get extended images
	short*** extended_images = MakeExtendedImages(start_index, final_index, kernelSize);

	//Create a buffer for sorting array
	short buffer_size = kernelSize*kernelSize;
	short *buffer = new short[buffer_size];

	//Sorting array and apply median filter for input image
	for (short p = 0; p < count; ++p)
	{
		for (short k = 0; k < rows; ++k)
		{
			for (short l = 0; l < columns; ++l)
			{
				for (short i = 0; i < kernelSize; ++i)
				{
					for (short j = 0; j < kernelSize; ++j)
					{
						buffer[i*kernelSize + j] = extended_images[p][i + k][j + l];
					}
				}
				images[p + start_index].Data[k][l] = getMedianValue(buffer, buffer_size, 0, buffer_size - 1);
			}
		}
		cout << "Image " << p + start_index << " was handled" << "\n";
	}
	delete[] buffer;

	short new_image_rows = rows + 2 * (kernelSize / 2);
	for (int k = 0; k < count; ++k)
	{
		for (int j = 0; j < new_image_rows; ++j)
			delete[] extended_images[k][j];

		delete[] extended_images[k];
	}
	delete[] extended_images;
}

void Filter::mean_function(short kernelSize, short start_index, short final_index)
{
	//Initialize new rows and columns values
	Image *images = collection->GetImages();
	short count = final_index - start_index;
	short rows = images[0].Rows;
	short columns = images[0].Columns;

	//Get extended images
	short*** extended_images = MakeExtendedImages(start_index, final_index, kernelSize);

	//Initialize matrix element of mean filter
	double matrix_element = 1.0 / (kernelSize*kernelSize);

	//New pixel values will be put here
	short sum = 0;

	//Apply mean filter for image
	for (short p = 0; p < count; ++p)
	{
		for (short k = 0; k < rows; ++k)
		{
			for (short l = 0; l < columns; ++l)
			{
				for (short i = 0; i < kernelSize; ++i)
				{
					for (short j = 0; j < kernelSize; ++j)
					{
						sum += (short)(matrix_element * extended_images[p][j + k][i + l]);
					}
				}
				images[p + start_index].Data[k][l] = sum;
				sum = 0;
			}
		}
		cout << "Image " << p + start_index << " was handled" << "\n";
	}

	unsigned short new_image_rows = rows + 2 * (kernelSize / 2);
	for (int k = 0; k < count; ++k)
	{
		for (int j = 0; j < new_image_rows; ++j)
			delete[] extended_images[k][j];

		delete[] extended_images[k];
	}
	delete[] extended_images;
}

void Filter::erosion_function(short kernelSize, short start_index, short final_index)
{
	//To storage pixels area in series
	short buffer_size = (kernelSize * kernelSize) / 2 + 1;
	short* buffer = new short[buffer_size];

	//Initialize new rows and columns values
	Image *images = collection->GetImages();
	short count = final_index - start_index;
	short rows = images[0].Rows;
	short columns = images[0].Columns;

	//Get extended images
	short*** extended_images = MakeExtendedImages(start_index, final_index, kernelSize);

	//This algorithm form area of matrix from pixels
	for (short p = 0; p < count; ++p)
	{
		for (short k = 0; k < rows; ++k)
		{
			for (short l = 0; l < columns; ++l)
			{
				short index = 0;
				for (short i = 0; i < kernelSize; ++i)
				{
					if (i <= kernelSize / 2)
						for (short j = kernelSize / 2 - i; (j >= 0) && (j <= kernelSize / 2 + i); ++j)
						{
							buffer[index] = extended_images[p][i + k][j + l];
							index++;
						}

					else
						for (short j = i - kernelSize / 2; j < kernelSize - (i - kernelSize / 2); ++j)
						{
							buffer[index] = extended_images[p][i + k][j + l];
							index++;
						}
				}
				images[p + start_index].Data[k][l] = getMinValue(buffer, buffer_size);
			}
		}
		cout << "Image " << p + start_index << " was handled" << "\n";
	}
	delete[] buffer;

	unsigned short new_image_rows = rows + 2 * (kernelSize / 2);
	for (int k = 0; k < count; ++k)
	{
		for (int j = 0; j < new_image_rows; ++j)
			delete[] extended_images[k][j];

		delete[] extended_images[k];
	}
	delete[] extended_images;
}

void Filter::dilation_function(short kernelSize, short start_index, short final_index)
{
	//To storage pixels area in series
	short buffer_size = (kernelSize * kernelSize) / 2 + 1;
	short* buffer = new short[buffer_size];

	//Initialize new rows and columns values
	Image *images = collection->GetImages();
	short count = final_index - start_index;
	short rows = images[0].Rows;
	short columns = images[0].Columns;

	//Get extended images
	short*** extended_images = MakeExtendedImages(start_index, final_index, kernelSize);

	//This algorithm form area of matrix from pixels
	for (short p = 0; p < count; ++p)
	{
		for (short k = 0; k < rows; ++k)
		{
			for (short l = 0; l < columns; ++l)
			{
				short index = 0;
				for (short i = 0; i < kernelSize; ++i)
				{
					if (i <= kernelSize / 2)
						for (short j = kernelSize / 2 - i; (j >= 0) && (j <= kernelSize / 2 + i); ++j)
						{
							buffer[index] = extended_images[p][i + k][j + l];
							index++;
						}

					else
						for (short j = i - kernelSize / 2; j < kernelSize - (i - kernelSize / 2); ++j)
						{
							buffer[index] = extended_images[p][i + k][j + l];
							index++;
						}
				}
				images[p + start_index].Data[k][l] = getMaxValue(buffer, buffer_size);
			}
		}
		cout << "Image " << p + start_index << " was handled" << "\n";
	}
	delete[] buffer;

	unsigned short new_image_rows = rows + 2 * (kernelSize / 2);
	for (int k = 0; k < count; ++k)
	{
		for (int j = 0; j < new_image_rows; ++j)
			delete[] extended_images[k][j];

		delete[] extended_images[k];
	}
	delete[] extended_images;
}

void Filter::OpenFunction(short kernelSize)
{
	ErosionFilter(kernelSize);

	DilationFilter(kernelSize);
}

void Filter::CloseFunction(short kernelSize)
{
	DilationFilter(kernelSize);

	ErosionFilter(kernelSize);
}

void Filter::OpenCloseFunction(short kernelSize)
{
	CloseFunction(kernelSize);

	OpenFunction(kernelSize);
}

void Filter::CloseOpenFunction(short kernelSize)
{
	OpenFunction(kernelSize);

	CloseFunction(kernelSize);
}

ImageCollection* Filter::GetHandledImageCollection()
{
	return this->collection;
}

vector<vector<float>> Filter::getGaussianKernel(short size, float sigma)
{
	//Create an array for storage convolution matrix
	vector<vector<float>>kernel(size);

	float x = 0, y = 0, sum = 0;
	float sigma_sq = sigma * sigma;

	//Filling coordinate values and getting element values of convolution matrix
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			x = -size / static_cast<float>(2 + i);
			y = -size / static_cast<float>(2 + j);
			kernel[i].resize(size);
			kernel[i][j] = (float)((1.0 / 2.0*M_PI*sigma_sq)*exp(-(x*x + y*y) / 2.0*sigma_sq));
			sum += kernel[i][j];
		}
	}
	//Normalizing convolution matrix
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			kernel[i][j] /= sum;
		}
	}
	return kernel;
}

short*** Filter::MakeExtendedImages(short start_index, short final_index, short kernelSize)
{
	short count = final_index - start_index;
	short rows = collection->GetImages()[0].Rows;
	short columns = collection->GetImages()[0].Columns;
	Image* images = collection->GetImages();

	short new_image_columns = columns + 2 * (kernelSize / 2);
	short new_image_rows = rows + 2 * (kernelSize / 2);

	//Make windows extended
	short*** extended_images = new short**[count];
	for (short i = 0; i < count; ++i) {
		extended_images[i] = new short*[new_image_rows];

		for (short j = 0; j < new_image_rows; ++j)
			extended_images[i][j] = new short[new_image_columns];
	}

	for (short k = 0; k < count; ++k)
	{
		short** data = images[k + start_index].Data;

		//Fill left lower square
		for (short i = (rows - kernelSize / 2); i < rows; ++i)
		{
			for (short j = 0; j < kernelSize / 2; ++j)
			{
				extended_images[k][i + 2 * (kernelSize / 2)][j] = data[i][j];
			}
		}

		//Fill right lower square
		for (short i = (rows - kernelSize / 2); i < rows; ++i)
		{
			for (short j = (columns - kernelSize / 2); j < columns; ++j)
			{
				extended_images[k][i + 2 * (kernelSize / 2)][j + 2 * (kernelSize / 2)] = data[i][j];
			}
		}

		//Fill field from left lower to right lower squares
		for (short i = (rows - kernelSize / 2); i < rows; ++i)
		{
			for (short j = 0; j < columns; ++j)
			{
				extended_images[k][i + 2 * (kernelSize / 2)][j + kernelSize / 2] = data[i][j];
			}
		}

		//Fill field from right upper to right lower squares  Заполняем границы от правого верхнего до правого нижнего квадрата
		for (short i = 0; i < rows; ++i)
		{
			for (short j = (columns - kernelSize / 2); j < columns; ++j)
			{
				extended_images[k][i + kernelSize / 2][j + 2 * (kernelSize / 2)] = data[i][j];
			}
		}

		//Fill left upper square
		for (short i = 0; i < kernelSize / 2; ++i)
		{
			for (short j = 0; j < kernelSize / 2; ++j)
			{
				extended_images[k][i][j] = data[i][j];
			}
		}

		//Fill right upper square
		for (short i = 0; i < kernelSize / 2; ++i)
		{
			for (short j = (columns - kernelSize / 2); j < columns; ++j)
			{
				extended_images[k][i][j + 2 * (kernelSize / 2)] = data[i][j];
			}
		}

		//Fill field from left upper to right upper squares
		for (short i = 0; i < kernelSize / 2; ++i)
		{
			for (short j = 0; j < columns; ++j)
			{
				extended_images[k][i][j + kernelSize / 2] = data[i][j];
			}
		}

		//Fill field from left upper to left lower squares 
		for (short i = 0; i < rows; ++i)
		{
			for (short j = 0; j < kernelSize / 2; ++j)
			{
				extended_images[k][i + kernelSize / 2][j] = data[i][j];
			}
		}

		//Fill center
		for (short i = 0; i < rows; ++i)
		{
			for (short j = 0; j < columns; ++j)
			{
				extended_images[k][i + kernelSize / 2][j + kernelSize / 2] = data[i][j];
			}
		}
	}
	return extended_images;
}

bool Filter::kernelSizeIsValid(short kernel_size)
{
	//Check odd and non-negative
	if (kernel_size % 2 == 0 || kernel_size < 0)
		return false;

	return true;
}

void Filter::WriteSliceToFile(short index, string fileName)
{
	Image image = collection->GetImages()[index];
	Recodrer::WriteSliceToBinaryFile(image, fileName);
}
