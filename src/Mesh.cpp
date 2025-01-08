#include <set>
#include <map>
#include <functional>
#include <iostream>
#include <fstream>
#include <string>
#include <boost\algorithm\string.hpp>

#include "Mesh.h"
#include "IndexedVertex.h"
#include "OrderedPair.h"
#include "Recorder.h"

using namespace Service::Modeling;

Mesh::Mesh() { }

Mesh::Mesh(string model)
{
	this->file_path = model;
}

Mesh::Mesh(list<Triangle> &triangles)
{
	this->triangles = triangles;
	calculateQuality();
	setData();
}

void Mesh::LoadModelFromBinarySTL(const unsigned int buffer_width)
{
	Clear();
	triangles.clear();

	cout << "Reading file: " << file_path.c_str() << endl;

	ifstream in(file_path.c_str(), ios_base::binary);

	if (in.fail())
		return;

	const short header_size = 80;
	vector<char> buffer(header_size, 0);
	unsigned int num_triangles = 0; // Must be 4-byte unsigned int.

									// Read header.
	in.read(reinterpret_cast<char *>(&(buffer[0])), header_size);

	if (header_size != in.gcount())
		return;

	if ('s' == tolower(buffer[0]) &&
		'o' == tolower(buffer[1]) &&
		'l' == tolower(buffer[2]) &&
		'i' == tolower(buffer[3]) &&
		'd' == tolower(buffer[4]))
	{
		cout << "Encountered ASCII STL file header -- aborting." << endl;
		return;
	}

	// Read number of triangles.
	in.read(reinterpret_cast<char *>(&num_triangles), sizeof(unsigned int));

	if (sizeof(unsigned int) != in.gcount())
		return;

	tr.resize(num_triangles);

	cout << "Triangles: " << tr.size() << endl;

	// Enough bytes for twelve 4-byte floats plus one 2-byte integer, per triangle.
	const size_t per_triangle_data_size = (12 * sizeof(float) + sizeof(unsigned short));
	const size_t buffer_size = per_triangle_data_size * buffer_width;
	buffer.resize(buffer_size, 0);

	size_t num_triangles_remaining = tr.size();
	unsigned int tri_index = 0;
	set<Indexed_Vertex> vertex_set;

	while (num_triangles_remaining > 0)
	{
		size_t num_triangles_to_read = buffer_width;

		if (num_triangles_remaining < buffer_width)
			num_triangles_to_read = num_triangles_remaining;

		size_t data_size = per_triangle_data_size*num_triangles_to_read;

		in.read(reinterpret_cast<char *>(&buffer[0]), data_size);

		if (data_size != in.gcount() || in.fail())
			return;

		num_triangles_remaining -= num_triangles_to_read;

		// Use a pointer to assist with the copying.
		// Should probably use std::copy() instead, but memcpy() does the trick, so whatever...
		char *cp = &buffer[0];

		for (unsigned int i = 0; i < num_triangles_to_read; i++)
		{
			// Skip face normal. We will calculate them manually later.
			cp += 3 * sizeof(float);

			Vertex v[3];
			// For each of the three vertices in the triangle.
			for (short unsigned int j = 0; j < 3; j++)
			{
				// Get vertex components.
				memcpy(&v[j].x, cp, sizeof(float)); cp += sizeof(float);
				memcpy(&v[j].y, cp, sizeof(float)); cp += sizeof(float);
				memcpy(&v[j].z, cp, sizeof(float)); cp += sizeof(float);
			}

			Triangle triangle(v);
			triangle.normal.Normalize();
			triangles.push_back(triangle);

			// Skip attribute.
			cp += sizeof(unsigned short);

			tri_index++;
		}
	}
	in.close();

	calculateQuality();
	setData();
}

