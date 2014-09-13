//********************************************
// Utils3d.cpp
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 29/01/97
// Modified : 14/09/98
//********************************************

#include "..\stdafx.h"


int round(double x)
{
	int _ceil = (int)ceil(x);
	int _floor = (int)floor(x);
	if((_ceil-x) < (x-_floor))
		return _ceil;
	else
		return _floor;
}

//**************************************
// SinAngle between two faces
// u  ^ v  = w
// |w| = |u||v| |sin(u,v)|
//**************************************
double SinAngle(CFace3d *pFace1,
							  CFace3d *pFace2)
{
	ASSERT(pFace1 != NULL);
	ASSERT(pFace2 != NULL);

	pFace1->CalculateNormal();
	pFace2->CalculateNormal();

	if(!pFace1->HasNormal() || !pFace2->HasNormal())
		return 0.0f;

	// Avoid illegal call of non-static function
	CVector3d w = CVector3d::Inner(pFace1->GetNormal(),pFace2->GetNormal());
	double Norm1 = pFace1->GetNormal()->GetNormL2();
	double Norm2 = pFace2->GetNormal()->GetNormL2();
	double Norm_w = w.GetNormL2();

	if(Norm1 * Norm2)
		return (Norm_w / (Norm1 * Norm2));
	else
	{
		TRACE("** Norms : %g %g\n",Norm1,Norm2);
		return 0.0;
	}
}


//**************************************
// CosAngle between two faces
// uv = |u||v| cos(u,v)
//**************************************
double CosAngle(CFace3d *pFace1,
							  CFace3d *pFace2)
{
	ASSERT(pFace1 != NULL);
	ASSERT(pFace2 != NULL);

	pFace1->CalculateNormal();
	pFace2->CalculateNormal();

	if(!pFace1->HasNormal() || !pFace2->HasNormal())
		return 0.0f;

	CVector3d *pU = pFace1->GetNormal();
	CVector3d *pV = pFace2->GetNormal();
	double NormU = pU->GetNormL2();
	double NormV = pV->GetNormL2();
	double product = NormU*NormV;

	// Check
	if(product == 0)
	{
		TRACE("** Norms : %g %g\n",NormU,NormV);
		return 0.0;
	}

	double scalar = ::Scalar(pU,pV);

	return (scalar / product);
}

//**************************************
// Angle between two faces (in radians)
// we use this formula
// uv = |u||v| cos(u,v)
// u  ^ v  = w
// |w| = |u||v| |sin(u,v)|
//**************************************
double Angle(CFace3d *pFace1,
						 CFace3d *pFace2)
{
	ASSERT(pFace1 != NULL);
	ASSERT(pFace2 != NULL);

	pFace1->CalculateNormal();
	pFace2->CalculateNormal();

	if(!pFace1->HasNormal() || !pFace2->HasNormal())
		return 0.0f;

	CVector3d *pU = pFace1->GetNormal();
	CVector3d *pV = pFace2->GetNormal();

	return Angle(pU,pV);
}

//**************************************
// Angle between two vectors (in radians)
// we use this formula
// uv = |u||v| cos(u,v)
// u  ^ v  = w
// |w| = |u||v| |sin(u,v)|
//**************************************
double Angle(CVector3d *pU,
						 CVector3d *pV)
{
	double NormU = pU->GetNormL2();
	double NormV = pV->GetNormL2();
	double product = NormU*NormV;

	// Check
	if(product == 0)
	{
		TRACE("** Norms : %g %g\n",NormU,NormV);
		return 0.0;
	}

	double scalar = ::Scalar(pU,pV);
	ASSERT(product != 0);
	double cosinus = scalar / product;

	// Sinus
	CVector3d w = CVector3d::Inner(pU,pV);
	double Norm_w = w.GetNormL2();

	double AbsSinus = Norm_w / product;

	// Remove degeneracy
	AbsSinus = (AbsSinus > 1) ? 1 : AbsSinus;
	AbsSinus = (AbsSinus < -1) ? -1 : AbsSinus;
	if(AbsSinus > 1)
	{
		TRACE("Remove degeneracy (AbsSinus : %lg)\n",AbsSinus);
	}

	if(cosinus >= 0)
		return asin(AbsSinus);
	else
		return (PI-asin(AbsSinus));
}


