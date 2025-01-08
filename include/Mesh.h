#pragma once

#include <list>
#include <vector>

#include "IndexedTriangle.h"
#include "Triangle.h"
#include "Edge.h"

using namespace std;

namespace Service {

	namespace Modeling {

		class Mesh
		{
		public:
			Mesh(list<Triangle>&);
			Mesh(string);
			Mesh();

			void RepairModel();

			void LoadModelFromBinarySTL(const unsigned int buffer_width = 65536);
			void LoadModelFromSTL();
			void Clear();
			void RecalculateQuality();
			list <Triangle>& GetTriangleList();
			vector<Indexed_Triangle>& GetTrianglesWithIndexedVertices();
			vector<Vertex>& GetUniqueVertices();
			vector<vector<unsigned int>>& GetVertexListToTriangleIndices();
			vector<vector<unsigned int>>& GetVertexListToVertexIndices();
			float GetMeshQuality();

		private:
			template<typename T> void eliminate_duplicates(vector<T>&);
			bool merge_vertex_pair(const size_t keeper, const size_t goner);
			void setData();
			void regenerate_triangle_normals();
			void fixProblemEdges();
			void removeBadTriangles();
			void removeIntersectingTriangle();
			void calculateQuality();
			void rebuildMeshData();
			string file_path;
			list<Triangle> triangles;
			vector<Indexed_Triangle> tr;
			vector<Vertex> vertices;
			vector<vector<unsigned int>> vertex_to_triangle_indices;
			vector<vector<unsigned int>> vertex_to_vertex_indices;
			float quality;
		};
	}
}