void Mesh::LoadModelFromSTL()
{
	Clear();
	triangles.clear();

	cout << "Reading file: " << file_path.c_str() << endl;

	ifstream in(file_path.c_str());

	if (in.fail())
		return;

	string buffer;
	float vertex_coords[3][3];

	while (getline(in, buffer))
	{
		if (buffer.find("outer loop") != string::npos)
		{
			for (short i = 0; i < 3; ++i)
			{
				getline(in, buffer);

				string str = buffer;
				string text = "vertex ";
				size_t pos = buffer.find(text);
				str.erase(0, pos + text.length());

				vector<string> numbers;
				boost::split(numbers, str, boost::is_any_of(" "));

				for (short j = 0; j < 3; ++j)
				{
					vertex_coords[i][j] = strtof(numbers[j].c_str(), 0);
				}
			}

			Vertex v1(vertex_coords[0]);
			Vertex v2(vertex_coords[1]);
			Vertex v3(vertex_coords[2]);

			Triangle triangle(v1, v2, v3);
			triangle.normal.Normalize();
			triangles.push_back(triangle);
		}
	}
	in.close();

	calculateQuality();
	setData();
}

void Mesh::setData()
{
	set<Indexed_Vertex> vertex_set;
	unsigned int tri_index = 0;

	Clear();
	tr.resize(triangles.size());

	//Detect unique vertices
	for (Triangle triangle : triangles)
	{
		for (unsigned short i = 0; i < 3; ++i)
		{
			Indexed_Vertex v;

			v.x = triangle.v[i].x;
			v.y = triangle.v[i].y;
			v.z = triangle.v[i].z;

			set<Indexed_Vertex>::const_iterator it = vertex_set.find(v);

			if (vertex_set.end() == it)
			{
				v.index = vertices.size();
				vertex_set.insert(v);

				Vertex vertex;
				vertex.x = v.x;
				vertex.y = v.y;
				vertex.z = v.z;
				vertices.push_back(vertex);

				tr[tri_index].vertex_indices[i] = v.index;

				vector<unsigned int> tri_indices;
				tri_indices.push_back(tri_index);
				vertex_to_triangle_indices.push_back(tri_indices);
			}

			else
			{
				tr[tri_index].vertex_indices[i] = it->index;

				vertex_to_triangle_indices[it->index].push_back(tri_index);
			}
		}
		tri_index++;
	}
	vertex_set.clear();

	//Detect vertex_to_triangle_indices and vertex_to_vertex_indices
	vertex_to_vertex_indices.resize(vertices.size());

	for (unsigned int i = 0; i < vertex_to_triangle_indices.size(); ++i)
	{
		set<unsigned int> vertex_to_vertex_indices_set;

		for (unsigned int j = 0; j < vertex_to_triangle_indices[i].size(); ++j)
		{
			tri_index = vertex_to_triangle_indices[i][j];

			for (short k = 0; k < 3; ++k)
			{
				if (i != tr[tri_index].vertex_indices[k])
				{
					vertex_to_vertex_indices_set.insert(tr[tri_index].vertex_indices[k]);
				}
			}
		}
		for (set<unsigned int>::const_iterator iter = vertex_to_vertex_indices_set.begin(); iter != vertex_to_vertex_indices_set.end(); ++iter)
			vertex_to_vertex_indices[i].push_back(*iter);
	}
}

void Mesh::regenerate_triangle_normals()
{
	unsigned int tri_index = 0;
	for (list<Triangle>::iterator triangle = triangles.begin(); triangle != triangles.end(); ++triangle)
	{
		triangle->v[0] = vertices[tr[tri_index].vertex_indices[0]];
		triangle->v[1] = vertices[tr[tri_index].vertex_indices[1]];
		triangle->v[2] = vertices[tr[tri_index].vertex_indices[2]];

		triangle->RecalculateTriangleNormal();
		triangle->RecalculateTriangleQuality();

		tri_index++;
	}
}

