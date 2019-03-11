/*
		 1         2         3         4         5         6         7         8
12345678901234567890123456789012345678901234567890123456789012345678901234567890

	File:	rs_math.h


	1999-Nov-04

*/

#ifndef		__RS_MATH_H
#define		__RS_MATH_H

/*------------------------------- Dependencies  ------------------------------*/

#include	<stdlib.h>

#include	<math.h>

#include	"abb_rr_rs_typedefs.h"

//	End Dependencies



#define PORT


class	CM4;

/*
struct	dVertex;
*/
struct	dVector;


#define PI			3.141592653589793

#define	PI2DEG		(180.0 / PI)

#define	DEG2PI		(PI / 180.0)

#define PIf			3.14159265f

#define	PI2DEGf		(180.0f / PIf)

#define	DEG2PIf		(PIf / 180.0f)


struct PORT	fVertex
{
	fVertex();

	fVertex ( const fVertex & V );

//	fVertex ( const dVector & dV );

	fVertex ( float X, float Y, float Z );

	float		X, Y, Z;

	void		Set (  float X,  float Y,  float Z );

	void		Set ( double X, double Y, double Z );

//	void		operator = ( const dVertex & dV );
	void		operator = ( const dVector & dV );

	fVertex		operator + ( const fVertex & V ) const;

	fVertex		operator - ( const fVertex & V ) const;

	fVertex		operator * ( const fVertex & V ) const;

	fVertex		operator / ( float D ) const;

	void		operator += ( const fVertex & dV );

	void		operator /= ( float D );
};


/*
struct PORT	dVertex
{
	dVertex();

	dVertex ( double X, double Y, double Z );

	dVertex ( const fVertex & fV );

	void		Set ( double X, double Y, double Z );

	double		X, Y, Z;

	void		operator = ( const fVertex & fV );

	dVertex		operator + ( const dVertex & V ) const;

	dVertex		operator - ( const dVertex & V ) const;
};
*/


struct PORT	dVector
{
	dVector();

	dVector ( double X, double Y, double Z );

	dVector ( const dVector & V );

	dVector ( const fVertex & fV );

	void		Set ( double X, double Y, double Z );

	double		Mag() const { return sqrt ( (X * X) + (Y * Y) + (Z * Z) ); }

	double		X, Y, Z;

	dVector		operator = ( const fVertex & fV );

	dVector		operator = ( const dVector & V );

	dVector		operator + ( const dVector & V ) const;

	dVector		operator - ( const dVector & V ) const;

	dVector		operator - () const;

	dVector		operator * ( const dVector & V ) const;

	dVector		operator * ( double D ) const;

	dVector		operator / ( double D ) const;

	void		operator += ( const dVector & dV );

	void		operator *= ( double D );

	void		operator /= ( double D );

	double &	operator [] ( uint i );

};	//	dVector


struct PORT	dPlane
{
	double	A, B, C, D;

};	//	dPlane



//	Round I up or down around About.  That is, return a value closest to I 
//	that is divisible by About with no remainder.
//
PORT int	Round ( int I, int About );


//	Floating point round of V up or down so that it is divisible by About
//	with no remainder.
//
PORT double	Round ( double V, double About );

//	Round to nearest integer.
//
PORT int	Round ( double V );


PORT BOOL	SameSigns ( const dVector & v1, const dVector & v2 );


PORT BOOL	SameDirection ( const dVector & v1, const dVector & v2 );


PORT BOOL	Behind ( const dVector & Src, 
			 	     const dVector & Dst,
					 const dVector & Pt );


//	I am pretty sure that A, B, C can be used as a normalized vector that
//	is normal to the plane.
//
PORT void	DefinePlane ( const dVector & Pt1, 
						  const dVector & Pt2, 
						  const dVector & Pt3,
						  double & A, double & B, double & C, double & D );


PORT void	DefinePlane2 ( const dVector & Pt1, 
						   const dVector & Pt2, 
						   const dVector & Pt3,
						   double & A, double & B, double & C, double & D );


PORT BOOL	OnPlane (		double A, double B, double C, double D, 
					  const dVector &	Pt,
							double		Close );


PORT BOOL	OnPlane (		double A, double B, double C, double D, 
					  const dVector &	Pt,
							double		Close, double & Closest );