//**************************************
// AddFaceNoDuplicate
//**************************************
int AddFaceNoDuplicate(CArray3d<CFace3d> &array,
											 CFace3d *pFace)
{
	int size = array.GetSize();
	for(int i=0;i<size;i++)
	{
		if(array[i] == pFace)
			return 0;
	}
	array.Add(pFace);
	//TRACE("** AddFaceNoDuplicate : %x\n",pFace);

	return 1;
}

//**************************************
// AddFaceNoDuplicate
//**************************************
int AddFaceRecursive(CArray3d<CFace3d> &array,
										 CFace3d *pFace,
										 CVertex3d *pVertex)
{
	// Add vertex, if needed
	if(pFace->HasVertex(pVertex))
	{
		if(!AddFaceNoDuplicate(array,pFace))
			return 0;
	}
	else
		return 0;
	
	// Search in neighbors
	for(int i=0;i<3;i++)
	{
		CFace3d *pFaceNeighbor = pFace->f(i);
		if(pFaceNeighbor != NULL)
			if(pFaceNeighbor->HasVertex(pVertex))
				AddFaceRecursive(array,pFaceNeighbor,pVertex);
	}

	return 1;
}


//**************************************
// NbSharpEdge
//**************************************
int NbSharpEdge(CArray3d<CFace3d> &array,
								double threshold)
{
	int size = array.GetSize();

	// Check 
	if(size == 0)
		return 0;

	// Only one face, 2 sharp edges
	if(size == 1)
		return 2;

	int NbSharp = 0;

	for(int i=0;i<size-1;i++)
	{
		CFace3d *pFace = array[i];
		for(int j=i+1;j<size;j++)
		{
			CFace3d *pFaceTest = array[j];
			if(pFace->HasNeighbor(pFaceTest))
				NbSharp += (SinAngle(pFace,pFaceTest) >= threshold);
		}
	}
	
	return NbSharp;
}


//**************************************
// NormalSum
//**************************************
int NormalSum(CArray3d<CFace3d> &array,
							double *pSum)
{
	int size = array.GetSize();

	// Check 
	if(size == 0)
		return 0;

	// Only one face
	if(size == 1)
		return 0;

	double sum = 0;
	for(int i=0;i<size-1;i++)
	{
		CFace3d *pFace = array[i];
		for(int j=i+1;j<size;j++)
		{
			CFace3d *pFaceTest = array[j];
			if(pFace->HasNeighbor(pFaceTest))
				sum += SinAngle(pFace,pFaceTest);
		}
	}

	*pSum = sum;
	//TRACE("Value : %g\n",sum);
	return 1;
}

//**************************************
// NormalMax
// (in radians)
//**************************************
int NormalMax(CArray3d<CFace3d> &array,
							double *pMax)
{
	int size = array.GetSize();

	// Check 
	if(size == 0)
		return 0;

	// Only one face
	if(size == 1)
		return 0;

	double max = 0;
	for(int i=0;i<size-1;i++)
	{
		CFace3d *pFace = array[i];
		for(int j=i+1;j<size;j++)
		{
			CFace3d *pFaceTest = array[j];
			if(pFace->HasNeighbor(pFaceTest))
			{
				double sin = SinAngle(pFace,pFaceTest);
				max = (sin > max) ? sin : max;
			}
		}
	}

	*pMax = max;
	max = (max > 1) ? 1 : max; // precision work-around
	//TRACE("Value : %g\n",max);
	
	return 1;
}


