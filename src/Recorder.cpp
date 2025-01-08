#include "Recorder.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <exception>

using namespace Service::Saving;

bool Recodrer::WriteSliceToBinaryFile(Service::Image image, string file_name)
{
	try
	{
		short rows = image.Rows;
		short** data = image.Data;

		ofstream file(file_name, ios_base::binary, ios_base::trunc);
		for (size_t i = 0; i < rows; ++i)
		{
			const char* pointer = reinterpret_cast<const char*>(&data[i][0]);
			file.write(pointer, sizeof(data[i][0]) * rows);
		}
		file.close();
		return true;
	}
	catch (exception& ex)
	{
		cout << ex.what() << "\n";
		return false;
	}
}

bool Recodrer::WriteModelToBinarySTL(list<Triangle> &triangles, string file_name)
{
	try
	{
		ofstream model(file_name.c_str(), ios_base::binary | ios_base::trunc);

		vector<char> buffer(80, '0');
		unsigned int triangle_num = triangles.size();

		cout << "Writing data..." << "\n";

		model.write(reinterpret_cast<const char *>(&buffer[0]), 80);
		model.write(reinterpret_cast<const char *>(&triangle_num), sizeof(triangle_num));
		unsigned short attribute_byte_count = 0;

		for (Triangle triangle : triangles)
		{
			float buffer_x = triangle.normal.Nx;
			float buffer_y = triangle.normal.Ny;
			float buffer_z = triangle.normal.Nz;

			model.write(reinterpret_cast<const char *>(&buffer_x), sizeof(buffer_x));
			model.write(reinterpret_cast<const char *>(&buffer_y), sizeof(buffer_y));
			model.write(reinterpret_cast<const char *>(&buffer_z), sizeof(buffer_z));

			for (short i = 0; i < 3; ++i)
			{
				buffer_x = triangle.v[i].x;
				buffer_y = triangle.v[i].y;
				buffer_z = triangle.v[i].z;

				model.write(reinterpret_cast<const char *>(&buffer_x), sizeof(buffer_x));
				model.write(reinterpret_cast<const char *>(&buffer_y), sizeof(buffer_y));
				model.write(reinterpret_cast<const char *>(&buffer_z), sizeof(buffer_z));
			}

			model.write(reinterpret_cast<const char *>(&attribute_byte_count), sizeof(attribute_byte_count));
		}

		model.close();
		cout << "Successful writing!\n";
		return true;
	}
	catch (exception& ex)
	{
		cout << ex.what() << "\n";
		return false;
	}
}

bool Recodrer::WriteModelToSTL(list<Triangle> &triangles, string file_name)
{
	try
	{
		ofstream model(file_name.c_str(), ios_base::trunc);

		model << "solid \n";

		for (Triangle triangle : triangles)
		{
			model << "facet normal " << triangle.normal.Nx << " " << triangle.normal.Ny << " " << triangle.normal.Nz << "\n";
			model << "outer loop\n";

			model << "vertex " << triangle.v[0].x << " " << triangle.v[0].y << " " << triangle.v[0].z << "\n";
			model << "vertex " << triangle.v[1].x << " " << triangle.v[1].y << " " << triangle.v[1].z << "\n";
			model << "vertex " << triangle.v[2].x << " " << triangle.v[2].y << " " << triangle.v[2].z << "\n";

			model << "end loop\n";
			model << "end facet\n";
		}
		model << "endsolid ";
		model.close();
		return true;
	}
	catch (exception& ex)
	{
		cout << ex.what() << "\n";
		return false;
	}
}

bool Recodrer::WriteModelToPLY(list<Triangle> &triangles, string file_name)
{
	try
	{
		ofstream model(file_name.c_str(), ios_base::trunc);

		char format[] = "ply";
		char extansion[] = "format ascii 1.0";
		string vertices_number = "element vertex " + to_string(3 * triangles.size());

		char prop_x[] = "property float x";
		char prop_y[] = "property float y";
		char prop_z[] = "property float z";

		string face = "element face " + to_string(triangles.size());
		char vert_indices[] = "property list uint uint vertex_indices";
		char end_header[] = "end_header";

		model << format << "\n";
		model << extansion << "\n";
		model << vertices_number << "\n";
		model << prop_x << "\n";
		model << prop_y << "\n";
		model << prop_z << "\n";
		model << face << "\n";
		model << vert_indices << "\n";
		model << end_header << "\n";

		for (Triangle triangle : triangles)
		{
			for (short i = 0; i < 3; ++i)
			{
				model << triangle.v[i].x << " ";
				model << triangle.v[i].y << " ";
				model << triangle.v[i].z << "\n";
			}
		}

		unsigned int v = 3;
		unsigned int index = 0;
		for (Triangle triangle : triangles)
		{
			model << v << " ";
			model << index << " ";
			index++;

			model << index << " ";
			index++;

			model << index << "\n";
			index++;
		}
		model.close();
		return true;
	}
	catch (exception& ex)
	{
		cout << ex.what() << "\n";
		return false;
	}
}