PORT BOOL	InsideTriangle ( double A, double B, double C, double D,
							 const dVector & Pt1, 
							 const dVector & Pt2, 
							 const dVector & Pt3,
								   dVector & PtI );


PORT int	CPMostDef ( const dVector & V1,
						const dVector & V2,
						const dVector & V3 );


//	Return TRUE if P is inside triangle defined by V1 V2 V3.
//
//	Does two tests:  From P to + and - infinity.
//
//	Hopefully by testing from P in two different directions things like
//	border line cases will be dealt with.
//
//	If the point is found inside the triangle in the coordinate plane of CP
//	then tests distance from the point to the plane defined by V1 V2 V3.
//	In other words, to be inside the triangle the point must be on the
//	plane of V1 V2 V3.
//
//		CP		Coordinate plane triangle best defined in.  That
//				provided by CPMostDef().
//
//		DP		Max distance of P from plane V1 V2 V3 to be considered on 
//				the plane
//
//	2001-Sep-24.
//
//	If P is on the edge of the triangle (on one of the lines defined by
//	V1-V2, V2-V3, or V3-V1) then it is not inside.
//
//	2002-May-17
//
//		InLineClose		P must be greater then this distance (inside) from 
//						the end points of the triangle edge lines to be 
//						considered inside the line.
//
PORT BOOL	InsideTriangle (	   int			CP,
								   double		DP,
							 const dVector &	P,
							 const dVector &	V1, 
							 const dVector &	V2,
							 const dVector &	V3,
								   double		InLineClose = 0.0 );

//	2003-Jan-24		Based on InsideTriangle() just above.
//
//	Polygon[] may be 3 or more points.
//
//	Returns TRUE if a line from P to infinity crosses Polygon[] lines
//	an odd number of times.
//
PORT BOOL	InsidePolygon (		  int		CP,
								  double	DP,
							const dVector &	P,
							const dVector	PPt[], uint nPPts,
								  double	InLineClose = 0.0 );


PORT BOOL	LineIntersects ( int	CP,
							 const dVector & Va1, const dVector & Va2,
							 const dVector & Vb1, const dVector & Vb2 );

PORT BOOL	LineIntersects ( const dVector & Va1, const dVector & Va2,
							 const dVector & Vb1, const dVector & Vb2 );

PORT BOOL	LineIntersects ( const dVector & Va1, const dVector & Va2,
							 const dVector & Vb1, const dVector & Vb2,
								   dVector & Vi );


PORT BOOL	IntersectPoint ( double A1x, double A1y, double A2x, double A2y,
							 double B1x, double B1y, double B2x, double B2y,
							 double & Ix,  
							 double & Iy );

PORT BOOL	LineIntersects ( double A1x, double A1y, double A2x, double A2y,
							 double B1x, double B1y, double B2x, double B2y );

PORT BOOL	PolysOverlap ( const dVector &	AVa, 
						   const dVector &	AVb, 
						   const dVector &	AVc, 
						   const dVector &	BVa, 
						   const dVector &	BVb, 
						   const dVector &	BVc,
								 double		Close );

PORT double	Mag ( const dVector & V );

PORT float	Mag ( const fVertex & V );

//	Line - Plane intersection.
//
//	Returns FALSE if an intersection point is not found (if the line and plane
//	are nearly parallel).
//
PORT BOOL	GetIntersection ( double A, double B, double C, double D,
							  const dVector * pPt1, 
							  const dVector * pPt2, 
									dVector * pI );

PORT void	CalcNormal ( dVector * pPt1, dVector * pPt2, dVector * pPt3, 
						 dVector * pN );

PORT void	CalcNormal ( fVertex * pV1, fVertex * pV2, fVertex * pV3, 
						 float * n );

PORT void	CalcNormal ( float * pV1, float * pV2, float * pV3, 
						 float * n );

//	Normal to a line in 2D space.
//
//	The normal vector is normalized.
//
PORT void	CalcNormal2D ( float   Xs, float   Ys, 
						   float   Xe, float   Ye,
						   float & Xn, float & Yn );


/*
PORT void 	DiscXY ( double A, double B, fVertex * pV, int T4 );
*/

PORT dVector	NormalizeVector ( const dVector & V );

PORT void	NormalizeVector ( dVector * pN );

