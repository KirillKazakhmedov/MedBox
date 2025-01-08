#include "Builder.h"

using namespace Service::Modeling;

Builder::Builder(Service::ImageCollection* collection, short iso_surface, bool standartMC, const short size)
{
	this->iso_surface = iso_surface;
	this->segmentation_mark = collection->SegmentationMark;
	this->standartMC = standartMC;
	this->images = collection->GetImages();
	this->dx = collection->XLength;
	this->dy = collection->YLength;
	this->dz = collection->ZLength;
	this->cell_size = size;
}

Builder::Builder(Service::ImageCollection* collection, bool standartMC, const short size)
{
	this->segmentation_mark = collection->SegmentationMark;
	this->standartMC = standartMC;
	this->images = collection->GetImages();
	this->dx = collection->XLength;
	this->dy = collection->YLength;
	this->dz = collection->ZLength;
	this->cell_size = size;
}

void Builder::getVertices(short edge, short *arr) {
	switch (edge) {
	case 0:
		arr[0] = 0;
		arr[1] = 1;
		break;
	case 1:
		arr[0] = 1;
		arr[1] = 2;
		break;
	case 2:
		arr[0] = 2;
		arr[1] = 3;
		break;
	case 3:
		arr[0] = 3;
		arr[1] = 0;
		break;
	case 4:
		arr[0] = 4;
		arr[1] = 5;
		break;
	case 5:
		arr[0] = 5;
		arr[1] = 6;
		break;
	case 6:
		arr[0] = 6;
		arr[1] = 7;
		break;
	case 7:
		arr[0] = 7;
		arr[1] = 4;
		break;
	case 8:
		arr[0] = 0;
		arr[1] = 4;
		break;
	case 9:
		arr[0] = 1;
		arr[1] = 5;
		break;
	case 10:
		arr[0] = 2;
		arr[1] = 6;
		break;
	case 11:
		arr[0] = 3;
		arr[1] = 7;
		break;
	}
}

bool Builder::Build(short i, short j, short k) {
	Image current = images[k];
	Image next = images[k + 1];

	cell.vertex[0].x = i * dx;
	cell.vertex[0].y = j * dy;
	cell.vertex[0].z = k * dz;

	cell.vertex[1].x = (i + cell_size) * dx;
	cell.vertex[1].y = j * dy;
	cell.vertex[1].z = k * dz;

	cell.vertex[2].x = (i + cell_size) * dx;
	cell.vertex[2].y = j * dy;
	cell.vertex[2].z = (k + 1) * dz;

	cell.vertex[3].x = i * dx;
	cell.vertex[3].y = j * dy;
	cell.vertex[3].z = (k + 1) * dz;

	cell.vertex[4].x = i * dx;
	cell.vertex[4].y = (j + cell_size) * dy;
	cell.vertex[4].z = k * dz;

	cell.vertex[5].x = (i + cell_size) * dx;
	cell.vertex[5].y = (j + cell_size) * dy;
	cell.vertex[5].z = k * dz;

	cell.vertex[6].x = (i + cell_size) * dx;
	cell.vertex[6].y = (j + cell_size) * dy;
	cell.vertex[6].z = (k + 1) * dz;

	cell.vertex[7].x = i * dx;
	cell.vertex[7].y = (j + cell_size) * dy;
	cell.vertex[7].z = (k + 1) * dz;

	short count = 0;
	if (segmentation_mark != Segmentation::SettedThreshold)
	{
		if (standartMC)
		{
			cell.value[0] = current.Data[j][i];
			cell.value[1] = current.Data[j][i + cell_size];
			cell.value[2] = next.Data[j][i + cell_size];
			cell.value[3] = next.Data[j][i];
			cell.value[4] = current.Data[j + cell_size][i];
			cell.value[5] = current.Data[j + cell_size][i + cell_size];
			cell.value[6] = next.Data[j + cell_size][i + cell_size];
			cell.value[7] = next.Data[j + cell_size][i];

			for (int i = 0; i < 8; ++i)
			{
				cell.nodeParity[i] = cell.value[i] > iso_surface;

				if (cell.nodeParity[i])
					count++;
			}
		}

		else
		{
			cell.value[0] = current.Data[j][i] - iso_surface;
			cell.value[1] = current.Data[j][i + cell_size] - iso_surface;
			cell.value[2] = next.Data[j][i + cell_size] - iso_surface;
			cell.value[3] = next.Data[j][i] - iso_surface;
			cell.value[4] = current.Data[j + cell_size][i] - iso_surface;
			cell.value[5] = current.Data[j + cell_size][i + cell_size] - iso_surface;
			cell.value[6] = next.Data[j + cell_size][i + cell_size] - iso_surface;
			cell.value[7] = next.Data[j + cell_size][i] - iso_surface;

			for (int i = 0; i < 8; ++i)
			{
				cell.nodeParity[i] = cell.value[i] > 0;

				if (cell.nodeParity[i])
					count++;
			}
		}
	}

	else
	{
		cell.value[0] = current.Data[j][i];
		cell.value[1] = current.Data[j][i + cell_size];
		cell.value[2] = next.Data[j][i + cell_size];
		cell.value[3] = next.Data[j][i];
		cell.value[4] = current.Data[j + cell_size][i];
		cell.value[5] = current.Data[j + cell_size][i + cell_size];
		cell.value[6] = next.Data[j + cell_size][i + cell_size];
		cell.value[7] = next.Data[j + cell_size][i];

		for (int i = 0; i < 8; ++i)
		{
			cell.nodeParity[i] = cell.value[i] > 0;

			if (cell.nodeParity[i])
				count++;
		}
	}

	if (count > 0 && count < 8)
		return true;

	else
		return false;
}