int MaxAngleBetweenFaces(CArray3d<CFace3d> &array,
												 double *pMax)
{
	int size = array.GetSize();
	// Check 
	if(size == 0)
		return 0;
	// Only one face
	if(size == 1)
		return 0;
	double max = 0;
	for(int i=0;i<size-1;i++)
	{
		CFace3d *pFace = array[i];
		for(int j=i+1;j<size;j++)
		{
			CFace3d *pFaceTest = array[j];
			if(pFace->HasNeighbor(pFaceTest))
			{
				double angle = ::Angle(pFace,pFaceTest);
				max = (angle > max) ? angle : max;
			}
		}
	}

	*pMax = max;
	return 1;
}


//**************************************
// DistanceSquare
//**************************************
double DistanceSquare(CVertex3d *pVertex1,
										 CVertex3d *pVertex2)
{
	return ( (double)(pVertex1->x() - pVertex2->x()) * 
		       (double)(pVertex1->x() - pVertex2->x()) +
				   (double)(pVertex1->y() - pVertex2->y()) * 
					 (double)(pVertex1->y() - pVertex2->y()) +
				   (double)(pVertex1->z() - pVertex2->z()) * 
					 (double)(pVertex1->z() - pVertex2->z()) );
}

//**************************************
// Distance
//**************************************
double Distance(CVertex3d *pVertex1,
							  CVertex3d *pVertex2)
{
	return sqrt(DistanceSquare(pVertex1,pVertex2));
}

//**************************************
// DistanceVertexToLine
// Line go through Va and Vb
// Projected vertex is stored in 
// pVertexProjected
//**************************************
double DistanceVertexToLine(CVertex3d *pVertex,
													 CVertex3d *pVa,
													 CVertex3d *pVb,
													 CVertex3d *pVertexProjected)
{
	CVector3d VectorAC(pVa,pVertex);
	CVector3d VectorAB(pVa,pVb);
	double DistanceSquareAB = DistanceSquare(pVa,pVb);
	double temp = Scalar(&VectorAC,&VectorAB) / DistanceSquareAB;
	VectorAB *= (float)temp;
	pVertexProjected->Set(pVa);
	pVertexProjected->Move(&VectorAB);
	return Distance(pVertexProjected,pVertex);
}

//**************************************
// DistanceVertexToLine
// Line go through Va and Vb
// Projected vertex is stored in 
// pVertexProjected
//**************************************
int ProjectVertexOnLine(CVertex3d *pVertex,
 												 CVertex3d *pVa,
												 CVertex3d *pVb,
												 CVertex3d *pVertexProjected)
{
	CVector3d VectorAC(pVa,pVertex);
	CVector3d VectorAB(pVa,pVb);
	double DistanceSquareAB = DistanceSquare(pVa,pVb);
	if(DistanceSquareAB == 0.0f)
		return 0; // no line
	double temp = Scalar(&VectorAC,&VectorAB) / DistanceSquareAB;
	VectorAB *= (float)temp;
	pVertexProjected->Set(pVa);
	pVertexProjected->Move(&VectorAB);
	return 1;
}


//**************************************
// DistanceVertexToFace
// Projected vertex is stored in 
// pVertexProjected
//**************************************
double SquareDistanceVertexToFaceIfInf(CVertex3d *pVertex,
																			CFace3d *pFace,
																			CVertex3d *pVertexProjected,
																			double OldDistance)
{
	// Project point on triangle's plane
	ProjectPointOnFace(pVertex,pFace,pVertexProjected);

	if(DistanceSquare(pVertexProjected,pVertex) > OldDistance && 
		 !VertexInFace(pVertexProjected,pFace))
		return -1.0; // flag
	else
		return SquareDistanceVertexToFace(pVertex,pFace,pVertexProjected);
}