void Mesh::removeBadTriangles()
{
	//Remove triangles as line
	unsigned int deleted = 0;
	for (list<Triangle>::iterator triangle = triangles.begin(); triangle != triangles.end(); )
	{
		if ((triangle->v[0] == triangle->v[1]) || (triangle->v[0] == triangle->v[2]) || (triangle->v[1] == triangle->v[2]))
		{
			triangle = triangles.erase(triangle);
			deleted++;
		}

		else
			++triangle;
	}
	cout << "Crack triangles have been deleted as line: " << deleted << "\n";
	//----------------------------------------------------------------------

	if (deleted != 0)
		rebuildMeshData();

	//Remove excess triangle from triangle fan
	set<unsigned int, std::greater<int>> removable_triangle_indices;
	vector<unsigned int> vertex_indices;

	for (size_t i = 0; i < vertices.size(); ++i)
	{
		//Insert vertices----->triangles
		multimap<unsigned int, unsigned int> vertex_to_triangles;
		for (size_t j = 0; j < vertex_to_vertex_indices[i].size(); ++j)
		{
			unsigned int vert_index = vertex_to_vertex_indices[i][j];

			for (size_t k = 0; k < vertex_to_triangle_indices[i].size(); ++k)
			{
				unsigned int tri_index = vertex_to_triangle_indices[i][k];

				if (tr[tri_index].Exist(vert_index))
					vertex_to_triangles.insert(pair<unsigned int, unsigned int>(vert_index, tri_index));
			}
		}

		if (vertex_to_triangles.size() != 0)
		{
			//Find all vertices belong a once triangle
			map<unsigned int, unsigned int> vertex_to_triangle;
			for (multimap<unsigned int, unsigned int>::iterator iter = vertex_to_triangles.begin(); iter != vertex_to_triangles.end(); ++iter)
			{
				if (vertex_to_triangles.count(iter->first) == 1)
					vertex_to_triangle.insert(pair<unsigned int, unsigned int>(iter->first, iter->second));
			}

			//Find removable triangles
			if (vertex_to_triangle.size() > 1)
			{
				set<unsigned int> triangle_buffer;
				for (map<unsigned int, unsigned int>::iterator vertex = vertex_to_triangle.begin(); vertex != vertex_to_triangle.end(); ++vertex)
				{
					set<unsigned int>::iterator triangle = triangle_buffer.find(vertex->second);
					if (triangle != triangle_buffer.end())
					{
						removable_triangle_indices.insert(vertex->second);
						vertex_indices.push_back(i);
					}

					else
						triangle_buffer.insert(vertex->second);
				}
			}
		}
	}

	if (removable_triangle_indices.size() != 0)
	{
		std::vector<Triangle> t{ std::make_move_iterator(std::begin(triangles)),
								 std::make_move_iterator(std::end(triangles)) };
		/*list<Triangle> trs;
		list<Triangle> trs1;

		for (int i = 0; i < vertex_indices.size(); ++i)
		{
			for (int j = 0; j < vertex_to_triangle_indices[vertex_indices[i]].size(); ++j)
			{
				trs.push_back(t[vertex_to_triangle_indices[vertex_indices[i]][j]]);
			}
		}

		for (set<unsigned int>::iterator it = removable_triangle_indices.begin(); it != removable_triangle_indices.end(); ++it)
			trs1.push_back(t[*it]);

		string fileName = "D:/Study/Kursach/Project/Models/crack.stl";
		Service::Saving::Recodrer::WriteModelToBinarySTL(trs, fileName);

		string fileName1 = "D:/Study/Kursach/Project/Models/crack1.stl";
		Service::Saving::Recodrer::WriteModelToBinarySTL(trs1, fileName1);*/

		//Remove founded triangles
		for (set<unsigned int>::iterator it = removable_triangle_indices.begin(); it != removable_triangle_indices.end(); ++it)
			t.erase(t.begin() + (*it));

		cout << "Excess triangles was deleted: " << removable_triangle_indices.size() << " from triangle fans" << "\n";
		//---------------------------------------------------------------------------------------------------------

		rebuildMeshData();
	}
}