Vertex Builder::getIntersection(short edge) 
{
	if (edge == 12)
		return cell.additional_vertex;

	short vertices[2];
	getVertices(edge, vertices);

	Vertex v1 = cell.vertex[vertices[0]];
	Vertex v2 = cell.vertex[vertices[1]];

	short value1 = cell.value[vertices[0]];
	short value2 = cell.value[vertices[1]];

	Vertex v;
	float scale;

	switch (segmentation_mark)
	{
	case Segmentation::Null:
	{
		if (standartMC)
		{
			if (abs(iso_surface - value1) == 0)
				return v1;
			if (abs(iso_surface - value2) == 0)
				return v2;

			scale = (iso_surface - value1) / static_cast<float>(value2 - value1);
		}

		else
		{
			if (abs(value1) == 0)
				return v1;
			if (abs(value2) == 0)
				return v2;

			scale = -value1 / static_cast<float>(value2 - value1);
		}

		if (abs(value1 - value2) == 0)
			return v1;
		break;
	}

	case Segmentation::SettedThreshold:
	{
		scale = 0.5;
		break;
	}

	default:
		break;
	}

	v = v1 + (v2 - v1) * scale;

	return v;
}

short Builder::getNodeCaseNumber()
{
	short powerOf2 = 1;
	short caseID = 0;
	for (int i = 0; i < 8; i++) 
	{
		if (cell.nodeParity[i])
			caseID += powerOf2;

		powerOf2 *= 2;
	}
	return caseID;
}

void Builder::addTriangles(list<Triangle> &triangles, const __int8 edges[], short triangles_count)
{
	short index = 0;
	for (short i = 0; i < triangles_count; ++i)
	{
		Vertex v[3];

		v[0] = getIntersection(edges[index]);
		v[1] = getIntersection(edges[index + 1]);
		v[2] = getIntersection(edges[index + 2]);

		Triangle triangle(v);
		triangle.normal.Normalize();

		triangles.push_back(triangle);
		index += 3;
	}
}