PORT void	NormalizeVector ( fVertex * pN );

PORT void	NormalizeVector2D ( float & Xn, float & Yn );

PORT int	CheckNormal ( dVector * pN, double Close = 0.0  );

PORT int	CheckNormal ( fVertex * pN, float  Close = 0.0f );

PORT double	DotProduct ( const dVector & V1, const dVector & V2 );

PORT void	CrossProduct ( const dVector & V1, 
						   const dVector & V2, 
						   const dVector & V3, dVector & N );

PORT dVector CrossProduct ( const dVector & A, const dVector & B );


PORT BOOL	IsParallel ( const dVector & A, const dVector & B );


PORT BOOL	IsParallel ( const dVector & A, const dVector & B, double Close );


PORT void	XYZToSphere ( double   X, double   Y, double   Z,
						  double & R, double & T, double & P );

PORT void	SphereToXYZ ( double   R, double   T, double   P,
						  double & X, double & Y, double & Z );

PORT double	Dist ( const dVector & PtA, const dVector & PtB );

PORT float	Dist ( const fVertex & Va, const fVertex & Vb );

PORT double	Dist ( double aX, double aY, double aZ,
				   double bX, double bY, double bZ );

PORT float	Dist2D ( float  Xs, float  Ys, float  Xe, float  Ye );

PORT double	Dist2D ( double Xs, double Ys, double Xe, double Ye );


//	Return shortest distance from point to line.
//
//	HWN 2000-Jan-20 pgs 1, 2.
//
//		Pt1, Pt2	Points defining the line.
//
//		PtA			The point from which we find the shortest distance to 
//					the line.
//
PORT double DistPt2Line ( const dVector & Pt1, 
						  const dVector & Pt2,
						  const dVector & PtA );

PORT float	DistPt2Line ( const fVertex & Pt1, 
						  const fVertex & Pt2,
						  const fVertex & PtA );

PORT float	DistPt2Line2D ( float X1, float Y1,
						    float X2, float Y2,
						    float XA, float YA );


//	Return point on line from which it is the shortest distance to a point
//	not on the line.
//
PORT dVector	LinePtAtMinDist ( const dVector & Pt1, 
								  const dVector & Pt2,
								  const dVector & PtA );


//	Return shortest distance from line to Z axis.
//
//	See HWN 2000-Mar-01 p2, 3.
//
//		Pt1, Pt2	Points defining the line.
//
//		P			Point on the line where the minimum distance occurs.
//					A line from this point to the Z axis will be 
//					perpendicular with the line and Z axis.
//
PORT double DistLine2Z ( const dVector & Pt1, 
						 const dVector & Pt2,
							   dVector & P );


//	Return shortest distance from point to plane.
//
//	From p182 of "Handbook of Mathmatical Formulas, ..., for Mathmaticians,
//	Scientists, Engineers".
//
//		A, B, C, D		Defines the plane.  Use DefinePlane().
//
//		P				Point from which the distince to the plane is found.
//
PORT double DistPt2Plane ( double A, double B, double C, double D,
						   const dVector & P );


//	2009-Sep-20
//
//	Returns minimum distance between two lines.
//
//	Uses DistLine2Z().
//
//	The points on lines A and B where the minimum distance is are returned
//	in PtA and PtB.
//
PORT double	DistLine2Line ( const dVector & PtA1, const dVector & PtA2,
						    const dVector & PtB1, const dVector & PtB2,
							      dVector & PtA,        dVector & PtB );



//	2003-Nov-05		From a vector to coordinate frame.
//
//	See illustration in HWN 2003-Nov-05 pg 1.
//
//	Points the frame's Z in the direction of the vector.
//
PORT void	XYZToM4 ( double X, double Y, double Z, CM4 & M );


PORT double	TriArea2D ( double x1, double y1,
						double x2, double y2,
						double x3, double y3 );


//	Returns the center of the triangle by averaging all the points.
//
PORT dVector	TriCenter ( const dVector & Pt1,
						    const dVector &	Pt2,
							const dVector &	Pt3 );


//	Return angle (degrees) between lines Pt1, PtA and PtA, Pt2.
//
PORT double	LinesAngle ( const dVector & Pt1, 
						 const dVector & PtA,
						 const dVector & Pt2 );


