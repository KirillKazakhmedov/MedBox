#pragma once

namespace Service
{
	namespace Modeling
	{
		namespace Geometry
		{
			class OrderedPair
			{
			public:
				unsigned int indices[2];

				OrderedPair(const unsigned int &a, const unsigned int &b)
				{
					if (a < b)
					{
						indices[0] = a;
						indices[1] = b;
					}
					else
					{
						indices[0] = b;
						indices[1] = a;
					}
				}

				bool operator==(const OrderedPair &right) const
				{
					if (indices[0] == right.indices[0] && indices[1] == right.indices[1])
						return true;

					else
						return false;
				}

				bool operator<(const OrderedPair &right) const
				{
					if (indices[0] < right.indices[0])
						return true;
					else if (indices[0] > right.indices[0])
						return false;

					if (indices[1] < right.indices[1])
						return true;
					else if (indices[1] > right.indices[1])
						return false;

					return false;
				}
			};
		}
	}
}
