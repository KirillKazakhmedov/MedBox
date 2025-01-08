#include <string>
#include <conio.h>

#include "ApplicationFactory.h"
#include "filter.h"
#include "MarchingCube.h"
#include "Smoother.h"
#include "Decimator.h"
#include "Recorder.h"
#include "Mesh.h"
#include "CellSize.h"

using namespace std;
using namespace Service;
using namespace Service::Imaging;
using namespace Service::Modeling;
using namespace Service::Smoothing;
using namespace Service::Decimation;
using namespace Service::Saving;

int main(int argc, char *argv[])
{
	//Load tomography slices
	ApplicationFactory::Initializer(argv);
	ImageCollection *collection = ApplicationFactory::GetImageCollection();

	short iso_surface = 100;
	short kernel_size = 5;
	float sigma = 0.4f;

	Filter *filter = new Filter(collection);
	filter->GaussianFilter(kernel_size, sigma);
	//filter->SegmentationBySettedThreshold(iso_surface);

	//Building model
	MarchingCube *cube = new MarchingCube(collection, false, CellSize::Two);
	cube->March(iso_surface);

	//Repairing the mesh
	Mesh* mesh = cube->GetMesh();
	mesh->RepairModel();

	//Writing the mesh
	list<Triangle>& triangles = mesh->GetTriangleList();
	string fileName = "D:/Study/Kursach/Project/Models/Test33.stl";
	Recodrer::WriteModelToBinarySTL(triangles, fileName);

	//Using Taubin smooth algorithm for model with Fujiwara operator
	Smoother* smoother = new Smoother(mesh);
	smoother->TaubinSmooth(0.55f, -0.6f, 15);
	mesh = smoother->GetSmoothedMesh();

	//Writing smoothed mesh
	string fileName1 = "D:/Study/Kursach/Project/Models/Test33_smoothed.stl";
	Recodrer::WriteModelToBinarySTL(triangles, fileName1);

	collection->Clear();
	delete filter;
	delete cube;
	delete mesh;
	delete smoother;
	ApplicationFactory::Clear();

	cout << "Working have been finished!\n";
	getch();
	return 0;
}
