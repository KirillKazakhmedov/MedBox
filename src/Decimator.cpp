#include "Decimator.h"

using namespace Service::Decimation;
using namespace Service::Modeling;
using namespace std;

Decimator::Decimator(list<Triangle>& Triangles) : triangles(Triangles)
{ }

list<Triangle>& Decimator::GetDecimatedMesh()
{
	return triangles;
}