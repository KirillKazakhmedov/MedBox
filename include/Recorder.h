#pragma once

#include "Triangle.h"
#include "Image.h"
#include <list>

using namespace std;
using namespace Service::Modeling::Geometry;

namespace Service {
	namespace Saving
	{
		class Recodrer : public exception
		{
		public:
			static bool WriteModelToBinarySTL(list<Triangle>&, string);
			static bool WriteModelToSTL(list<Triangle>&, string);
			static bool WriteModelToPLY(list<Triangle>&, string);
			static bool WriteSliceToBinaryFile(Image, string);

		private:
			virtual const char* what() const throw()
			{ return "Error was happened in during writing!"; }
		};
	}
}