list <Triangle> Builder::getTriangles() 
{
	list <Triangle> triangles(0);
	short index = 0;
	short nodeCase = getNodeCaseNumber();

	if (standartMC)
	{
		while (index < 16 && classicCases[nodeCase][index] != -1)
		{
			Vertex v[3];

			v[0] = getIntersection(classicCases[nodeCase][index]);
			v[1] = getIntersection(classicCases[nodeCase][index + 1]);
			v[2] = getIntersection(classicCases[nodeCase][index + 2]);

			Triangle triangle(v);
			triangle.normal.Normalize();

			triangles.push_back(triangle);
			index += 3;
		}
		return triangles;
	}

	//MC33 begining
	_case = cases[nodeCase][0];
	_config = cases[nodeCase][1];
	_subconfig = 0;

	switch (_case) 
	{
	case  0 :
    break ;

  case  1 :
    addTriangles(triangles, tiling1[_config], 1) ;
    break ;

  case  2 :
    addTriangles(triangles, tiling2[_config], 2) ;
    break ;

  case  3 :
    if(testFace(test3[_config]))
      addTriangles(triangles, tiling3_2[_config], 4 ) ; // 3.2
    else
      addTriangles(triangles, tiling3_1[_config], 2 ) ; // 3.1
    break ;

  case  4 :
    if( testInterior(test4[_config]))
      addTriangles(triangles, tiling4_1[_config], 2 ) ; // 4.1.1
    else
      addTriangles(triangles, tiling4_2[_config], 6 ) ; // 4.1.2
    break ;

  case  5 :
    addTriangles(triangles, tiling5[_config], 3 ) ;
    break ;

  case  6 :
    if( testFace(test6[_config][0]) )
      addTriangles(triangles, tiling6_2[_config], 5 ) ; // 6.2
    else
    {
      if( testInterior(test6[_config][1]) )
        addTriangles(triangles, tiling6_1_1[_config], 3 ) ; // 6.1.1
      else
	  {
        addAdditionalVertex();
        addTriangles(triangles, tiling6_1_2[_config], 9) ; // 6.1.2
      }
    }
    break ;

  case  7 :
    if( testFace(test7[_config][0]) ) _subconfig +=  1 ;
    if( testFace(test7[_config][1]) ) _subconfig +=  2 ;
    if( testFace(test7[_config][2]) ) _subconfig +=  4 ;
    switch( _subconfig )
      {
      case 0 :
        addTriangles(triangles, tiling7_1[_config], 3 ) ; break ;
      case 1 :
        addTriangles(triangles, tiling7_2[_config][0], 5 ) ; break ;
      case 2 :
        addTriangles(triangles, tiling7_2[_config][1], 5 ) ; break ;
      case 3 :
        addAdditionalVertex();
        addTriangles(triangles, tiling7_3[_config][0], 9) ; break ;
      case 4 :
        addTriangles(triangles, tiling7_2[_config][2], 5 ) ; break ;
      case 5 :
        addAdditionalVertex();
        addTriangles(triangles, tiling7_3[_config][1], 9) ; break ;
      case 6 :
        addAdditionalVertex();
        addTriangles(triangles, tiling7_3[_config][2], 9) ; break ;
      case 7 :
        if( testInterior( test7[_config][3]) )
          addTriangles(triangles, tiling7_4_2[_config], 9) ;
        else
          addTriangles(triangles, tiling7_4_1[_config], 5) ;
        break ;
      };
    break ;

  case  8 :
    addTriangles(triangles, tiling8[_config], 2) ;
    break ;

  case  9 :
    addTriangles(triangles, tiling9[_config], 4) ;
    break ;

  case 10 :
    if(testFace(test10[_config][0]))
    {
      if( testFace(test10[_config][1]) )
        addTriangles(triangles, tiling10_1_1_[_config], 4) ; // 10.1.1
      else
      {
        addAdditionalVertex();
        addTriangles(triangles, tiling10_2[_config], 8) ; // 10.2
      }
    }
    else
    {
      if( testFace(test10[_config][1]) )
      {
        addAdditionalVertex();
        addTriangles(triangles, tiling10_2_[_config], 8) ; // 10.2
      }
      else
      {
        if(testInterior(test10[_config][2]))
          addTriangles(triangles, tiling10_1_1[_config], 4 ) ; // 10.1.1
        else
          addTriangles(triangles, tiling10_1_2[_config], 8 ) ; // 10.1.2
      }
    }
    break ;

  case 11 :
    addTriangles(triangles, tiling11[_config], 4 ) ;
    break ;

  case 12 :
    if( testFace(test12[_config][0]) )
    {
      if( testFace(test12[_config][1]) )
        addTriangles(triangles, tiling12_1_1_[_config], 4 ) ; // 12.1.1
      else
      {
        addAdditionalVertex();
        addTriangles(triangles, tiling12_2[_config], 8) ; // 12.2
      }
    }
    else
    {
      if( testFace(test12[_config][1]) )
      {
        addAdditionalVertex();
        addTriangles(triangles, tiling12_2_[_config], 8) ; // 12.2
      }
      else
      {
        if( testInterior( test12[_config][2]))
          addTriangles(triangles, tiling12_1_1[_config], 4 ) ; // 12.1.1
        else
          addTriangles(triangles, tiling12_1_2[_config], 8 ) ; // 12.1.2
      }
    }
    break ;

  case 13 :
    if( testFace(test13[_config][0]) ) _subconfig +=  1 ;
    if( testFace(test13[_config][1]) ) _subconfig +=  2 ;
    if( testFace(test13[_config][2]) ) _subconfig +=  4 ;
    if( testFace(test13[_config][3]) ) _subconfig +=  8 ;
    if( testFace(test13[_config][4]) ) _subconfig += 16 ;
    if( testFace(test13[_config][5]) ) _subconfig += 32 ;
    switch( subconfig13[_subconfig] )
    {
      case 0 :/* 13.1 */
        addTriangles(triangles, tiling13_1[_config], 4 ) ; break ;

      case 1 :/* 13.2 */
        addTriangles(triangles, tiling13_2[_config][0], 6 ) ; break ;
      case 2 :/* 13.2 */
        addTriangles(triangles, tiling13_2[_config][1], 6 ) ; break ;
      case 3 :/* 13.2 */
        addTriangles(triangles, tiling13_2[_config][2], 6 ) ; break ;
      case 4 :/* 13.2 */
        addTriangles(triangles, tiling13_2[_config][3], 6 ) ; break ;
      case 5 :/* 13.2 */
        addTriangles(triangles, tiling13_2[_config][4], 6 ) ; break ;
      case 6 :/* 13.2 */
        addTriangles(triangles, tiling13_2[_config][5], 6 ) ; break ;

      case 7 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3[_config][0], 10) ; break ;
      case 8 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3[_config][1], 10) ; break ;
      case 9 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3[_config][2], 10) ; break ;
      case 10 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3[_config][3], 10) ; break ;
      case 11 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3[_config][4], 10) ; break ;
      case 12 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3[_config][5], 10) ; break ;
      case 13 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3[_config][6], 10) ; break ;
      case 14 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3[_config][7], 10) ; break ;
      case 15 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3[_config][8], 10) ; break ;
      case 16 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3[_config][9], 10) ; break ;
      case 17 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3[_config][10], 10) ; break ;
      case 18 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3[_config][11], 10) ; break ;

      case 19 :/* 13.4 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_4[_config][0], 12) ; break ;
      case 20 :/* 13.4 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_4[_config][1], 12) ; break ;
      case 21 :/* 13.4 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_4[_config][2], 12) ; break ;
      case 22 :/* 13.4 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_4[_config][3], 12) ; break ;

      case 23 :/* 13.5 */
        _subconfig = 0 ;
        if( testInterior( test13[_config][6]) )
          addTriangles(triangles, tiling13_5_1[_config][0], 6 ) ;
        else
          addTriangles(triangles, tiling13_5_2[_config][0], 10 ) ;
        break ;
      case 24 :/* 13.5 */
        _subconfig = 1 ;
        if( testInterior( test13[_config][6]) )
          addTriangles(triangles, tiling13_5_1[_config][1], 6 ) ;
        else
          addTriangles(triangles, tiling13_5_2[_config][1], 10 ) ;
        break ;
      case 25 :/* 13.5 */
        _subconfig = 2 ;
        if( testInterior( test13[_config][6]) )
          addTriangles(triangles, tiling13_5_1[_config][2], 6 ) ;
        else
          addTriangles(triangles, tiling13_5_2[_config][2], 10 ) ;
        break ;
      case 26 :/* 13.5 */
        _subconfig = 3 ;
        if( testInterior( test13[_config][6]) )
          addTriangles(triangles, tiling13_5_1[_config][3], 6 ) ;
        else
          addTriangles(triangles, tiling13_5_2[_config][3], 10 ) ;
        break ;

      case 27 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3_[_config][0], 10) ; break ;
      case 28 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3_[_config][1], 10) ; break ;
      case 29 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3_[_config][2], 10) ; break ;
      case 30 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3_[_config][3], 10) ; break ;
      case 31 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3_[_config][4], 10) ; break ;
      case 32 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3_[_config][5], 10) ; break ;
      case 33 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3_[_config][6], 10) ; break ;
      case 34 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3_[_config][7], 10) ; break ;
      case 35 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3_[_config][8], 10) ; break ;
      case 36 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3_[_config][9], 10) ; break ;
      case 37 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3_[_config][10], 10) ; break ;
      case 38 :/* 13.3 */
        addAdditionalVertex();
        addTriangles(triangles, tiling13_3_[_config][11], 10) ; break ;

      case 39 :/* 13.2 */
        addTriangles(triangles, tiling13_2_[_config][0], 6 ) ; break ;
      case 40 :/* 13.2 */
        addTriangles(triangles, tiling13_2_[_config][1], 6 ) ; break ;
      case 41 :/* 13.2 */
        addTriangles(triangles, tiling13_2_[_config][2], 6 ) ; break ;
      case 42 :/* 13.2 */
        addTriangles(triangles, tiling13_2_[_config][3], 6 ) ; break ;
      case 43 :/* 13.2 */
        addTriangles(triangles, tiling13_2_[_config][4], 6 ) ; break ;
      case 44 :/* 13.2 */
        addTriangles(triangles, tiling13_2_[_config][5], 6 ) ; break ;

      case 45 :/* 13.1 */
        addTriangles(triangles, tiling13_1_[_config], 4 ) ; break ;

      default :
				std::cout << "Marching cell.values: Impossible case 13?\n";
      }
      break ;

  case 14 :
    addTriangles(triangles, tiling14[_config], 4 ) ;
    break ;
	};

	return triangles;
}

