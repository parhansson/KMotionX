//********************************************
// Utils3d.h
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 29/01/97
// Modified : 19/02/97
//********************************************

#ifndef _UTILS_3D_
#define _UTILS_3D_

#include "Base3d.h"

// Rounding
int round(double x);

// Angle between faces
double SinAngle(CFace3d *pFace1,CFace3d *pFace2);
double CosAngle(CFace3d *pFace1,CFace3d *pFace2);
double Angle(CFace3d *pFace1,CFace3d *pFace2);
double Angle(CVector3d *pU,CVector3d *pV);

// Distance
double DistanceSquare(CVertex3d *pVertex1,CVertex3d *pVertex2);
double Distance(CVertex3d *pVertex1,CVertex3d *pVertex2);
double SquareDistanceVertexToFaceIfInf(CVertex3d *pVertex,CFace3d *pFace,
																			 CVertex3d *pVertexProjected,double OldDistance);
double SquareDistanceVertexToFace(CVertex3d *pVertex,CFace3d *pFace,CVertex3d *pVertexProjected);
double SquareDistanceToSetOfFace(CVertex3d *pVertex,CVertex3d *pVertexRef,
																 CFace3d **ppFace,CArray3d<CFace3d> &ArrayFace);
double MaxSquareDistanceVertexFace(CArray3d<CVertex3d> *pArrayVertex,CArray3d<CFace3d> *pArrayFace,
																	CVertex3d **ppVertex=NULL,CFace3d **pFace=NULL);

// Operators on vectors
CVector3d Inner(CVector3d* u,CVector3d* v);
double Scalar(CVector3d *pV1,CVector3d *pV2);
double Scalar(CVector3d &v1,CVector3d &v2);
int OppositePerCoord(CVector3d *pV1,CVector3d *pV2);

// Vertex / faces
int VertexInFace(CVertex3d *pVertex,CFace3d *pFace);

// Faces
int AddFaceNoDuplicate(CArray3d<CFace3d> &array,CFace3d *pFace);
int AddFaceRecursive(CArray3d<CFace3d> &array,CFace3d *pFace,CVertex3d *pVertex);

// Sharp edges
int NbSharpEdge(CArray3d<CFace3d> &array,double threshold);

// Normals 
int NormalSum(CArray3d<CFace3d> &array,double *pSum);
int NormalMax(CArray3d<CFace3d> &array,double *pMax);
int MaxAngleBetweenFaces(CArray3d<CFace3d> &array,double *pMax);

// Plane
int PlaneEquation(CFace3d *pFace,float *a,float *b,float *c,float *d);

// Vertex / segment
int VertexOnSegment(CVertex3d *pVertex,CVertex3d *pV1,CVertex3d *pV2);

// Projection
int ProjectPointOnPlane(CVertex3d *pVertex,CVertex3d *pVertexProjected,float a,float b,float c,float d);
int ProjectPointOnFace(CVertex3d *pVertex,CFace3d *pFace,CVertex3d *pVertexProjected);
int ProjectVertexOnLine(CVertex3d *pVertex,CVertex3d *pVa,CVertex3d *pVb,CVertex3d *pVertexProjected);

// Spring
float Spring(CVertex3d *pVertex,float k);

// Area
double Area(CVertex3d *pV0,CVertex3d *pV1,CVertex3d *pV2);
double FormFunction(CVertex3d *pV0,CVertex3d *pV1,CVertex3d *pV2,CVertex3d *pV3);
double GetMeanArea(CArray3d<CFace3d> *pArrayFace);
double GetMinArea(CArray3d<CFace3d> *pArrayFace);


// Intersections
int IntersectionLineFace(CVertex3d *pV0,CVertex3d *pV1,CFace3d *pFace,CVertex3d *pVertexResult);
int IntersectionLineFaceRecursive(CVertex3d *pV0,CVertex3d *pV1,CFace3d *pFaceStart,CVertex3d *pVertexResult,
																	CFace3d **ppFaceResult,CArray3d<CFace3d> *pArrayFace,int MaxNbFace);
int NearestIntersectionWithLine(CVertex3d *pV0,CVertex3d *pV1,CFace3d *pFaceStart,
																CVertex3d *pVertexResult,CFace3d **ppFaceResult,int MaxNbFace);
int NearestIntersectionWithLine(CArray3d<CFace3d> *pArrayFace,CVertex3d *pV0,CVertex3d *pV1,
																CVertex3d *pVertexResult,CFace3d **ppFaceResult,int *pNbVisitedFace);
int NearestIntersectionWithLineFromFaceAndNeighbors(CFace3d *pFace,CVertex3d *pV0,CVertex3d *pV1,
																CVertex3d *pVertexResult,CFace3d **ppFaceResult);


int GravityCenter(CArray3d<CVertex3d> *pArray,CVertex3d *pVertexCenter);


#endif // _UTILS_3D_