void Mesh::fixProblemEdges()
{
	cout << "Finding cracks" << endl;

	// Find edges that belong to only one triangle.
	set<Edge> problem_edges_set;
	size_t problem_edge_id = 0;

	// For each vertex.
	for (size_t i = 0; i < vertices.size(); i++)
	{
		// For each edge.
		for (size_t j = 0; j < vertex_to_vertex_indices[i].size(); j++)
		{
			size_t triangle_count = 0;
			size_t neighbour_j = vertex_to_vertex_indices[i][j];

			// Find out which two triangles are shared by this edge.
			for (size_t k = 0; k < vertex_to_triangle_indices[i].size(); k++)
			{
				for (size_t l = 0; l < vertex_to_triangle_indices[neighbour_j].size(); l++)
				{
					unsigned int tri0_index = vertex_to_triangle_indices[i][k];
					unsigned int tri1_index = vertex_to_triangle_indices[neighbour_j][l];

					if (tri0_index == tri1_index)
					{
						triangle_count++;
						break;
					}
				}
			} // End of: Find out which two triangles are shared by this edge.

			  // Found a problem edge.
			if (triangle_count != 2)
			{
				Indexed_Vertex v0;
				v0.index = i;
				v0.x = vertices[i].x;
				v0.y = vertices[i].y;
				v0.z = vertices[i].z;

				Indexed_Vertex v1;
				v1.index = neighbour_j;
				v1.x = vertices[neighbour_j].x;
				v1.y = vertices[neighbour_j].y;
				v1.z = vertices[neighbour_j].z;

				Edge problem_edge(v0, v1);

				if (problem_edges_set.end() == problem_edges_set.find(problem_edge))
				{
					problem_edge.id = problem_edge_id++;
					problem_edges_set.insert(problem_edge);
				}
			} // End of: Found a problem edge.
		} // End of: For each edge.
	} // End of: For each vertex.

	if (0 == problem_edges_set.size())
	{
		cout << "No cracks found -- the mesh seems to be in good condition" << endl;
		return;
	}

	cout << "Found " << problem_edges_set.size() << " problem edges" << endl;

	if (0 != problem_edges_set.size() % 2)
	{
		cout << "Error -- the number of problem edges must be an even number (perhaps the mesh has holes?). Aborting." << endl;
		return;
	}

	// Make a copy of the set into a vector because the edge matching will
	// run a bit faster while looping through a vector by index vs looping through
	// a set by iterator.
	vector<Edge> problem_edges_vec(problem_edges_set.begin(), problem_edges_set.end());
	vector<bool> processed_problem_edges(problem_edges_set.size(), false);
	problem_edges_set.clear();

	set<OrderedPair> merge_vertices;

	cout << "Pairing problem edges" << endl;

	// Each problem edge is practically a duplicate of some other, but not quite exactly.
	// So, find the closest match for each problem edge.
	for (size_t i = 0; i < problem_edges_vec.size(); i++)
	{
		// This edge has already been matched up previously, so skip it.
		if (true == processed_problem_edges[problem_edges_vec[i].id])
			continue;

		float closest_dist_sq = numeric_limits<float>::max();
		size_t closest_problem_edges_vec_index = 0;

		for (size_t j = i + 1; j < problem_edges_vec.size(); j++)
		{
			// Note: Don't check to see if this edge has been processed yet.
			// Doing so will actually only slow this down further.
			// Perhaps vector<bool> is a bit of a sloth?
			//if(true == processed_problem_edges[problem_edges_vec[j].id])
			//	continue;

			float dist_sq = problem_edges_vec[i].center.distance_sq(problem_edges_vec[j].center);

			if (dist_sq < closest_dist_sq)
			{
				closest_dist_sq = dist_sq;
				closest_problem_edges_vec_index = j;
			}
		}

		processed_problem_edges[problem_edges_vec[i].id] = true;
		processed_problem_edges[problem_edges_vec[closest_problem_edges_vec_index].id] = true;

		// If edge 0 vertex 0 is further in space from edge 1 vertex 0 than from edge 1 vertex 1,
		// then swap the indices on the edge 1 -- this makes sure that the edges are not pointing
		// in opposing directions.
		if (vertices[problem_edges_vec[i].vertex_indices[0]].distance_sq(vertices[problem_edges_vec[closest_problem_edges_vec_index].vertex_indices[0]]) > 
			vertices[problem_edges_vec[i].vertex_indices[0]].distance_sq(vertices[problem_edges_vec[closest_problem_edges_vec_index].vertex_indices[1]]))
		{
			size_t temp = problem_edges_vec[closest_problem_edges_vec_index].vertex_indices[0];
			problem_edges_vec[closest_problem_edges_vec_index].vertex_indices[0] = problem_edges_vec[closest_problem_edges_vec_index].vertex_indices[1];
			problem_edges_vec[closest_problem_edges_vec_index].vertex_indices[1] = temp;
		}

		// If the first indices aren't already the same, then merge them.
		if (problem_edges_vec[i].vertex_indices[0] != problem_edges_vec[closest_problem_edges_vec_index].vertex_indices[0])
			merge_vertices.insert(OrderedPair(problem_edges_vec[i].vertex_indices[0], problem_edges_vec[closest_problem_edges_vec_index].vertex_indices[0]));

		// If the second indices aren't already the same, then merge them.
		if (problem_edges_vec[i].vertex_indices[1] != problem_edges_vec[closest_problem_edges_vec_index].vertex_indices[1])
			merge_vertices.insert(OrderedPair(problem_edges_vec[i].vertex_indices[1], problem_edges_vec[closest_problem_edges_vec_index].vertex_indices[1]));
	}

	cout << "Merging " << merge_vertices.size() << " vertex pairs" << endl;

	for (set<OrderedPair>::const_iterator ci = merge_vertices.begin(); ci != merge_vertices.end(); ci++)
		merge_vertex_pair(ci->indices[0], ci->indices[1]);

	// Recalculate normals, if necessary.
	regenerate_triangle_normals();
}

