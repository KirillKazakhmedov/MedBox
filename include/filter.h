#pragma once

#include <vector>
#include "ImageCollection.h"

using namespace std;

namespace Service
{
	namespace Imaging
	{
	
		class Filter
		{
		public:
			//CreateFilters class constructor
			Filter(ImageCollection*);

			//This function apply Gaussian filter for input image 
			void GaussianFilter(short, float);

			//This function apply median filter for input image
			void MedianFilter(short);

			//This function apply mean filter for input image
			void MeanFilter(short);

			//This function apply erosion filter for input image
			void ErosionFilter(short);

			//This function apply dilation filter for input image
			void DilationFilter(short);

			//This function apply open operation for input image
			void OpenFunction(short);

			//This function apply close operation for input image
			void CloseFunction(short);

			//This function apply close and open operation for input image successively
			void OpenCloseFunction(short);

			//This function apply open and close operation for input image successively
			void CloseOpenFunction(short);

			//Binary segmentation function
			void SegmentationBySettedThreshold(short iso_surface);

			//Getting handled image
			ImageCollection* GetHandledImageCollection();

			//Write in a binary file
			void WriteSliceToFile(short, string);

			//Additional functions
		private:
			ImageCollection* collection;
			short cores;

			void gauss_function(vector<vector<float>>, short, short);
			void median_function(short, short, short);
			void mean_function(short, short, short);
			void erosion_function(short, short, short);
			void dilation_function(short, short, short);

			//This finction build convolution matrix
			vector<vector<float>> getGaussianKernel(short, float);

			//This function check size of matrix convolution
			bool kernelSizeIsValid(short);

			//This function fill window
			short*** MakeExtendedImages(short, short, short);

			inline short getMedianValue(short buffer[], short buffer_size, short b, short e)
			{
				short l = b, r = e;
				short piv = buffer[(l + r) / 2];
				while (l <= r)
				{
					while (buffer[l] < piv)
						l++;
					while (buffer[r] > piv)
						r--;
					if (l <= r)
						swap(buffer[l++], buffer[r--]);
				}
				if (b < r)
					getMedianValue(buffer, buffer_size, b, r);
				if (e > l)
					getMedianValue(buffer, buffer_size, l, e);

				return buffer[buffer_size / 2];
			} 

			inline short getMinValue(short buffer[], short buffer_size)
			{
				short min = buffer[0];

				for (short i = 0; i < buffer_size; ++i)
					if (buffer[i] < min)
						min = buffer[i];

				return min;
			}

			inline short getMaxValue(short buffer[], short buffer_size)
			{
				short max = buffer[0];

				for (short i = 0; i < buffer_size; ++i)
					if (buffer[i] > max)
						max = buffer[i];

				return max;
			}
		};
	}
}