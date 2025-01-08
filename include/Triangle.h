#pragma once

#include <list>
#include <algorithm>

#include "Vertex.h"
#include "Plane.h"
#include "LineSegment.h"

using namespace Service::Modeling;

namespace Service 
{
	namespace Modeling
	{
		namespace Geometry
		{
			class Triangle {
			public:
				Vertex v[3];
				Vector normal;
				float quality;

				Triangle() { }
				Triangle(Vertex v[])
				{
					this->v[0] = v[0];
					this->v[1] = v[1];
					this->v[2] = v[2];

					set_normal();
					set_quality();
				}

				Triangle(Vertex &v1, Vertex &v2, Vertex &v3)
				{
					this->v[0] = v1;
					this->v[1] = v2;
					this->v[2] = v3;

					set_normal();
					set_quality();
				}

				inline void Triangle::RecalculateTriangleQuality()
				{
					set_quality();
				}

				inline void Triangle::RecalculateTriangleNormal()
				{
					set_normal();
				}

				inline bool Triangle::IsIntersection(const Triangle &triangle) const
				{
					Plane plane_1(this->v[0], this->normal);
					Plane plane_2(triangle.v[0], triangle.normal);

					//d0, d1, d2 are distances from first triangle to second triangle plane
					float d0, d1, d2;

					//Determine IsLies vertices of first trinagle on the second triangle plane
					if (!plane_2.IsLies(this->v[0]) && !plane_2.IsLies(this->v[1]) && !plane_2.IsLies(this->v[2]))
					{
						d0 = plane_2.GetDistance(this->v[0]);
						d1 = plane_2.GetDistance(this->v[1]);
						d2 = plane_2.GetDistance(this->v[2]);

						//If every vertices heve same sign, then first triangle lies on one side of second triangle plane
						//end operation
						if ((d0 > 0 && d1 > 0 && d2 > 0) || (d0 < 0 && d1 < 0 && d2 < 0))
							return false;
					}

					//This operations is same for second triangle
					if (!plane_1.IsLies(triangle.v[0]) && !plane_1.IsLies(triangle.v[1]) && !plane_1.IsLies(triangle.v[2]))
					{
						d0 = plane_1.GetDistance(triangle.v[0]);
						d1 = plane_1.GetDistance(triangle.v[1]);
						d2 = plane_1.GetDistance(triangle.v[2]);

						if ((d0 > 0 && d1 > 0 && d2 > 0) || (d0 < 0 && d1 < 0 && d2 < 0))
							return false;
					}

					//Check co-planar triangles
					if (plane_1.A == plane_2.A && 
						plane_1.B == plane_2.B && 
						plane_1.C == plane_2.C && 
						plane_1.D == plane_2.D)
					{
						//Select the first plane
						Vector n(plane_1.A, plane_1.B, plane_1.C);

						Vector xy_n(0.0f, 0.0f, 1.0f);
						Vector xz_n(0.0f, 1.0f, 0.0f);
						Vector yz_n(1.0f, 0.0f, 0.0f);

						//Determine cos value between plane normal and basic planes
						float length = sqrt(n.Nx * n.Nx + n.Ny * n.Ny + n.Nz * n.Nz);
						float cos_xy = abs(n.Nz * xy_n.Nz);
						float cos_xz = abs(n.Ny * xz_n.Ny);
						float cos_yz = abs(n.Nx * yz_n.Nx);

						//Find max cos value
						float max_cos = std::max({ cos_xy, cos_xz, cos_yz });

						//For the first trinagle edges
						LineSegment sides1[3] = { LineSegment(this->v[0], this->v[1]),
												  LineSegment(this->v[0], this->v[2]), 
												  LineSegment(this->v[1], this->v[2]) };

						//For the second trinagle edges
						LineSegment sides2[3] = { LineSegment(triangle.v[0], triangle.v[1]),
												  LineSegment(triangle.v[0], triangle.v[2]),
												  LineSegment(triangle.v[1], triangle.v[2]) };

						bool projected = false;

						//Test on the XY plane lines intersection 
						if(max_cos == cos_xy) 
						{
							for (short i = 0; i < 3; ++i)
							{
								sides1[i].ProjectToXY();
								sides2[i].ProjectToXY();
							}
							projected = true;
						}

						//Test on the XZ plane lines intersection 
						if(max_cos == cos_xz && !projected) 
						{ 
							for (short i = 0; i < 3; ++i)
							{
								sides1[i].ProjectToXZ();
								sides2[i].ProjectToXZ();
							}
							projected = true;
						}

						//Test on the YZ plane lines intersection 
						if(max_cos == cos_yz && !projected) 
						{
							for (short i = 0; i < 3; ++i)
							{
								sides1[i].ProjectToYZ();
								sides2[i].ProjectToYZ();
							}
							projected = true;
						}

						if (projected)
						{
							for (short i = 0; i < 3; ++i)
							{
								for (short j = 0; j < 3; ++j)
								{
									if (sides1[i].IsIntersection(sides2[j]))
										return true;
								}
							}
						}
					}

					return false;
				}

				inline Triangle& operator=(const Triangle &right)
				{
					this->v[0] = right.v[0];
					this->v[1] = right.v[1];
					this->v[2] = right.v[2];

					this->normal = right.normal;
					this->quality = right.quality;

					return *this;
				}

			private:
				inline void Triangle::set_normal()
				{
					Vector normal(this->v);
					this->normal = normal;
				}

				inline void set_quality()
				{
					Vertex edge1, edge2, edge3;
					edge1 = v[1] - v[0];
					edge2 = v[2] - v[0];
					edge3 = v[2] - v[1];

					float a = edge1.length();
					float b = edge2.length();
					float c = edge3.length();
					float p = static_cast<float>((a + b + c) / 2);
					float area = (float)sqrt(p * (p - a) * (p - b) * (p - c));

					quality = (float)(4 * sqrt(3) * area / static_cast<float>(a * a + b * b + c * c));

					if (isnan(quality))
						quality = 0;
				}
			};
		}
	}
}