//**************************************
// DistanceVertexToFace
// Projected vertex is stored in 
// pVertexProjected
//**************************************
double SquareDistanceVertexToFace(CVertex3d *pVertex,
																 CFace3d *pFace,
																 CVertex3d *pVertexProjected)
{
	// ** DEBUG **

	// Project point on triangle's plane
	ProjectPointOnFace(pVertex,pFace,pVertexProjected);

	// Is point in triangle  ?
	if(VertexInFace(pVertexProjected,pFace))
	{
		// TRACE(" vertex in face... (projected : %g %g %g)\n",pVertexProjected->x(),pVertexProjected->y(),pVertexProjected->z());
		return DistanceSquare(pVertexProjected,pVertex);
	}

	// Out, also project "projected vertex" on 3 lines
	CVertex3d VertexProjectedOnLine[3];
	int i;
	for(i=0;i<3;i++)
	{
		ProjectVertexOnLine(pVertexProjected,pFace->v(i),pFace->v((i+1)%3),&VertexProjectedOnLine[i]);
		//// TRACE("  ProjectVertexOnLine : (%g %g %g)...\n",VertexProjectedOnLine[i].x(),VertexProjectedOnLine[i].y(),VertexProjectedOnLine[i].z());
	}

	// Is point on segment of face ?
	int success = 0;
	for(i=0;i<3;i++)
		if(VertexOnSegment(&VertexProjectedOnLine[i],pFace->v(i),pFace->v((i+1)%3)))
		{
			success = 1;
			VertexProjectedOnLine[i].SetFlag(1);
			//// TRACE("  vertex on segment of face (%g %g %g)...\n",VertexProjectedOnLine[i].x(),VertexProjectedOnLine[i].y(),VertexProjectedOnLine[i].z());
		}

	// At least one vertex on segment
	if(success)
	{
		// Compare with projected points in segment and vertices of triangle

		// Get nearest vertex
		CVertex3d *pVertexTmp = pFace->FindNearestVertex(pVertexProjected);
		CVertex3d VertexProjectedFinal(pVertexTmp);
		double MinDistance = DistanceSquare(pVertexProjected,pVertexTmp);

		for(i=0;i<3;i++)
			if(VertexProjectedOnLine[i].GetFlag())
			{
				VertexProjectedOnLine[i].SetFlag(0); // Restore
				double tmp = DistanceSquare(pVertexProjected,&VertexProjectedOnLine[i]);
				if(tmp <= MinDistance)
				{
					VertexProjectedFinal.Set(VertexProjectedOnLine[i]);
					MinDistance = tmp;
				}
			}
		pVertexProjected->Set(VertexProjectedFinal);
		// TRACE("  nearest vertex on face (%g %g %g)...\n",VertexProjectedFinal.x(),VertexProjectedFinal.y(),VertexProjectedFinal.z());
		return DistanceSquare(&VertexProjectedFinal,pVertex);
	}

	// Out, we must search nearest vertex
	CVertex3d *pVertexProjectedFinal = pFace->FindNearestVertex(pVertexProjected);

	pVertexProjected->Set(pVertexProjectedFinal);
	// TRACE("  nearest vertex on face (%g %g %g)...\n",pVertexProjectedFinal->x(),pVertexProjectedFinal->y(),pVertexProjectedFinal->z());
	return DistanceSquare(pVertexProjectedFinal,pVertex);
}


//********************************************
// SquareDistanceToFaceFast
//********************************************
// Search min distance between pVertex and 
// mesh's faces around pFace
//********************************************
double SquareDistanceToSetOfFace(CVertex3d *pVertex,
																CVertex3d *pVertexRef,
																CFace3d **ppFace,
																CArray3d<CFace3d> &ArrayFace)
{
	ASSERT(pVertex != NULL);
	ASSERT(pVertexRef != NULL);

	int size = ArrayFace.GetSize();
	ASSERT(size != 0);

	// For each face in set
	CVertex3d vertex;
	double distance = MAX_DOUBLE;

	for(int i=0;i<size;i++)
	{
		double tmp = SquareDistanceVertexToFaceIfInf(pVertex,ArrayFace[i],&vertex,distance);
		if(tmp < distance && tmp != -1.0f)
		{
			distance = tmp;
			pVertexRef->Set(vertex);
			*ppFace = ArrayFace[i];
		}
	}

	//TRACE("Dist : %g vertex(%g,%g,%g)",distance,pVertex->x(),pVertex->y(),pVertex->z());

	return distance;
}