template<typename T> void Mesh::eliminate_duplicates(vector<T> &v)
{
	if (0 == v.size())
		return;

	set<T> s(v.begin(), v.end()); // Eliminate duplicates (and sort them)
	vector<T> vtemp(s.begin(), s.end()); // Stuff things back into a temp vector
	v.swap(vtemp); // Assign temp vector contents to destination vector
}

bool Mesh::merge_vertex_pair(const size_t keeper, const size_t goner)
{
	if (keeper >= vertices.size() || goner >= vertices.size())
		return false;

	if (keeper == goner)
		return true;

	// Merge vertex to triangle data.

	// Add goner's vertex to triangle data to keeper's triangle to vertex data,
	// and replace goner's index with keeper's index in all relevant triangles' index data.
	for (size_t i = 0; i < vertex_to_triangle_indices[goner].size(); i++)
	{
		size_t tri_index = vertex_to_triangle_indices[goner][i];

		vertex_to_triangle_indices[keeper].push_back(tri_index);

		for (size_t j = 0; j < 3; j++)
			if (goner == tr[tri_index].vertex_indices[j])
				tr[tri_index].vertex_indices[j] = keeper;
	}

	// Finalize keeper's vertex to triangle data.
	eliminate_duplicates(vertex_to_triangle_indices[keeper]);

	// Clear out goner's vertex to triangle data for good.
	vertex_to_triangle_indices[goner].clear();


	// Merge vertex to vertex data.

	// Add goner's vertex to vertex data to keeper's vertex to vertex data,
	// and replace goner's index with keeper's index in all relevant vertices' vertex to vertex data.
	for (size_t i = 0; i < vertex_to_vertex_indices[goner].size(); i++)
	{
		size_t vert_index = vertex_to_vertex_indices[goner][i];

		vertex_to_vertex_indices[keeper].push_back(vert_index);

		for (size_t j = 0; j < vertex_to_vertex_indices[vert_index].size(); j++)
		{
			// Could probably break after this, but whatever...
			if (goner == vertex_to_vertex_indices[vert_index][j])
				vertex_to_vertex_indices[vert_index][j] = keeper;
		}

		eliminate_duplicates(vertex_to_vertex_indices[vert_index]);
	}

	// Finalize keeper's vertex to vertex data.
	eliminate_duplicates(vertex_to_vertex_indices[keeper]);

	// Clear out goner's vertex to vertex data for good.
	vertex_to_vertex_indices[goner].clear();

	// Note: At this point, vertices[goner] is now a rogue vertex.
	// We will skip erasing it from the vertices vector because that would mean a whole lot more work
	// (we'd have to reindex every vertex after it in the vector, etc.).
	// 
	// If the mesh is saved to STL, then the rogue vertex will automatically be skipped, and life is good.
	//
	// If the mesh is saved to POV-Ray mesh2, then the rogue vertex will be included in the vertex
	// list, but it will simply not be referenced in the triangle list -- this is a bit inoptimal
	// in terms of the file size (it will add a few dozen unneeded bytes to the file size).

	return true;
}

