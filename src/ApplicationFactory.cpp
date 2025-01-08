#include "ApplicationFactory.h"

using namespace Service;

ImageCollection* ApplicationFactory::images;

void ApplicationFactory::Initializer(char *argv[])
{
	Float64 sliceLocation1;
	Float64 sliceLocation2;
	Float64 x_pixelSpacing;
	Float64 y_pixelSpacing;
	Float64 sliceSpacing;
	Uint16 rows;
	Uint16 columns;
	int count;

	//Getting count file in current directory
	count = count_if(directory_iterator(argv[1]), directory_iterator(), static_cast<bool(*)(const path&)>(is_regular_file));
	images = new ImageCollection(count);

	std::cout << "Images was found: " << count << "\n";

	short current = 0;
	for (recursive_directory_iterator it(argv[1]), end; it != end; it++)
	{
		DcmFileFormat fileformat;
		string file_name;

		file_name = it->path().string();
		OFCondition status = fileformat.loadFile(file_name.c_str());

		if (current == 0)
		{
			if (status.good())
			{
				//Getting rows data
				if (fileformat.getDataset()->findAndGetUint16(DCM_Rows, rows).good())
				{
					cout << "Rows: " << rows << endl;
				}
				//Getting columns data
				if (fileformat.getDataset()->findAndGetUint16(DCM_Columns, columns).good())
				{
					cout << "Columns: " << columns << endl;
				}
				//Getting x pixels spacing
				if (fileformat.getDataset()->findAndGetFloat64(DCM_PixelSpacing, x_pixelSpacing).good())
				{
					cout << "Pixels spacing, X: " << x_pixelSpacing << "\n";
					images->XLength = (float)x_pixelSpacing;
				}
				//Getting y pixels spacing
				if (fileformat.getDataset()->findAndGetFloat64(DCM_PixelSpacing, y_pixelSpacing, 1).good())
				{
					cout << "Pixels spacing, Y: " << y_pixelSpacing << "\n";
					images->YLength = (float)y_pixelSpacing;
				}

				//Getting location for the first slice
				if (fileformat.getDataset()->findAndGetFloat64(DCM_SliceLocation, sliceLocation1).good()) { }

				////Getting x coordinate for first pixel
				//if (fileformat.getDataset()->findAndGetFloat64(DCM_ImagePositionPatient, x_imagePosition).good())
				//{
				//	cout << "Image position, X: " << x_imagePosition << "\n";
				//}
				////Getting y coordinate for first pixel
				//if (fileformat.getDataset()->findAndGetFloat64(DCM_ImagePositionPatient, y_imagePosition, 1).good())
				//{
				//	cout << "Image position, Y: " << y_imagePosition << "\n";
				//}
				////Getting z coordinate for first pixel
				//if (fileformat.getDataset()->findAndGetFloat64(DCM_ImagePositionPatient, z_imagePosition).good(), 2)
				//{
				//	cout << "Image position, Z: " << z_imagePosition << "\n";
				//}
			}
		}

		if (current == 1)
		{
			//Getting location for the second slice
			if (fileformat.getDataset()->findAndGetFloat64(DCM_SliceLocation, sliceLocation2).good())
			{
				sliceSpacing = fabs(sliceLocation1 - sliceLocation2);
				cout << "Slice spacing: " << sliceSpacing << "\n";
				images->ZLength = (float)sliceSpacing;
			}
		}

		DicomImage *image = new DicomImage(file_name.c_str());
		Image im;

		switch (image->getStatus())
		{
		case EIS_Normal:
			im = extractPixelsData(image, count, rows, columns);
			break;

		case EIS_MissingAttribute:
			DJDecoderRegistration::registerCodecs(); // register JPEG codecs
			if (status.good())
			{
				DcmDataset *dataset = fileformat.getDataset();

				// decompress data set if compressed
				dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);

				// check if everything went well
				if (dataset->canWriteXfer(EXS_LittleEndianExplicit))
				{
					delete image;
					fileformat.saveFile(file_name.c_str(), EXS_LittleEndianExplicit);

					DicomImage *img = new DicomImage(file_name.c_str());
					im = extractPixelsData(img, count, rows, columns);
				}
			}
			DJDecoderRegistration::cleanup(); // deregister JPEG codecs
			break;
		}
		images->Add(im);
		delete image;
		current++;
	}
}

Image ApplicationFactory::extractPixelsData(DicomImage *image, short image_count, short rows, short columns)
{
	const DiPixel *inter = image->getInterData();
	Image im(rows, columns);

	if (inter != NULL)
	{
		short *raw_pixel_data = (short *)inter->getData();

		if (raw_pixel_data == nullptr)
		{
			cout << "Couldn't acces pixel data!\n";
			exit(1);
		}

		for (int j = 0; j < rows; ++j)
		{
			for (int i = 0; i < columns; ++i)
				im.Data[j][i] = *(raw_pixel_data + j*columns + i);
		}
	}
	return im;
}

void ApplicationFactory::Clear()
{
	delete images;
}

ImageCollection* ApplicationFactory::GetImageCollection()
{
	return images;
}