//********************************************
// MaxSquareDistanceVertexFace
// For each vertex, search smallest distance 
// to set of faces (pArrayFace)
// Return max founded
//********************************************
double MaxSquareDistanceVertexFace(CArray3d<CVertex3d> *pArrayVertex,
				    											CArray3d<CFace3d> *pArrayFace,
																	CVertex3d **ppVertex,
																	CFace3d **ppFace)
{
	int NbVertex = pArrayVertex->GetSize();
	int NbFace = pArrayFace->GetSize();

	// Check
	ASSERT(NbVertex != 0 && NbFace != 0);

	// Non-optimized
	CVertex3d v;
	double max = 0.0;
	double distance;
	for(int i=0;i<NbVertex;i++)
	{
		// Get distance to set of faces
		distance = MAX_FLOAT;
		int j;
		for(j=0;j<NbFace;j++)
		{
			double d = ::SquareDistanceVertexToFace(pArrayVertex->GetAt(i),
				                                      pArrayFace->GetAt(j),&v);
			if(d < distance)
				distance = d;
		}
		// Get max for each vertex
	  if(distance > max)
		{
			max = distance;
			if(ppFace != NULL && ppVertex != NULL)
			{
				*ppFace = pArrayFace->GetAt(j);
				*ppVertex = pArrayVertex->GetAt(i);
			}
		}
	}
	return max;
}


//**************************************
// Scalar product
//**************************************
double Scalar(CVector3d *pV1,
							CVector3d *pV2)
{
	return ((double)pV1->x()*(double)pV2->x() +  
		      (double)pV1->y()*(double)pV2->y() + 
					(double)pV1->z()*(double)pV2->z());
}

//**************************************
// Opposite per coord
//**************************************
int OppositePerCoord(CVector3d *pV1,
										 CVector3d *pV2)
{
	return (pV1->x()*pV2->x() < 0.0f &&  
		      pV1->y()*pV2->y() < 0.0f &&  
					pV1->z()*pV2->z() < 0.0f);
}

//**************************************
// Scalar product
//**************************************
double Scalar(CVector3d &v1,
							CVector3d &v2)
{
	return ((double)v1.x() * (double)v2.x() +  
		      (double)v1.y() * (double)v2.y() + 
					(double)v1.z() * (double)v2.z());
}

//********************************************
// Inner
//********************************************
CVector3d Inner(CVector3d* u,
								CVector3d* v)
{
	// w = u ^ v
	CVector3d w;
	w.Set((float)((double)u->y()*(double)v->z()-(double)u->z()*(double)v->y()),
		    (float)((double)u->z()*(double)v->x()-(double)u->x()*(double)v->z()),
		    (float)((double)u->x()*(double)v->y()-(double)u->y()*(double)v->x()));
	return w;
}



//**************************************
// PlaneEquation
// Plane : ax + by + cz + d = 0
// Normal : a,b,c
// d : distance from origin along normal 
//     to the plane
//**************************************
int PlaneEquationFromFace(CFace3d *pFace,
													float *a,
													float *b,
													float *c,
													float *d)
{
	ASSERT(a != NULL);
	ASSERT(b != NULL);
	ASSERT(c != NULL);
	ASSERT(d != NULL);

	pFace->CalculateNormal();
	if(!pFace->HasNormal())
		return 0;

	CVector3d *pNormal = pFace->GetNormal();
	CVertex3d *pVertex = pFace->v1();

	ASSERT(pNormal != NULL);
	ASSERT(pVertex != NULL);

	// Store coeff
	*a = pNormal->x();
	*b = pNormal->y();
	*c = pNormal->z();
	*d = -(pNormal->x()*pVertex->x()+
		     pNormal->y()*pVertex->y()+
				 pNormal->z()*pVertex->z());

	return 1;
}