void Mesh::removeIntersectingTriangle()
{
	//The collection will contain intersecting trinagles
	set<unsigned int, std::greater<unsigned int>> removable_triangle_indices;

	//Find by center vertex
	for (unsigned int i = 0; i < vertex_to_triangle_indices.size(); ++i)
	{
		for (unsigned int j = 0; j < vertex_to_triangle_indices[i].size(); ++j)
		{
			unsigned int index_first = vertex_to_triangle_indices[i][j];

			Vertex v[3];
			v[0] = vertices[tr[index_first].vertex_indices[0]];
			v[1] = vertices[tr[index_first].vertex_indices[1]];
			v[2] = vertices[tr[index_first].vertex_indices[2]];

			Triangle tri_first(v);
			tri_first.normal.Normalize();

			for (unsigned int k = j + 1; k < vertex_to_triangle_indices[i].size(); ++k)
			{
				unsigned int index_second = vertex_to_triangle_indices[i][k];

				Vertex v_[3];
				v_[0] = vertices[tr[vertex_to_triangle_indices[i][k]].vertex_indices[0]];
				v_[1] = vertices[tr[vertex_to_triangle_indices[i][k]].vertex_indices[1]];
				v_[2] = vertices[tr[vertex_to_triangle_indices[i][k]].vertex_indices[2]];

				Triangle tri_second(v_);
				tri_second.normal.Normalize();

				if (tri_first.IsIntersection(tri_second))
				{
					if (std::find(removable_triangle_indices.begin(), removable_triangle_indices.end(), index_first) == removable_triangle_indices.end())
						removable_triangle_indices.insert(index_first);

					if (std::find(removable_triangle_indices.begin(), removable_triangle_indices.end(), index_second) == removable_triangle_indices.end())
						removable_triangle_indices.insert(index_second);
				}
			}
		}
	}

	//Remove intersecting triangles
	if (removable_triangle_indices.size() != 0)
	{
		//-----------------------------------------------------------------------------------------------------------------
		std::vector<Triangle> t{ std::make_move_iterator(std::begin(triangles)),
								 std::make_move_iterator(std::end(triangles)) };
		//list<Triangle> trs;

		//for (set<unsigned int>::iterator it = removable_triangle_indices.begin(); it != removable_triangle_indices.end(); ++it)
		//	trs.push_back(t[*it]);

		//string fileName = "D:/Study/Kursach/Project/Models/crack.stl";
		//Service::Saving::Recodrer::WriteModelToBinarySTL(trs, fileName);
		//--------------------------------------------------------------------------------------------------------------------

		for (set<unsigned int>::iterator it = removable_triangle_indices.begin(); it != removable_triangle_indices.end(); ++it)
			t.erase(t.begin() + (*it));

		cout << "Intersecting triangles was deleted: " << removable_triangle_indices.size() << "\n";

		rebuildMeshData();
	}
}

//TODO
void Mesh::RepairModel()
{
	fixProblemEdges();
	removeBadTriangles();
	removeIntersectingTriangle();

	RecalculateQuality();
}

void Mesh::RecalculateQuality()
{
	calculateQuality();
}

void Mesh::calculateQuality()
{
	float sum = 0.0f;
	for (list<Triangle>::iterator triangle = triangles.begin(); triangle != triangles.end(); ++triangle)
		sum += triangle->quality;

	this->quality = sum / static_cast<float>(triangles.size());
}

void Mesh::rebuildMeshData()
{
	setData();
}

list <Triangle>& Mesh::GetTriangleList()
{
	return triangles;
}

vector<Indexed_Triangle>& Mesh::GetTrianglesWithIndexedVertices()
{
	return tr;
}

vector<Vertex>& Mesh::GetUniqueVertices()
{
	return vertices;
}

vector<vector<unsigned int>>& Mesh::GetVertexListToTriangleIndices()
{
	return vertex_to_triangle_indices;
}

vector<vector<unsigned int>>& Mesh::GetVertexListToVertexIndices()
{
	return vertex_to_vertex_indices;
}

float Mesh::GetMeshQuality()
{
	return quality;
}

void Mesh::Clear()
{
	vertices.clear();
	vertex_to_triangle_indices.clear();
	vertex_to_vertex_indices.clear();
	tr.clear();
}