bool Builder::testFace(__int8 face)
{
	float A, B, C, D;

	switch (face) {
	case -1:
	case 1:
		A = cell.value[0];
		B = cell.value[4];
		C = cell.value[5];
		D = cell.value[1];
		break;
	case -2:
	case 2:
		A = cell.value[1];
		B = cell.value[5];
		C = cell.value[6];
		D = cell.value[2];
		break;
	case -3:
	case 3:
		A = cell.value[2];
		B = cell.value[6];
		C = cell.value[7];
		D = cell.value[3];
		break;
	case -4:
	case 4:
		A = cell.value[3];
		B = cell.value[7];
		C = cell.value[4];
		D = cell.value[0];
		break;
	case -5:
	case 5:
		A = cell.value[0];
		B = cell.value[3];
		C = cell.value[2];
		D = cell.value[1];
		break;
	case -6:
	case 6:
		A = cell.value[4];
		B = cell.value[7];
		C = cell.value[6];
		D = cell.value[5];
		break;

	default:
		cout << "Invalid face code " << face << "\n";
		A = B = C = D = 0;
		break;
	};

	if (fabs(A * C - B * D) < 0.1)
		return face >= 0;

	return face * A * (A * C - B * D) >= 0; // face and A invert signs
}

bool Builder::testInterior(__int8 s)
{
	short a, b;
	float t;
	float At = 0, Bt = 0, Ct = 0, Dt = 0;
	char  test = 0;
	char  edge = -1; // reference edge of the triangulation

	switch (_case)
	{
	case  4:
	case 10:
		a = (cell.value[4] - cell.value[0]) * (cell.value[6] - cell.value[2]) - (cell.value[7] - cell.value[3]) * (cell.value[5] - cell.value[1]);
		b = cell.value[2] * (cell.value[4] - cell.value[0]) + cell.value[0] * (cell.value[6] - cell.value[2])
			- cell.value[1] * (cell.value[7] - cell.value[3]) - cell.value[3] * (cell.value[5] - cell.value[1]);
		t = -b / static_cast<float>(2 * a);
		if (t<0 || t>1) return s>0;

		At = cell.value[0] + (cell.value[4] - cell.value[0]) * t;
		Bt = cell.value[3] + (cell.value[7] - cell.value[3]) * t;
		Ct = cell.value[2] + (cell.value[6] - cell.value[2]) * t;
		Dt = cell.value[1] + (cell.value[5] - cell.value[1]) * t;
		break;

	case  6:
	case  7:
	case 12:
	case 13:
		switch (_case)
		{
		case  6: edge = test6[_config][2]; break;
		case  7: edge = test7[_config][4]; break;
		case 12: edge = test12[_config][3]; break;
		case 13: edge = tiling13_5_1[_config][_subconfig][0]; break;
		}
		switch (edge)
		{
		case  0:
			t = cell.value[0] / static_cast<float>(cell.value[0] - cell.value[1]);
			At = 0;
			Bt = cell.value[3] + (cell.value[2] - cell.value[3]) * t;
			Ct = cell.value[7] + (cell.value[6] - cell.value[7]) * t;
			Dt = cell.value[4] + (cell.value[5] - cell.value[4]) * t;
			break;
		case  1:
			t = cell.value[1] / static_cast<float>(cell.value[1] - cell.value[2]);
			At = 0;
			Bt = cell.value[0] + (cell.value[3] - cell.value[0]) * t;
			Ct = cell.value[4] + (cell.value[7] - cell.value[4]) * t;
			Dt = cell.value[5] + (cell.value[6] - cell.value[5]) * t;
			break;
		case  2:
			t = cell.value[2] / static_cast<float>(cell.value[2] - cell.value[3]);
			At = 0;
			Bt = cell.value[1] + (cell.value[0] - cell.value[1]) * t;
			Ct = cell.value[5] + (cell.value[4] - cell.value[5]) * t;
			Dt = cell.value[6] + (cell.value[7] - cell.value[6]) * t;
			break;
		case  3:
			t = cell.value[3] / static_cast<float>(cell.value[3] - cell.value[0]);
			At = 0;
			Bt = cell.value[2] + (cell.value[1] - cell.value[2]) * t;
			Ct = cell.value[6] + (cell.value[5] - cell.value[6]) * t;
			Dt = cell.value[7] + (cell.value[4] - cell.value[7]) * t;
			break;
		case  4:
			t = cell.value[4] / static_cast<float>(cell.value[4] - cell.value[5]);
			At = 0;
			Bt = cell.value[7] + (cell.value[6] - cell.value[7]) * t;
			Ct = cell.value[3] + (cell.value[2] - cell.value[3]) * t;
			Dt = cell.value[0] + (cell.value[1] - cell.value[0]) * t;
			break;
		case  5:
			t = cell.value[5] / static_cast<float>(cell.value[5] - cell.value[6]);
			At = 0;
			Bt = cell.value[4] + (cell.value[7] - cell.value[4]) * t;
			Ct = cell.value[0] + (cell.value[3] - cell.value[0]) * t;
			Dt = cell.value[1] + (cell.value[2] - cell.value[1]) * t;
			break;
		case  6:
			t = cell.value[6] / static_cast<float>(cell.value[6] - cell.value[7]);
			At = 0;
			Bt = cell.value[5] + (cell.value[4] - cell.value[5]) * t;
			Ct = cell.value[1] + (cell.value[0] - cell.value[1]) * t;
			Dt = cell.value[2] + (cell.value[3] - cell.value[2]) * t;
			break;
		case  7:
			t = cell.value[7] / static_cast<float>(cell.value[7] - cell.value[4]);
			At = 0;
			Bt = cell.value[6] + (cell.value[5] - cell.value[6]) * t;
			Ct = cell.value[2] + (cell.value[1] - cell.value[2]) * t;
			Dt = cell.value[3] + (cell.value[0] - cell.value[3]) * t;
			break;
		case  8:
			t = cell.value[0] / static_cast<float>(cell.value[0] - cell.value[4]);
			At = 0;
			Bt = cell.value[3] + (cell.value[7] - cell.value[3]) * t;
			Ct = cell.value[2] + (cell.value[6] - cell.value[2]) * t;
			Dt = cell.value[1] + (cell.value[5] - cell.value[1]) * t;
			break;
		case  9:
			t = cell.value[1] / static_cast<float>(cell.value[1] - cell.value[5]);
			At = 0;
			Bt = cell.value[0] + (cell.value[4] - cell.value[0]) * t;
			Ct = cell.value[3] + (cell.value[7] - cell.value[3]) * t;
			Dt = cell.value[2] + (cell.value[6] - cell.value[2]) * t;
			break;
		case 10:
			t = cell.value[2] / static_cast<float>(cell.value[2] - cell.value[6]);
			At = 0;
			Bt = cell.value[1] + (cell.value[5] - cell.value[1]) * t;
			Ct = cell.value[0] + (cell.value[4] - cell.value[0]) * t;
			Dt = cell.value[3] + (cell.value[7] - cell.value[3]) * t;
			break;
		case 11:
			t = cell.value[3] / static_cast<float>(cell.value[3] - cell.value[7]);
			At = 0;
			Bt = cell.value[2] + (cell.value[6] - cell.value[2]) * t;
			Ct = cell.value[1] + (cell.value[5] - cell.value[1]) * t;
			Dt = cell.value[0] + (cell.value[4] - cell.value[0]) * t;
			break;
		default: std::cout << " Invalid edge " << edge << "\n";  break;
		}
		break;

	default: std::cout << " Invalid ambiguous case " << _case << "\n";  break;
	}

	if (At >= 0) test++;
	if (Bt >= 0) test += 2;
	if (Ct >= 0) test += 4;
	if (Dt >= 0) test += 8;
	switch (test)
	{
	case  0: return s>0;
	case  1: return s>0;
	case  2: return s>0;
	case  3: return s>0;
	case  4: return s>0;
	case  5: if (At * Ct - Bt * Dt <  std::numeric_limits<float>::epsilon()) return s>0; break;
	case  6: return s>0;
	case  7: return s<0;
	case  8: return s>0;
	case  9: return s>0;
	case 10: if (At * Ct - Bt * Dt >= std::numeric_limits<float>::epsilon()) return s>0; break;
	case 11: return s<0;
	case 12: return s>0;
	case 13: return s<0;
	case 14: return s<0;
	case 15: return s<0;
	}

	return s < 0;
}