//**************************************
// VertexOnSegment
//**************************************
int VertexOnSegment(CVertex3d *pVertex,
										CVertex3d *pV1,
										CVertex3d *pV2)
{
	CVector3d v1(pV1,pVertex);	
	CVector3d v2(pV2,pVertex);	
	return (Scalar(v1,v2) <= 0);
}

//**************************************
// ProjectPointOnPlane
// Plane : ax + by + cz + d = 0
// We store projected vertex coordinates 
// in pVertexProjected
//**************************************
int ProjectPointOnPlane(CVertex3d *pVertex,
												CVertex3d *pVertexProjected,
												float a,
												float b,
												float c,
												float d)
{
	// We need valid plane
	float div = a*a+b*b+c*c;
	ASSERT(div != 0.0f);
	if(div == 0.0f)
		return 0;

	float x = pVertex->x();
	float y = pVertex->y();
	float z = pVertex->z();
	float t = -(a*x + b*y + c*z + d) / div;
	pVertexProjected->Set(a*t+x,b*t+y,c*t+z);

	return 1;
}

//**************************************
// ProjectPointOnFace
// We store projected vertex coordinates 
// in pVertexProjected
//**************************************
int ProjectPointOnFace(CVertex3d *pVertex,
												CFace3d *pFace,
												CVertex3d *pVertexProjected)
{
	// Get plane coeff
	float a,b,c,d;
	if(PlaneEquationFromFace(pFace,&a,&b,&c,&d))
		return ProjectPointOnPlane(pVertex,pVertexProjected,a,b,c,d);
	else
		return 0;
	//TRACE("** Plane equation : %f %f %f %f\n",a,b,c,d);
	// Project point on triangle's plane
}


//**************************************
// IsVertexInFace
// We know vertex is in plane of
// triangle, and just test whenever
// vertex V is in triangular face F
//**************************************
int VertexInFace(CVertex3d *pVertex,
								 CFace3d *pFace)
{
	CVector3d vv1(pVertex,pFace->v1());	
	CVector3d vv2(pVertex,pFace->v2());	
	CVector3d vv3(pVertex,pFace->v3());	

	CVector3d w1 = CVector3d::Inner(&vv1,&vv2);
	CVector3d w2 = CVector3d::Inner(&vv2,&vv3);
	CVector3d w3 = CVector3d::Inner(&vv3,&vv1);

	return (Scalar(&w1,&w2) >= 0 && 
		      Scalar(&w2,&w3) >= 0 && 
					Scalar(&w1,&w3) >= 0);
}


//**************************************
// Spring term 
// SIGMA(j,k)(k|Vj - Vk|^2);
//**************************************
double Spring(CVertex3d *pVertex,
						  double k)
{
	// Check
	int NbVertexNeighbor = pVertex->NbVertexNeighbor();
	if(NbVertexNeighbor == 0)
		return 0.0f;

	double sum = 0.0f;
	for(int i=0;i<NbVertexNeighbor;i++)
		sum += DistanceSquare(pVertex,pVertex->GetVertexNeighbor(i));

	return (sum*k);
}

//**************************************
// Area
//**************************************
double Area(CVertex3d *pV0,
						CVertex3d *pV1,
						CVertex3d *pV2)
{
	ASSERT(pV0 != NULL);
	ASSERT(pV1 != NULL);
	ASSERT(pV2 != NULL);
	CVector3d bc(pV1,pV2);
	CVector3d ba(pV1,pV0);
	//bc.Trace();
	//ba.Trace();
	CVector3d v = ::Inner(&bc,&ba);
	//v.Trace();
	return v.GetNormL2()/2.0;
}

