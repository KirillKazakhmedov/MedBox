#pragma once

//dcmtk library
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimage/diregist.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmjpeg/djdecode.h"
#include "dcmtk/dcmjpls/djdecode.h"
#include <boost\lambda\bind.hpp>
#include <boost\filesystem.hpp>

#include "ImageCollection.h"

using namespace std;
using namespace boost::filesystem;
using namespace boost::lambda;

namespace Service
{
	class ApplicationFactory
	{
	public:
		static void Initializer(char *argv[]);
		static ImageCollection* GetImageCollection();
		static void Clear();

	private:
		static ImageCollection *images;
		static Image extractPixelsData(DicomImage*, short, short, short);
	};
}