//	Assumes P is on a line defined by Pt1, Pt2.
//
//	Return TRUE if P is within the boundaries of Pt1, Pt2.
//
PORT int	IsPointInside ( const fVertex & P, 
						    const fVertex &	V1, 
							const fVertex &	V2 );

PORT int	IsPointInside ( const dVector & P, 
						    const dVector &	Pt1, 
							const dVector &	Pt2,
								  double	Close = 0.0 );


//	Return TRUE if P is on line defined by V1 V2.  
//
//	Tests for being within the end points V1 V2.
//
//		CP			Coordinate plane line best defined (generally, has most
//					length) in.  That provided by CPMostDef().
//
//		Close		Distance from line to be considered on the line.
//
PORT int	IsPointOnLine (		  int		CP,
								  double	Close,
						    const dVector & P, 
						    const dVector &	V1, 
							const dVector &	V2 );

//	A default "closeness" is used.  See implementation.
//
PORT int	IsPointOnLine ( const dVector & P, 
						    const dVector &	V1, 
							const dVector &	V2 );

PORT int	IsPointOnLine (		  double	OnLineClose,
						    const dVector & P, 
						    const dVector &	V1, 
							const dVector &	V2,
								  double	InLineClose = 0.0 );


//	Given X Y on a line defined by V1 V2, returns Z point on the line.
//
//	In other words, returns Z where X Y Z is a point on the line defined
//	by V1 V2.
//
PORT float	LinePtZ ( const fVertex & V1, 
					  const fVertex & V2, float X, float Y );

PORT float	LinePtY ( const fVertex & V1, 
					  const fVertex & V2, float X, float Z );

PORT float	LinePtX ( const fVertex & V1, 
					  const fVertex & V2, float Y, float Z );

PORT double	LinePtZ ( const dVector & V1, 
					  const dVector & V2, double X, double Y );

PORT double	LinePtY ( const dVector & V1, 
					  const dVector & V2, double X, double Z );

PORT double	LinePtX ( const dVector & V1, 
					  const dVector & V2, double Y, double Z );


//	Return the average between the two points.
//
//		AvgX = (V1.X + V2.X) / 2.0;
//		etc
//
PORT fVertex	PointAverage ( const fVertex & V1, const fVertex & V2 );

PORT dVector	PointAverage ( const dVector & V1, const dVector & V2 );



//	Set X, Y given the polar coordinates R, Theta.
//
//		Theta		In degrees.
//
PORT void	SetPolarXY ( float & X, float & Y, float R, float Theta );

PORT void	SetPolarXY ( double & X, double & Y, double R, double Theta );



//	Set X, Y, Z given spherical coordinates R, Theta, Phi.
//
//		Theta, Phi		In degrees.
//
void PORT	SetSphericalXYZ ( double & X, double & Y, double & Z,
							  double   R, double   T, double   P );


//	Set components of Min and Max if the respective components of V
//	are less than, greater than the current values.
//
PORT void	SetMinMax ( const dVector & V, dVector & Min, dVector & Max );



//	2003-Feb-03		Find roots of a quartic equation.
//
//		z^4  +  a3 z^3  +  a2 z^2  +  a1 z^1  +  a0  =  0
//
//		MaxErr		If non-zero then tests the equation with each root
//					and sets the corresponding BOOL TRUE if the result
//					is <= MaxErr.
//
//					If MaxErr is 0 then no verification is performed.
//
//	See HWN 2003-Feb-03 pg 7.
//
void PORT	QuarticRoots ( double a3, double a2, double a1, double a0,
						   BOOL & b1, double & z1, 
						   BOOL & b2, double & z2, 
						   BOOL & b3, double & z3, 
						   BOOL & b4, double & z4,
						   double MaxErr );


//	2003-Feb-03		Solve cubic.
//
//	Given cubic coefficients, a2, a1, a0, provide a real root if it exists.
//
//		z^3  +  a2 z^2  +  a1 z^1  +  a0  =  0
//
//	See HWN 2003-Feb-03 pg 5.
//
//	Return FALSE if no root exists.
//
PORT BOOL	CubicRoot ( double a2, double a1, double a0, double & Root );


//	2003-Feb-04		Return the cubed root.
//
//	If a is negative the result is negative.
//
PORT double CubedRoot ( double a );



#endif

//	rs_math.h