//**************************************
// GetMeanArea
//**************************************
double GetMeanArea(CArray3d<CFace3d> *pArrayFace)
{
	int size = pArrayFace->GetSize();
	ASSERT(size > 0);
	double sum = 0.0f;
	for(int i=0;i<size;i++)
		sum += pArrayFace->GetAt(i)->Area();
	return (sum/(double)size);
}

//**************************************
// GetMinArea
//**************************************
double GetMinArea(CArray3d<CFace3d> *pArrayFace)
{
	int size = pArrayFace->GetSize();
	ASSERT(size > 0);
	double min = MAX_DOUBLE;
	for(int i=0;i<size;i++)
	{
		double area = pArrayFace->GetAt(i)->Area();
		//TRACE("Area : %g\n",area);
		min = (area < min) ? area : min;
	}
	return min;
}


//**************************************
// FormFunction
//**************************************
// Ratio (area(013)/area(012))
// Triangle : 012
// Vertex on which we want to evaluate 
// form function : 3
//**************************************
double FormFunction(CVertex3d *pV0,
										CVertex3d *pV1,
										CVertex3d *pV2,
										CVertex3d *pV3)
{
	double area = Area(pV0,pV1,pV2);
	ASSERT(area != 0.0);
	return Area(pV0,pV1,pV3)/area;
}


//**************************************
// IntersectionLineFace
//**************************************
int IntersectionLineFace(CVertex3d *pV0,
												 CVertex3d *pV1,
												 CFace3d *pFace,
												 CVertex3d *pVertexResult)
{
	float a,b,c,d;
	PlaneEquationFromFace(pFace,&a,&b,&c,&d);
	float i = pV1->x()-pV0->x();
	float j = pV1->y()-pV0->y();
	float k = pV1->z()-pV0->z();

	float den = a*i + b*j + c*k;
	float num = -(a * pV0->x() + b * pV0->y() + c * pV0->z() + d);

	// parallel or face contain line
	if(den == 0.0f)
		return 0;

	float t = num / den;
	pVertexResult->Set(pV0->x()+t*i,pV0->y()+t*j,pV0->z()+t*k);

	return VertexInFace(pVertexResult,pFace);
}

//********************************************
// IntersectionLineFaceRecursive
// (propagation from pFaceStart)
//********************************************
int IntersectionLineFaceRecursive(CVertex3d *pV0,
																	CVertex3d *pV1,
																	CFace3d *pFaceStart,
																	CVertex3d *pVertexResult,
																	CFace3d **ppFaceResult,
																	CArray3d<CFace3d> *pArrayFace,
																	int MaxNbFace)
{
	if(pArrayFace->GetSize() >= MaxNbFace)
		return 0;

	pArrayFace->Add(pFaceStart);
	if(IntersectionLineFace(pV0,pV1,pFaceStart,pVertexResult))
	{
		*ppFaceResult = pFaceStart;
		return 1;
	}

	//TRACE("Size : %d (max : %d)\n",pArrayFace->GetSize(),MaxNbFace);

	pFaceStart->SetFlag(1);

	// neighboring
	for(int i=0;i<3;i++)
	{
		CFace3d *pFace = pFaceStart->f(i);
		if(pFace != NULL)  // neighbor exists
			if(pFace->GetFlag() != 1) // not yet
				if(IntersectionLineFaceRecursive(pV0,pV1,pFace,
					             pVertexResult,ppFaceResult,pArrayFace,MaxNbFace))
					return 1;
	}

	return 0;
}

//********************************************
// NearestIntersectionWithLine
// First intersection founded (propagation
// from pFaceStart)
//********************************************
int NearestIntersectionWithLine(CVertex3d *pV0,
																CVertex3d *pV1,
																CFace3d *pFaceStart,
																CVertex3d *pVertexResult,
																CFace3d **ppFaceResult,
																int MaxNbFace)
{
	// Check
	ASSERT(pV0 != NULL);
	ASSERT(pV1 != NULL);
	ASSERT(pFaceStart != NULL);
	ASSERT(pVertexResult != NULL);

	// Use flag
	CArray3d<CFace3d> ArrayFace;
	int success = IntersectionLineFaceRecursive(pV0,pV1,pFaceStart,
		                                          pVertexResult,ppFaceResult,&ArrayFace,MaxNbFace);
	for(int i=0;i<ArrayFace.GetSize();i++)
		ArrayFace[i]->SetFlag(0);

	//TRACE("NbFace : %d\n",ArrayFace.GetSize());
	ArrayFace.SetSize(0);

	return success;
}


//********************************************
// NearestIntersectionWithLine
// Only search in pArrayFace
//********************************************
int NearestIntersectionWithLine(CArray3d<CFace3d> *pArrayFace,
																CVertex3d *pV0,
																CVertex3d *pV1,
																CVertex3d *pVertexResult,
																CFace3d **ppFaceResult,
																int *pNbVisitedFace)
{
	int success = 0;
	*ppFaceResult = NULL;
	int size = pArrayFace->GetSize();
	*pNbVisitedFace = size;
	double SqDistance = MAX_DOUBLE;
	// For each face
	for(int i=0;i<size;i++)
	{
		CVertex3d v;
		if(IntersectionLineFace(pV0,pV1,pArrayFace->GetAt(i),&v))
		{
			success++;
			double d = ::DistanceSquare(pV0,&v);
			if(d < SqDistance)
			{
				SqDistance = d;
				pVertexResult->Set(v);
				// this face contains intersection
				*ppFaceResult = pArrayFace->GetAt(i);
			}
		}
	}
	//TRACE("NbIntersection : %d\n",success);
	if(success)
	{
		ASSERT(ppFaceResult != NULL);
	}
	return success; // NbIntersection
}


//********************************************
// NearestIntersectionWithLine
// Only search in pFace and its neighbors
//********************************************
int NearestIntersectionWithLineFromFaceAndNeighbors(CFace3d *pFace,
																										CVertex3d *pV0,
																										CVertex3d *pV1,
																										CVertex3d *pVertexResult,
																										CFace3d **ppFaceResult)
{
	int success = 0;
	*ppFaceResult = NULL;
	double SqDistance = MAX_DOUBLE;
	CVertex3d v;

	// pFace alone
	if(IntersectionLineFace(pV0,pV1,pFace,&v))
	{
		success++;
		double d = ::DistanceSquare(pV0,&v);
		if(d < SqDistance)
		{
			SqDistance = d;
			pVertexResult->Set(v);
			*ppFaceResult = pFace;
		}
	}

	// Its neighbors
	for(int i=0;i<3;i++)
	{
		CFace3d *pFaceNeighbor = pFace->f(i);
		if(pFaceNeighbor != NULL) // neighbor
			if(IntersectionLineFace(pV0,pV1,pFaceNeighbor,&v))
			{
				success++;
				double d = ::DistanceSquare(pV0,&v);
				if(d < SqDistance)
				{
					SqDistance = d;
					pVertexResult->Set(v);
					*ppFaceResult = pFaceNeighbor;
				}
			}
	}

	if(success) { ASSERT((*ppFaceResult) != NULL); }
	return success;
}



int GravityCenter(CArray3d<CVertex3d> *pArray,
									CVertex3d *pVertexCenter)
{
	int size = pArray->GetSize();
	for(int j=0;j<3;j++)
	{
		float v = 0.0f;
		for(int i=0;i<size;i++)
			v += pArray->GetAt(i)->Get(j);
		v /= size;
		pVertexCenter->Set(j,v);
	}
	return 1;
}

// ** EOF **
