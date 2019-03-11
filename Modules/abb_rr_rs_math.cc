/*
		 1         2         3         4         5         6         7
12345678901234567890123456789012345678901234567890123456789012345678901234567

	File:	rs_math.cpp
*/


/*------------------------------- Dependencies  ----------------------------*/
	
#include	"stdio.h"

#include	"float.h"

#include	"memory.h"

#include	"abb_rr_rs_typedefs.h"

#include	"abb_rr_m4.h"

#include	"abb_rr_error.h"

#include	"abb_rr_rs_math.h"

//	End Dependencies


#define PORT


//	A macro for specifiying filename and function name in diagnostics
//	call.
//
#define	F(f)	"(" __FILE__ ")" "  " f


//	Return TRUE if z is a root of the quartic polynomial with the specified 
//	coefficients.
//
static BOOL	TestQuarticRoot ( double a3, double a2, double a1, double a0,
							  double z,  double MaxErr );

//	Return TRUE if z is a root of the cubic polynomial with the specified 
//	coefficients.
//
static BOOL	TestCubicRoot ( double a2, double a1, double a0,
							double z );


//	Return TRUE if P1 is inside triangle defined by V1 V2 V3.
//
//	Tests line of P1 P2 intersecting sides of triangle V1 V2 V3 ounce.
//
//		CP		Coordinate plane triangle best defined in.  That
//				provided by CPMostDef().
//
static BOOL	InsideTriangle (	   int			CP,
							 const dVector &	P1,
							 const dVector &	P2,
							 const dVector &	V1, 
							 const dVector &	V2,
							 const dVector &	V3,
								   double		InLineClose = 0.0 );


static BOOL	InsidePolygon (		  int		CP,
							const dVector &	_P1,
							const dVector &	_P2,
							const dVector	PPt[], uint nPPts,
								   double	InLineClose );	//	= 0.0


//	Return TRUE if Vc is between Va and Vb by more than Close.
//
//	That is, if Vc is between Va and Vb and Vc is more than Close 
//	away from both Va and Vb then return TRUE.
//
static BOOL	Inside ( double Va, double Vb, double Vc, double Close );



/*-------------------------  fVertex::fVertex()  ---------------------------*/
/*
*/
fVertex::fVertex ()
{
	Set ( 0.0f, 0.0f, 0.0f );

}	//	fVertex::fVertex ()


/*-------------------------  fVertex::fVertex()  ---------------------------*/
/*
*/
fVertex::fVertex ( const fVertex & V )
{
	Set ( V.X, V.Y, V.Z );

}	//	fVertex::fVertex ()


/*-------------------------  fVertex::fVertex()  ---------------------------*/
/*

fVertex::fVertex ( const dVector & dV )
{
	Set ( (float)dV.X, (float)dV.Y, (float)dV.Z );

}	//	fVertex::fVertex ()
*/

/*-------------------------  fVertex::fVertex()  ---------------------------*/
/*
*/
fVertex::fVertex ( float _X, float _Y, float _Z )
{
	Set ( _X, _Y, _Z );

}	//	fVertex::fVertex ()


/*---------------------------  fVertex::Set()  -----------------------------*/
/*
*/
void	fVertex::Set ( float _X, float _Y, float _Z )
{
	X = _X;

	Y = _Y;

	Z = _Z;

}	//	fVertex::Set ()


/*---------------------------  fVertex::Set()  -----------------------------*/
/*
*/
void	fVertex::Set ( double _X, double _Y, double _Z )
{
	X = (float)_X;

	Y = (float)_Y;

	Z = (float)_Z;

}	//	fVertex::Set ()


/*----------------------  fVertex::operator = ()  --------------------------*/
/*

void	fVertex::operator = ( const dVertex & dV )
{
	X = (float)dV.X;

	Y = (float)dV.Y;

	Z = (float)dV.Z;

}	//	fVertex::operator = ()
*/
/*----------------------  fVertex::operator = ()  --------------------------*/
/*
*/
void	fVertex::operator = ( const dVector & dV )
{
	X = (float)dV.X;

	Y = (float)dV.Y;

	Z = (float)dV.Z;

}	//	fVertex::operator = ()


/*----------------------  fVertex::operator + ()  --------------------------*/
/*
*/
fVertex	fVertex::operator + ( const fVertex & fV ) const
{
	fVertex	R;

	R.X = X + fV.X;

	R.Y = Y + fV.Y;

	R.Z = Z + fV.Z;

	return R;

}	//	fVertex::operator + ()


/*----------------------  fVertex::operator - ()  --------------------------*/
/*
*/
fVertex	fVertex::operator - ( const fVertex & fV ) const
{
	fVertex	R;

	R.X = X - fV.X;

	R.Y = Y - fV.Y;

	R.Z = Z - fV.Z;

	return R;

}	//	fVertex::operator - ()


/*----------------------  fVertex::operator * ()  --------------------------*/
/*
*/
fVertex	fVertex::operator * ( const fVertex & fV ) const
{
	fVertex	R;

	R.X = X * fV.X;

	R.Y = Y * fV.Y;

	R.Z = Z * fV.Z;

	return R;

}	//	fVertex::operator * ()


/*-----------------------  fVertex::operator / ()  -------------------------*/
/*
*/
fVertex	fVertex::operator / ( float D ) const
{
	fVertex	R;

	R.X = X / D;

	R.Y = Y / D;

	R.Z = Z / D;

	return R;

}	//	fVertex::operator / ()


/*----------------------  fVertex::operator += ()  -------------------------*/
/*
*/
void	fVertex::operator += ( const fVertex & V )
{
	X += V.X;

	Y += V.Y;

	Z += V.Z;

}	//	fVertex::operator += ()


/*----------------------  fVertex::operator /= ()  -------------------------*/
/*
*/
void	fVertex::operator /= ( float D )
{
	X /= D;

	Y /= D;

	Z /= D;

}	//	fVertex::operator /= ()




/*-------------------------  dVector::dVector()  ---------------------------*/
/*
*/
dVector::dVector ()
{
	Set ( 0, 0, 0 );

}	//	dVector::dVector ()


/*-------------------------  dVector::dVector()  ---------------------------*/
/*
*/
dVector::dVector ( double _X, double _Y, double _Z )
{
	Set ( _X, _Y, _Z );

}	//	dVector::dVector ()


/*-------------------------  dVector::dVector()  ---------------------------*/
/*
*/
dVector::dVector ( const dVector & V )
{
	Set ( V.X, V.Y, V.Z );

}	//	dVector::dVector ()


/*-------------------------  dVector::dVector()  ---------------------------*/
/*
*/
dVector::dVector ( const fVertex & fV )
{
	Set ( fV.X, fV.Y, fV.Z );

}	//	dVector::dVector ()


/*--------------------------  dVector::Set()  ------------------------------*/
/*
*/
void	dVector::Set ( double _X, double _Y, double _Z )
{
	X = _X;

	Y = _Y;

	Z = _Z;

}	//	dVector::Set ()


/*----------------------  dVector::operator = ()  --------------------------*/
/*
*/
dVector	dVector::operator = ( const fVertex & fV )
{
	X = fV.X;

	Y = fV.Y;

	Z = fV.Z;

	return *this;

}	//	dVector::operator = ()


/*----------------------  dVector::operator = ()  --------------------------*/
/*
*/
dVector	dVector::operator = ( const dVector & V )
{
	X = V.X;

	Y = V.Y;

	Z = V.Z;

	return V;

}	//	dVector::operator = ()


/*----------------------  dVector::operator + ()  --------------------------*/
/*
*/
dVector	dVector::operator + ( const dVector & V ) const
{
	dVector	R;

	R.X = X + V.X;

	R.Y = Y + V.Y;

	R.Z = Z + V.Z;

	return R;

}	//	dVector::operator - ()


/*----------------------  dVector::operator - ()  --------------------------*/
/*
*/
dVector	dVector::operator - ( const dVector & V ) const
{
	dVector	R;

	R.X = X - V.X;

	R.Y = Y - V.Y;

	R.Z = Z - V.Z;

	return R;

}	//	dVector::operator - ()


/*----------------------  dVector::operator - ()  --------------------------*/
/*
*/
dVector	dVector::operator - () const
{
	dVector	R;

	R.X = -X;

	R.Y = -Y;

	R.Z = -Z;

	return R;

}	//	dVector::operator - ()


/*----------------------  dVector::operator * ()  --------------------------*/
/*
*/
dVector	dVector::operator * ( const dVector & V ) const
{
	dVector	R;

	R.X = X * V.X;

	R.Y = Y * V.Y;

	R.Z = Z * V.Z;

	return R;

}	//	dVector::operator * ()


/*----------------------  dVector::operator * ()  --------------------------*/
/*
*/
dVector	dVector::operator * ( double D ) const
{
	dVector	R;

	R.X = X * D;

	R.Y = Y * D;

	R.Z = Z * D;

	return R;

}	//	dVector::operator * ()


/*----------------------  dVector::operator / ()  --------------------------*/
/*
*/
dVector	dVector::operator / ( double D ) const
{
	dVector	R;

	R.X = X / D;

	R.Y = Y / D;

	R.Z = Z / D;

	return R;

}	//	dVector::operator / ()


/*----------------------  dVector::operator += ()  -------------------------*/
/*
*/
void	dVector::operator += ( const dVector & V )
{
	X += V.X;

	Y += V.Y;

	Z += V.Z;

}	//	dVector::operator += ()


/*----------------------  dVector::operator *= ()  -------------------------*/
/*
*/
void	dVector::operator *= ( double D )
{
	X *= D;

	Y *= D;

	Z *= D;

}	//	dVector::operator /= ()


/*----------------------  dVector::operator /= ()  -------------------------*/
/*
*/
void	dVector::operator /= ( double D )
{
	X /= D;

	Y /= D;

	Z /= D;

}	//	dVector::operator /= ()


/*----------------------  dVector::operator [] ()  -------------------------*/
/*
*/
double &	dVector::operator [] ( uint i )
{
	_ASSERTE ( i < 3 );		if ( i >= 3 ) i = 0;

	if ( i == 0 )  return X;

	if ( i == 1 )  return Y;

	return Z;

}	//	dVector::operator /= ()




/*---------------------------------  Round()  ------------------------------*/
/*
*/
int		Round ( int I, int About )
{
	if ( (I % About) > (About / 2) )
	{
		I = ((I / About) + 1) * About;
	}
	else
		I = (I / About) * About;

	return I;

}	//	Round()


/*---------------------------------  Round()  ------------------------------*/
/*
*/
double	Round ( double V, double About )
{
	int		bNeg = V < 0.0;		V = fabs ( V );

	double	Rmndr = V - ((double)((int)(V / About)) * About);
	
	if ( Rmndr >= 0.5 )
	{
		V = (floor ( V / About ) + 1.0) * About;
	}
	else
		V = floor ( V / About ) * About;
	//	V =       ( V / About ) * About;
	//	V = (int)(( V / About ) * About);

	if ( bNeg )  V = -V;

	return V;

}	//	Round()


/*---------------------------------  Round()  ------------------------------*/
/*
*/
int		Round ( double V )
{
	return (int)Round ( V, 1.0 );

}	//	Round()



BOOL	SameSigns ( const dVector & v1, const dVector & v2 )
{
	if ( (v1.X < 0.0) && (v2.X > 0.0) )  return FALSE;
	if ( (v2.X < 0.0) && (v1.X > 0.0) )  return FALSE;

	if ( (v1.Y < 0.0) && (v2.Y > 0.0) )  return FALSE;
	if ( (v2.Y < 0.0) && (v1.Y > 0.0) )  return FALSE;

	if ( (v1.Z < 0.0) && (v2.Z > 0.0) )  return FALSE;
	if ( (v2.Z < 0.0) && (v1.Z > 0.0) )  return FALSE;

	return TRUE;

}	//	SameSigns()


BOOL	SameDirection ( const dVector & v1, const dVector & v2 )
{
	//	If v1 and v2 are generally pointing in the same direction then
	//	the magnitude of their sum should be greater than either magnitude
	//	alone.

	double	M = ( v1 + v2 ).Mag();

	return (M > v1.Mag()) && (M > v2.Mag());

}	//	SameDirection()


BOOL	Behind ( const dVector & Src, 
				 const dVector & Dst,
				 const dVector & Pt )
{
	//	2008-Sep-24
	//
	//	Given the vector that points from Src to Dst.  
	//
	//	Return TRUE if Pt is on the side of Src that is points away from Dst.

	dVector	V = Dst - Src;

	dVector	A = Pt - Src;

	return DotProduct ( V, A ) < 0;		//	Right?

}	//	Behind()



PORT void	DefinePlane ( const dVector & Pt1, 
						  const dVector & Pt2, 
						  const dVector & Pt3,
						  double & A, double & B, double & C, double & D )
{
	double	x1, y1, z1;
	double	x2, y2, z2;
	double	x3, y3, z3;

	x1 = Pt1.X;		y1 = Pt1.Y;		z1 = Pt1.Z;

	x2 = Pt2.X;		y2 = Pt2.Y;		z2 = Pt2.Z;

	x3 = Pt3.X;		y3 = Pt3.Y;		z3 = Pt3.Z;

	A = ((y2 - y1) * (z3 - z1)) - ((z2 - z1) * (y3 - y1));

	B = ((z2 - z1) * (x3 - x1)) - ((x2 - x1) * (z3 - z1));

	C = ((x2 - x1) * (y3 - y1)) - ((y2 - y1) * (x3 - x1));

	D = -((x1 * A) + (y1 * B) + (z1 * C));

	//	Note that if A is (close) to 0 then Pt1, Pt2, and Pt3 are (nearly) in 
	//	a line parallel to the X axis.  Likewise for B (Y) and C (Z).

	//	2000-Jan-25
	//
	//	If all are very small then other things don't seem to work very
	//	well.
	//
	//	Try "normalizing" by dividing by one of 'em that is non-zero.
	//
	//	2011-Apr-15
	//
	//	Should probably use DefinePlane2().
	//
	double	q = 0.0, Q = 0.0;		
	
	double	a = fabs ( A );		if ( a > q )  q = a;
	double	b = fabs ( B );		if ( b > q )  q = b;
	double	c = fabs ( C );		if ( c > q )  q = c;
	double	d = fabs ( D );		if ( d > q )  q = d;

	//	if ( a > 0.0000000000001 )                    { q = a; Q = A; }
	//	else if ( (b > 0.0000000000001) && (b <= q) ) { q = b; Q = B; }
	//	else if ( (c > 0.0000000000001) && (c <= q) ) { q = c; Q = C; }
	//	else if ( (d > 0.0000000000001) && (d <= q) ) { q = d; Q = D; }
	//
	//	if ( a > 0.0000000000001 )               { q = a; Q = A; }
	//	if ( (b > 0.0000000000001) && (b <= q) ) { q = b; Q = B; }
	//	if ( (c > 0.0000000000001) && (c <= q) ) { q = c; Q = C; }
	//	if ( (d > 0.0000000000001) && (d <= q) ) { q = d; Q = D; }
	//
	if (  a > 0.00000001              ) { q = a; Q = A; }
	if ( (b > 0.00000001) && (b <= q) ) { q = b; Q = B; }
	if ( (c > 0.00000001) && (c <= q) ) { q = c; Q = C; }
	if ( (d > 0.00000001) && (d <= q) ) { q = d; Q = D; }

	if ( Q != 0.0 )
	{
		A /= Q;
		B /= Q;
		C /= Q;
		D /= Q;
	}

}	//	DefinePlane()


PORT void	DefinePlane2 ( const dVector & Pt1, 
						   const dVector & Pt2, 
						   const dVector & Pt3,
						   double & a, double & b, double & c, double & d )
{
	double	x1, y1, z1;
	double	x2, y2, z2;
	double	x3, y3, z3;

	x1 = Pt1.X;		y1 = Pt1.Y;		z1 = Pt1.Z;
	x2 = Pt2.X;		y2 = Pt2.Y;		z2 = Pt2.Z;
	x3 = Pt3.X;		y3 = Pt3.Y;		z3 = Pt3.Z;

	//	2011-Apr-14
	//
	//	From:	http://en.wikipedia.org/wiki/Plane_(geometry)
	//			http://en.wikipedia.org/wiki/Determinant
	//			http://mathworld.wolfram.com/Plane.html
	//
	//	     | x1  y1  z1 |
	//	D =  | x2  y2  z2 |
	//		 | x3  y3  z3 |
	//
	//    = x1y2z3 + y1z2x3 + z1x2y3 - z1y2x3 - y1x2z3 - x1z2y3
	//
	double	D =   (x1 * y2 * z3) + (y1 * z2 * x3) + (z1 * x2 * y3)
				- (z1 * y2 * x3) - (y1 * x2 * z3) - (x1 * z2 * y3);

	a = (y2 * z3) + (y1 * z2) + (z1 * y3) - (z1 * y2) - (y1 * z3) - (z2 * y3);

	b = (x1 * z3) + (z2 * x3) + (z1 * x2) - (z1 * x3) - (x2 * z3) - (x1 * z2);

	c = (x1 * y2) + (y1 * x3) + (x2 * y3) - (y2 * x3) - (y1 * x2) - (x1 * y3);

	if ( fabs ( D ) < 0.000001 )	//	Plane passes through origin?
	{
		//	The plane is passing through the origin.  D is 0.  So, we 
		//	can't do the stuff below.
		//
		//	I  * think *  doing this and using a, b, c as set above is ok.
		//
		d = 0;

		return;
	}

	d = 1.0;

	a = (-d / D) * a;

	b = (-d / D) * b;

	c = (-d / D) * c;

}	//	DefinePlane2()


PORT BOOL	OnPlane (		double A, double B, double C, double D, 
					  const dVector &	Pt,
							double		Close )
{
	//	Return TRUE if Pt is on plane A, B, C, D.

	double	V = fabs ( (A * Pt.X) + (B * Pt.Y) + (C * Pt.Z) + D );

	if ( Close == 0.0 )
	{
		return  V < 0.0000001;
	}
	else
		return  V < Close;

}	//	OnPlane()


PORT BOOL	OnPlane (		double A, double B, double C, double D, 
					  const dVector &	Pt,
							double		Close, double & Closest )
{
	//	Return TRUE if Pt is on plane A, B, C, D.

	double	V = fabs ( (A * Pt.X) + (B * Pt.Y) + (C * Pt.Z) + D );

	if ( V < Closest )  Closest = V;

	if ( Close == 0.0 )
	{
		return  V < 0.0000001;
	}
	else
		return  V < Close;

}	//	OnPlane()


PORT BOOL	InsideTriangle ( double A, double B, double C, double D,
							 const dVector & Pt1, 
							 const dVector & Pt2, 
							 const dVector & Pt3,
								   dVector & PtI )
{
	//	If a vector from PtI to infinity intersects a triangle edge
	//	just once then PtI is inside the triangle.

	//	Make it a 2D problem by working in the coordinate plane where
	//	the triangle's profile is the greatest.
	//
	double	dX =   fabs ( Pt1.X - Pt2.X )
				 + fabs ( Pt2.X - Pt3.X )
				 + fabs ( Pt3.X - Pt1.X );

	double	dY =   fabs ( Pt1.Y - Pt2.Y )
				 + fabs ( Pt2.Y - Pt3.Y )
				 + fabs ( Pt3.Y - Pt1.Y );

	double	dZ =   fabs ( Pt1.Z - Pt2.Z )
				 + fabs ( Pt2.Z - Pt3.Z )
				 + fabs ( Pt3.Z - Pt1.Z );

	uint	nI = 0;		//	Number of intersections.

	if ( (dZ < dX) && (dZ < dY) )
	{
		//	Work in X, Y coordinates.
		//
		double	X0 = (Pt1.X + Pt2.X + Pt3.X) / 3.0;
		
		double	Y0 = (Pt1.Y + Pt2.Y + Pt3.Y) / 3.0;

		if ( fabs ( PtI.X - X0 ) < 0.1 )  X0 = PtI.X + 1.0;

		if ( LineIntersects ( PtI.X, PtI.Y, X0, Y0, 
							  Pt1.X, Pt1.Y, Pt2.X, Pt2.Y ) )  nI++;

		if ( LineIntersects ( PtI.X, PtI.Y, X0, Y0, 
							  Pt2.X, Pt2.Y, Pt3.X, Pt3.Y ) )  nI++;

		if ( LineIntersects ( PtI.X, PtI.Y, X0, Y0, 
							  Pt3.X, Pt3.Y, Pt1.X, Pt1.Y ) )  nI++;

		return nI == 1;
	}

	if ( (dY < dX) && (dY < dZ) )
	{
		//	Work in Z, X coordinates.
		//
		double	Z0 = (Pt1.Z + Pt2.Z + Pt3.Z) / 3.0;
		
		double	X0 = (Pt1.X + Pt2.X + Pt3.X) / 3.0;

		if ( fabs ( PtI.Z - Z0 ) < 0.1 )  Z0 = PtI.Z + 1.0;

		if ( LineIntersects ( PtI.Z, PtI.X, Z0, X0, 
							  Pt1.Z, Pt1.X, Pt2.Z, Pt2.X ) )  nI++;

		if ( LineIntersects ( PtI.Z, PtI.X, Z0, X0, 
							  Pt2.Z, Pt2.X, Pt3.Z, Pt3.X ) )  nI++;

		if ( LineIntersects ( PtI.Z, PtI.X, Z0, X0, 
							  Pt3.Z, Pt3.X, Pt1.Z, Pt1.X ) )  nI++;

		return nI == 1;
	}

	//	Work in Y, Z coordinates.
	//
	double	Y0 = (Pt1.Y + Pt2.Y + Pt3.Y) / 3.0;	
		
	double	Z0 = (Pt1.Z + Pt2.Z + Pt3.Z) / 3.0;

	if ( fabs ( PtI.Y - Y0 ) < 0.1 )  Y0 = PtI.Y + 1.0;

	if ( LineIntersects ( PtI.Y, PtI.Z, Y0, Z0, 
						  Pt1.Y, Pt1.Z, Pt2.Y, Pt2.Z ) )  nI++;

	if ( LineIntersects ( PtI.Y, PtI.Z, Y0, Z0, 
						  Pt2.Y, Pt2.Z, Pt3.Y, Pt3.Z ) )  nI++;

	if ( LineIntersects ( PtI.Y, PtI.Z, Y0, Z0, 
						  Pt3.Y, Pt3.Z, Pt1.Y, Pt1.Z ) )  nI++;

	return nI == 1;

}	//	InsideTriangle()


PORT int	CPMostDef ( const dVector & V1,
						const dVector & V2,
						const dVector & V3 )
{
	//	Determine which coordinate plane the specified triangle's area
	//	is the largest.

	double	Axy = TriArea2D ( V1.X, V1.Y, V2.X, V2.Y, V3.X, V3.Y );

	double	Ayz = TriArea2D ( V1.Y, V1.Z, V2.Y, V2.Z, V3.Y, V3.Z );

	double	Azx = TriArea2D ( V1.Z, V1.X, V2.Z, V2.X, V3.Z, V3.X );

	if ( Axy > Ayz )
	{
		if ( Axy > Azx )
		{
			//	Work in X, Y;
			//
			return 1;
		}

		//	Work in Z, X.
		//
		return 2;
	}

	if ( Ayz > Azx )
	{
		//	Work in Y, Z.
		//
		return 3;
	}

	//	Work in Z, X.
	//
	return 2;

}	//	CPMostDef()


/*----------------------------  InsideTriangle()  --------------------------*/
/*
	See .h.
*/
PORT BOOL	InsideTriangle (	   int			CP,
								   double		DP,
							 const dVector &	P,
							 const dVector &	V1, 
							 const dVector &	V2,
							 const dVector &	V3,
								   double		InLineClose )	//	= 0.0
{
#define	POL_CLOSE		0.00001			//	2001-Sep-29

#define	POL_NOT_CLOSE	0.001			//	2001-Sep-29

	const char *	sW = F("InsideTriangle()");

	/*
	fVertex	P1, P0, V, P2, P3;

	fVertex	VMax (  FLT_MAX / 2,  FLT_MAX / 2,  FLT_MAX / 2 );

	P0 = PointAverage ( V1, V2 );

	V  = P0 - P;		NormalizeVector ( &V );

	P2 = V  * VMax;

	P0 = PointAverage ( V2, V3 );

	V  = P0 - P1;		NormalizeVector ( &V );

	P3 = V  * VMax;
	*/

	//	dVector	P1 (  DBL_MAX / 2,  DBL_MAX / 2,  DBL_MAX / 2 );
	//
	//	Use FLT_MAX to prevent overflow in distance calculations.
	//
	dVector	P1 (  FLT_MAX / 2,  FLT_MAX / 2,  FLT_MAX / 2 );

	dVector P0 ( P ), v1 ( V1 ), v2 ( V2 ), v3 ( V3 );
	
	//	double	R = DBL_MAX / 2;
	//
	double	R = FLT_MAX / 2;

	int		Cnt = 1;

	switch ( CP )
	{
		case 1:

			P0.Z = P1.Z = v1.Z = v2.Z = v3.Z = 0;

			//	2001-Sep-24.
			//
			//	If P is on the edge of the triangle then it is not inside.
			//
			if (    IsPointOnLine ( POL_CLOSE, P0, v1, v2, InLineClose )
				 || IsPointOnLine ( POL_CLOSE, P0, v2, v3, InLineClose )
				 || IsPointOnLine ( POL_CLOSE, P0, v3, v1, InLineClose ) )
			{
				return FALSE;
			}

			while (    IsPointOnLine ( POL_NOT_CLOSE, v1, P0, P1 )
				    || IsPointOnLine ( POL_NOT_CLOSE, v2, P0, P1 )
					|| IsPointOnLine ( POL_NOT_CLOSE, v3, P0, P1 ) )
			{
				if ( Cnt == 180 )
				{
					_ASSERTE ( "Oops." == 0 );

					throw E ( sW, "  case 1:  cnt == 180" );
				}

				double	X, Y;	SetPolarXY ( X, Y, R, 45.0 + (++Cnt * 2.0) );

				P1.Set ( X, Y, 0  );
			}

			break;

		case 2:

			P0.Y = P1.Y = v1.Y = v2.Y = v3.Y = 0;

			if (    IsPointOnLine ( POL_CLOSE, P0, v1, v2, InLineClose )
				 || IsPointOnLine ( POL_CLOSE, P0, v2, v3, InLineClose )
				 || IsPointOnLine ( POL_CLOSE, P0, v3, v1, InLineClose ) )
			{
				return FALSE;
			}

			while (    IsPointOnLine ( POL_NOT_CLOSE, v1, P0, P1 )
				    || IsPointOnLine ( POL_NOT_CLOSE, v2, P0, P1 )
					|| IsPointOnLine ( POL_NOT_CLOSE, v3, P0, P1 ) )
			{
				if ( Cnt == 180 )
				{
					_ASSERTE ( "Oops." == 0 );

					throw E ( sW, "  case 2:  cnt == 180" );
				}

				double	X, Y;	SetPolarXY ( X, Y, R, 45.0 + (++Cnt * 2.0) );

				P1.Set ( Y, 0, X  );
			}

			break;

		case 3:

			P0.X = P1.X = v1.X = v2.X = v3.X = 0;

			if (    IsPointOnLine ( POL_CLOSE, P0, v1, v2, InLineClose )
				 || IsPointOnLine ( POL_CLOSE, P0, v2, v3, InLineClose )
				 || IsPointOnLine ( POL_CLOSE, P0, v3, v1, InLineClose ) )
			{
				return FALSE;
			}

			while (    IsPointOnLine ( POL_NOT_CLOSE, v1, P0, P1 )
				    || IsPointOnLine ( POL_NOT_CLOSE, v2, P0, P1 )
					|| IsPointOnLine ( POL_NOT_CLOSE, v3, P0, P1 ) )
			{
				if ( Cnt == 180 )
				{
					_ASSERTE ( "Oops." == 0 );

					throw E ( sW, "  case 3:  cnt == 180" );
				}

				double	X, Y;	SetPolarXY ( X, Y, R, 45.0 + (++Cnt * 2.0) );

				P1.Set ( 0, X, Y );
			}

			break;

		default:

			_ASSERTE ( "Oops." == 0 );
	}


	/*
	if (    InsideTriangle ( CP, P, P2, V1, V2, V3 )
		 && InsideTriangle ( CP, P, P3, V1, V2, V3 ) )
	*/
	if ( DistPt2Line ( v1, v2, P0 ) < POL_CLOSE )
	{
		if ( ! IsPointInside ( P0, v1, v2, InLineClose ) )  return FALSE;
	}

	if ( DistPt2Line ( v2, v3, P0 ) < POL_CLOSE )
	{
		if ( ! IsPointInside ( P0, v2, v3, InLineClose ) )  return FALSE;
	}

	if ( DistPt2Line ( v3, v1, P0 ) < POL_CLOSE )
	{
		if ( ! IsPointInside ( P0, v3, v1, InLineClose ) )  return FALSE;
	}

	if ( InsideTriangle ( CP, P0, P1, V1, V2, V3, InLineClose ) )
	{
		dVector	dP;		dP  = P;

		dVector	dV1;	dV1 = V1;
		dVector	dV2;	dV2 = V2;
		dVector	dV3;	dV3 = V3;

		double	A, B, C, D;

		DefinePlane ( dV1, dV2, dV3, A, B, C, D );

		return DistPt2Plane ( A, B, C, D, dP ) < (double)DP;
	}

	return FALSE;
	
#undef	POL_NOT_CLOSE

#undef	POL_CLOSE

}	//	InsideTriangle()


/*----------------------------  InsidePolygon()  ---------------------------*/
/*
	See .h.

	Based on InsideTriangle().
*/
PORT BOOL	InsidePolygon (		  int		CP,
								  double	DP,
							const dVector &	P,
							const dVector	PPt[], uint nPPts, 
								  double	InLineClose )	//	= 0.0
{
#define	POL_CLOSE		0.00001			//	2001-Sep-29

#define	POL_NOT_CLOSE	0.001			//	2001-Sep-29

#define	MAX_PPTS		7

	const char *	sW = F("InsidePolygon()");

	_ASSERTE ( nPPts <= MAX_PPTS );		if ( nPPts > MAX_PPTS )  
	{
		throw E ( sW, "Too many polgon points." );
	}

	_ASSERTE ( nPPts >= 3 );		if ( nPPts < 3 )
	{
		throw E ( sW, "Too few polgon points." );
	}

	//	Use FLT_MAX to prevent overflow in distance calculations.
	//
	uint	i, j, Cnt = 0;

	dVector	P1 (  FLT_MAX / 2,  FLT_MAX / 2,  FLT_MAX / 2 );

	dVector P0 ( P );

	dVector	v [MAX_PPTS];	for ( i = 0; i < nPPts; i++ )  v[i] = PPt[i];
	
	double	R = FLT_MAX / 2;

	switch ( CP )
	{
		case 1:

			P0.Z = P1.Z = 0;	for ( i = 0; i < nPPts; i++ ) v[i].Z = 0;

			//	If P is on any polygon edge then it is not inside.
			//
			for ( j = nPPts - 1, i = 0; i < nPPts; j = i++ )
			{
				if ( IsPointOnLine ( POL_CLOSE, P0, v[j], v[i], InLineClose ) )
				{
					return FALSE;
				}
			}

			//	If any of the polygon edge points is on the test line then
			//	rotate the test line.
			//
			for ( ; ; )
			{
				for ( i = 0; i < nPPts; i++ )
				{
					if ( IsPointOnLine ( POL_NOT_CLOSE, v[i], P0, P1 ) ) break;
				}

				if ( i == nPPts )  break;

				_ASSERTE ( Cnt < 180 );		if ( Cnt >= 180 )
				{
					throw E ( sW, "  case 1:  cnt == 180" );
				}

				double	X, Y;	SetPolarXY ( X, Y, R, 45.0 + (++Cnt * 2.0) );

				P1.Set ( X, Y, 0  );
			}

			break;

		case 2:

			P0.Y = P1.Y = 0;	for ( i = 0; i < nPPts; i++ ) v[i].Y = 0;

			for ( j = nPPts - 1, i = 0; i < nPPts; j = i++ )
			{
				if ( IsPointOnLine ( POL_CLOSE, P0, v[j], v[i], InLineClose ) )
				{
					return FALSE;
				}
			}

			for ( ; ; )
			{
				for ( i = 0; i < nPPts; i++ )
				{
					if ( IsPointOnLine ( POL_NOT_CLOSE, v[i], P0, P1 ) ) break;
				}

				if ( i == nPPts )  break;

				_ASSERTE ( Cnt < 180 );		if ( Cnt >= 180 )
				{
					throw E ( sW, "  case 2:  cnt == 180" );
				}

				double	X, Y;	SetPolarXY ( X, Y, R, 45.0 + (++Cnt * 2.0) );

				P1.Set ( Y, 0, X  );
			}

			break;

		case 3:

			P0.X = P1.X = 0;	for ( i = 0; i < nPPts; i++ ) v[i].X = 0;

			for ( j = nPPts - 1, i = 0; i < nPPts; j = i++ )
			{
				if ( IsPointOnLine ( POL_CLOSE, P0, v[j], v[i], InLineClose ) )
				{
					return FALSE;
				}
			}

			for ( ; ; )
			{
				for ( i = 0; i < nPPts; i++ )
				{
					if ( IsPointOnLine ( POL_NOT_CLOSE, v[i], P0, P1 ) ) break;
				}

				if ( i == nPPts )  break;

				_ASSERTE ( Cnt < 180 );		if ( Cnt >= 180 )
				{
					throw E ( sW, "  case 3:  cnt == 180" );
				}

				double	X, Y;	SetPolarXY ( X, Y, R, 45.0 + (++Cnt * 2.0) );

				P1.Set ( 0, X, Y );
			}

			break;

		default:

			_ASSERTE ( "Oops." == 0 );
	}


	//	If the point is close to any polygon edge then test for it being
	//	well inside the edge end points.
	//
	for ( j = nPPts - 1, i = 0; i < nPPts; j = i++ )
	{
		if ( DistPt2Line ( v[j], v[i], P0 ) < POL_CLOSE )
		{
			if ( ! IsPointInside ( P0, v[j], v[i], InLineClose ) )  
			{
				return FALSE;
			}
		}
	}

	//	Point P passed all the boundary tests.
	//
	//	Inside the polygon?
	//
	if ( InsidePolygon ( CP, P0, P1, PPt, nPPts, InLineClose ) )
	{
		//	It is inside.  Close the to the polygon plane?
		//
		dVector	dP;		dP  = P;

		dVector	dV1;	dV1 = PPt[0];
		dVector	dV2;	dV2 = PPt[1];
		dVector	dV3;	dV3 = PPt[2];

		double	A, B, C, D;

		DefinePlane ( dV1, dV2, dV3, A, B, C, D );

		return DistPt2Plane ( A, B, C, D, dP ) < (double)DP;
	}

	return FALSE;
	
#undef	MAX_PPTS

#undef	POL_NOT_CLOSE

#undef	POL_CLOSE

}	//	InsidePolygon()


PORT BOOL	LineIntersects ( int	CP,
							 const dVector & Va1, const dVector & Va2,
							 const dVector & Vb1, const dVector & Vb2 )
{
	double	Ix, Iy;

	switch ( CP )
	{
		case 1:		//	X, Y

			if ( IntersectPoint ( Va1.X, Va1.Y, Va2.X, Va2.Y,
								  Vb1.X, Vb1.Y, Vb2.X, Vb2.Y, Ix, Iy ) )
			{
				//	Lines intersect on X Y plane.
				//
				//	But the lines may not both be on X Y ...
				//
				dVector	Pa ( Ix, Iy, LinePtZ ( Va1, Va2, Ix, Iy ) );

				dVector	Pb ( Ix, Iy, LinePtZ ( Vb1, Vb2, Ix, Iy ) );

				return (    IsPointInside ( Pa, Va1, Va2 )
					     && IsPointInside ( Pb, Vb1, Vb2 ) );
			}

			break;

		case 2:		//	Z, X

			if ( IntersectPoint ( Va1.Z, Va1.X, Va2.Z, Va2.X,
								  Vb1.Z, Vb1.X, Vb2.Z, Vb2.X, Ix, Iy ) )
			{
				dVector	Pa ( Iy, LinePtY ( Va1, Va2, Iy, Ix ), Ix );

				dVector	Pb ( Iy, LinePtY ( Vb1, Vb2, Iy, Ix ), Ix );

				return (    IsPointInside ( Pa, Va1, Va2 )
					     && IsPointInside ( Pb, Vb1, Vb2 ) );
			}

			break;

		case 3:		//	Y, Z

			if ( IntersectPoint ( Va1.Y, Va1.Z, Va2.Y, Va2.Z, 
								  Vb1.Y, Vb1.Z, Vb2.Y, Vb2.Z, Ix, Iy ) )
			{
				dVector	Pa ( LinePtX ( Va1, Va2, Ix, Iy ), Ix, Iy );

				dVector	Pb ( LinePtX ( Vb1, Vb2, Ix, Iy ), Ix, Iy );

				return (    IsPointInside ( Pa, Va1, Va2 )
					     && IsPointInside ( Pb, Vb1, Vb2 ) );
			}

			break;

		default:

			_ASSERTE ( "Oops." == 0 );
	}

	return FALSE;

}	//	LineIntersects()


PORT BOOL	LineIntersects ( const dVector & Va1, const dVector & Va2,
							 const dVector & Vb1, const dVector & Vb2 )
{
	/*
	//	Make it a 2D problem by working in the coordinate plane where
	//	the line's profile is the greatest.
	//
	//	Find the area of the triangle in each of the three planes.
	//	Use the plane where the triangle has the greates area.
	//
	double	Axy =   TriArea2D ( Va1.X, Va1.Y, Va2.X, Va2.Y, Vb1.X, Vb1.Y )
				  + TriArea2D ( Va2.X, Va2.Y, Vb1.X, Vb1.Y, Vb2.X, Vb2.Y )
				  + TriArea2D ( Vb2.X, Vb2.Y, Va1.X, Va1.Y, Va1.X, Va1.Y );

	double	Ayz =   TriArea2D ( Va1.Y, Va1.Z, Va2.Y, Va2.Z, Vb1.Y, Vb1.Z )
				  + TriArea2D ( Va2.Y, Va2.Z, Vb1.Y, Vb1.Z, Vb2.Y, Vb2.Z )
				  + TriArea2D ( Vb2.Y, Vb2.Z, Va1.Y, Va1.Z, Va1.Y, Va1.Z );

	double	Azx =   TriArea2D ( Va1.Z, Va1.X, Va2.Z, Va2.X, Vb1.Z, Vb1.X )
				  + TriArea2D ( Va2.Z, Va2.X, Vb1.Z, Vb1.X, Vb2.Z, Vb2.X )
				  + TriArea2D ( Vb2.Z, Vb2.X, Va1.Z, Va1.X, Va1.Z, Va1.X );

	double	Ix, Iy;

	//	All areas will be 0 if all points are colinear.

	//	float has FLT_DIG digits of precision.  See float.h.

	double	Prec = 1.0 / pow ( 10, FLT_DIG );

	if ( (Axy < Prec) && (Ayz < Prec) && (Azx < Prec) )
	{
		//	Use the lengths of the lines.
		//
		double	daX = Va1.X - Va2.X;
		double	daY = Va1.Y - Va2.Y;
		double	daZ = Va1.Z - Va2.Z;

		double	dbX = Vb1.X - Vb2.X;
		double	dbY = Vb1.Y - Vb2.Y;
		double	dbZ = Vb1.Z - Vb2.Z;

		Axy =   sqrt ( (daX * daX) + (daY * daY) )
			  + sqrt ( (dbX * dbX) + (dbY * dbY) );

		Ayz =   sqrt ( (daY * daY) + (daZ * daZ) )
			  + sqrt ( (dbY * dbY) + (dbZ * dbZ) );

		Azx =   sqrt ( (daZ * daZ) + (daX * daX) )
			  + sqrt ( (dbZ * dbZ) + (dbX * dbX) );
	}

	if ( Axy > Ayz )
	{
		if ( Axy > Azx )
		{
			//	Work in X, Y;
			//
			return IntersectPoint ( Va1.X, Va1.Y, Va2.X, Va2.Y,
									Vb1.X, Vb1.Y, Vb2.X, Vb2.Y, Ix, Iy );
		}

		//	Work in Z, X.
		//
		return IntersectPoint ( Va1.Z, Va1.X, Va2.Z, Va2.X,
								Vb1.Z, Vb1.X, Vb2.Z, Vb2.X, Ix, Iy );
	}

	if ( Ayz > Azx )
	{
		//	Work in Y, Z.
		//
		return IntersectPoint ( Va1.Y, Va1.Z, Va2.Y, Va2.Z, 
								Vb1.Y, Vb1.Z, Vb2.Y, Vb2.Z, Ix, Iy );
	}

	//	Work in Z, X.
	//
	return IntersectPoint ( Va1.Z, Va1.X, Va2.Z, Va2.X,
							Vb1.Z, Vb1.X, Vb2.Z, Vb2.X, Ix, Iy );
	*/
	dVector	Vi;

	return LineIntersects ( Va1, Va2, Vb1, Vb2, Vi );

}	//	LineIntersects()


PORT BOOL	LineIntersects ( const dVector & Va1, const dVector & Va2,
							 const dVector & Vb1, const dVector & Vb2,
								   dVector & Vi )
{
	//	Make it a 2D problem by working in the coordinate plane where
	//	the line's profile is the greatest.
	//
	//	Find the area of the triangle in each of the three planes.
	//	Use the plane where the triangle has the greatest area.
	//
	double	Axy =   TriArea2D ( Va1.X, Va1.Y, Va2.X, Va2.Y, Vb1.X, Vb1.Y )
				  + TriArea2D ( Va2.X, Va2.Y, Vb1.X, Vb1.Y, Vb2.X, Vb2.Y )
				  + TriArea2D ( Vb2.X, Vb2.Y, Va1.X, Va1.Y, Va1.X, Va1.Y );

	double	Ayz =   TriArea2D ( Va1.Y, Va1.Z, Va2.Y, Va2.Z, Vb1.Y, Vb1.Z )
				  + TriArea2D ( Va2.Y, Va2.Z, Vb1.Y, Vb1.Z, Vb2.Y, Vb2.Z )
				  + TriArea2D ( Vb2.Y, Vb2.Z, Va1.Y, Va1.Z, Va1.Y, Va1.Z );

	double	Azx =   TriArea2D ( Va1.Z, Va1.X, Va2.Z, Va2.X, Vb1.Z, Vb1.X )
				  + TriArea2D ( Va2.Z, Va2.X, Vb1.Z, Vb1.X, Vb2.Z, Vb2.X )
				  + TriArea2D ( Vb2.Z, Vb2.X, Va1.Z, Va1.X, Va1.Z, Va1.X );

	double	Ix, Iy;

	//	All areas will be 0 if all points are colinear.

	//	float has FLT_DIG digits of precision.  See float.h.

	double	daX = Va1.X - Va2.X;
	double	daY = Va1.Y - Va2.Y;
	double	daZ = Va1.Z - Va2.Z;

	double	dbX = Vb1.X - Vb2.X;
	double	dbY = Vb1.Y - Vb2.Y;
	double	dbZ = Vb1.Z - Vb2.Z;

	double	Prec = 1.0 / pow ( 10.0, (int)(FLT_DIG - 1) );

	if ( (Axy < Prec) && (Ayz < Prec) && (Azx < Prec) )
	{
		//	Use the lengths of the lines.
		//
		Axy =   sqrt ( (daX * daX) + (daY * daY) )
			  + sqrt ( (dbX * dbX) + (dbY * dbY) );

		Ayz =   sqrt ( (daY * daY) + (daZ * daZ) )
			  + sqrt ( (dbY * dbY) + (dbZ * dbZ) );

		Azx =   sqrt ( (daZ * daZ) + (daX * daX) )
			  + sqrt ( (dbZ * dbZ) + (dbX * dbX) );
	}

	if ( Axy > Ayz )
	{
		if ( Axy > Azx )
		{
			//	Work in X, Y;
			//
			if ( IntersectPoint ( Va1.X, Va1.Y, Va2.X, Va2.Y,
								  Vb1.X, Vb1.Y, Vb2.X, Vb2.Y, Ix, Iy ) )
			{
				Vi.X = Ix;		Vi.Y = Iy;

				if (   (fabs ( daX ) + fabs ( daY )) 
					 > (fabs ( dbX ) + fabs ( dbY )) )
				{
					if ( fabs ( daX ) > fabs ( daY ) )
					{
						Vi.Z = ((daZ * (Ix - Va2.X)) / daX) + Va2.Z;
					}
					else
						Vi.Z = ((daZ * (Iy - Va2.Y)) / daY) + Va2.Z;
				}
				else
				{
					if ( fabs ( dbX ) > fabs ( dbY ) )
					{
						Vi.Z = ((dbZ * (Ix - Vb2.X)) / dbX) + Vb2.Z;
					}
					else
						Vi.Z = ((dbZ * (Iy - Vb2.Y)) / dbY) + Vb2.Z;
				}

				return    IsPointOnLine ( 1, Prec, Vi, Va1, Va2 )
				//	   && IsPointOnLine ( 1, Prec, Vi, Va1, Va2 );
					   && IsPointOnLine ( 1, Prec, Vi, Vb1, Vb2 );
			}

			return FALSE;
		}

		//	Work in Z, X.
		//
		goto LineIntersects_InZX;
	}

	if ( Ayz > Azx )
	{
		//	Work in Y, Z.
		//
		if ( IntersectPoint ( Va1.Y, Va1.Z, Va2.Y, Va2.Z, 
							  Vb1.Y, Vb1.Z, Vb2.Y, Vb2.Z, Ix, Iy ) )
		{
			Vi.Y = Ix;		Vi.Z = Iy;

			if (   (fabs ( daY ) + fabs ( daZ )) 
				 > (fabs ( dbY ) + fabs ( dbZ )) )
			{
				if ( fabs ( daY ) > fabs ( daZ ) )
				{
					Vi.X = ((daX * (Ix - Va2.Y)) / daY) + Va2.X;
				}
				else
					Vi.X = ((daX * (Iy - Va2.Z)) / daZ) + Va2.X;
			}
			else
			{
				if ( fabs ( dbY ) > fabs ( dbZ ) )
				{
					Vi.X = ((dbX * (Ix - Vb2.Y)) / dbY) + Vb2.X;
				}
				else
					Vi.X = ((dbX * (Iy - Vb2.Z)) / dbZ) + Vb2.X;
			}

			return    IsPointOnLine ( 3, Prec, Vi, Va1, Va2 )
			//	   && IsPointOnLine ( 3, Prec, Vi, Va1, Va2 );
				   && IsPointOnLine ( 3, Prec, Vi, Vb1, Vb2 );
		}

		return FALSE;
	}

LineIntersects_InZX:

	//	Work in Z, X.
	//
	if ( IntersectPoint ( Va1.Z, Va1.X, Va2.Z, Va2.X,
						  Vb1.Z, Vb1.X, Vb2.Z, Vb2.X, Ix, Iy ) )
	{
		Vi.Z = Ix;		Vi.X = Iy;

		if (   (fabs ( daZ ) + fabs ( daX )) 
			 > (fabs ( dbZ ) + fabs ( dbX )) )
		{
			if ( fabs ( daZ ) > fabs ( daX ) )
			{
				Vi.Y = ((daY * (Ix - Va2.Z)) / daZ) + Va2.Y;
			}
			else
				Vi.Y = ((daY * (Iy - Va2.X)) / daX) + Va2.Y;
		}
		else
		{
			if ( fabs ( dbZ ) > fabs ( dbX ) )
			{
				Vi.Y = ((dbY * (Ix - Vb2.Z)) / dbZ) + Vb2.Y;
			}
			else
				Vi.Y = ((dbY * (Iy - Vb2.X)) / dbX) + Vb2.Y;
		}

		return    IsPointOnLine ( 2, Prec, Vi, Va1, Va2 )
		//	   && IsPointOnLine ( 2, Prec, Vi, Va1, Va2 );
			   && IsPointOnLine ( 2, Prec, Vi, Vb1, Vb2 );
	}

	return FALSE;

}	//	LineIntersects()


	
PORT BOOL	IntersectPoint ( double A1x, double A1y, double A2x, double A2y,
							 double B1x, double B1y, double B2x, double B2y,
							 double & Ix,  
							 double & Iy )
{
	//	If A intersect B (and the intersection point is inside or on the
	//	ends of A and B) then return TRUE.

	//	float has FLT_DIG digits of precision.  See float.h.

	double	Prec = 1.0 / pow ( 10.0, (int)(FLT_DIG - 1) );

	//	A vertical?
	//
	if ( fabs ( A1x - A2x ) < Prec )
	{
		Ix = A1x;

		//	B parallel?
		//
		if ( fabs ( B1x - B2x ) < Prec )
		{
			//	But not colinear ...
			//
			if ( fabs ( A1x - B1x ) > Prec )  return FALSE;

			//	Assume colinear.

			//	If any end point is the same ...
			//
			if ( fabs ( B1y - A1y ) < Prec )  { Iy = A1y;	return TRUE; }

			if ( fabs ( B2y - A1y ) < Prec )  { Iy = A1y;	return TRUE; }

			if ( fabs ( B1y - A2y ) < Prec )  { Iy = A2y;	return TRUE; }

			if ( fabs ( B2y - A2y ) < Prec )  { Iy = A2y;	return TRUE; }

			//	End point inside the points of the other ...
			//
			if ( fabs ( A1y - A2y ) < Prec )  return FALSE;		//	A a point?

			if ( A1y > A2y )
			{
				if ( (B1y < A1y) && (B1y > A2y) )  { Iy = B1y;	return TRUE; }

				if ( (B2y < A1y) && (B2y > A2y) )  { Iy = B2y;	return TRUE; }

				return FALSE;
			}

			if ( (B1y < A2y) && (B1y > A1y) )  { Iy = B1y;	return TRUE; }

			if ( (B2y < A2y) && (B2y > A1y) )  { Iy = B2y;	return TRUE; }

			return FALSE;
		}


		//	B's slope and y intercept.
		//
		double	Bm = (B2y - B1y) / (B2x - B1x);

		double	Bb = B1y - (Bm * B1x);

		//	A intersecting B at A1x, Yi.
		//
		double	Yi = (Bm * A1x) + Bb;

		double	Xi = (A1x + A2x) / 2.0;

		Iy = (float)Yi;

		//	If Yi not on end point of A ...
		//
		if ( ! ((fabs ( Yi - A1y ) < Prec) || (fabs ( Yi - A2y ) < Prec)) )
		{
			if ( fabs ( A1y - A2y ) < Prec )  return FALSE;		//	A a point?

			//	If Yi not inside A ...
			//
			if ( A1y > A2y )  
			{
				if ( (Yi > A1y) || (Yi < A2y) )  return FALSE;
			}
			else
				if ( (Yi > A2y) || (Yi < A1y) )  return FALSE;
		}

		//	Yi is on end or inside of A.
		
		//	Check B.

		if ( Bm < 0.5 )
		{
			//	Work on X for B.

			//	If on B end point ...
			//
			if (    (fabs ( Xi - B1x ) < Prec) 
				 || (fabs ( Xi - B2x ) < Prec) )  return TRUE;

			if (    (fabs ( B1y - B2y ) < Prec)		//	B a point?
				 && (fabs ( B1x - B2x ) < Prec) )  return FALSE;

			//	If inside B ...
			//
			if ( B1x > B2x )
			{
				return (Xi < B1x) && (Xi > B2x);
			}

			return (Xi < B2x) && (Xi > B1x);
		}

		//	Work on Y for B.

		//	If on B end point ...
		//
		if (    (fabs ( Yi - B1y ) < Prec) 
			 || (fabs ( Yi - B2y ) < Prec) )  return TRUE;

		if (    (fabs ( B1y - B2y ) < Prec)		//	B a point?
			 && (fabs ( B1x - B2x ) < Prec) )  return FALSE;


		//	If inside B ...
		//
		if ( B1y > B2y )
		{
			return (Yi < B1y) && (Yi > B2y);
		}

		return (Yi < B2y) && (Yi > B1y);
	}


	//	B vertical?
	//
	if ( fabs ( B1x - B2x ) < Prec )
	{
		//	A's slope and y intercept.
		//
		double	Am = (A2y - A1y) / (A2x - A1x);

		double	Ab = A1y - (Am * A1x);

		//	B intersecting A at B1x, Yi.
		//
		double	Yi = (Am * B1x) + Ab;

		double	Xi = (B1x + B2x) / 2.0;

		Ix = (float)Xi;

		Iy = (float)Yi;

		//	If Yi not on end point of B ...
		//
		if ( ! ((fabs ( Yi - B1y ) < Prec) || (fabs ( Yi - B2y ) < Prec)) )
		{
			if ( fabs ( B1y - B2y ) < Prec )  return FALSE;		//	B a point?

			//	If Yi not inside B ...
			//
			if ( B1y > B2y )  
			{
				if ( (Yi > B1y) || (Yi < B2y) )  return FALSE;
			}
			else
				if ( (Yi > B2y) || (Yi < B1y) )  return FALSE;
		}

		//	Yi is on end or inside of B.

		//	Check A.

		if ( Am < 0.5 )
		{
			//	Work on X for A.

			//	If on A end point ...
			//
			if (    (fabs ( Xi - A1x ) < Prec) 
				 || (fabs ( Xi - A2x ) < Prec) )  return TRUE;

			if (    (fabs ( A1y - A2y ) < Prec)		//	A a point?
				 && (fabs ( A1x - A2x ) < Prec) )  return FALSE;

			//	If inside A ...
			//
			if ( A1x > A2x )
			{
				return (Xi < A1x) && (Xi > A2x);
			}

			return (Xi < A2x) && (Xi > A1x);
		}

		//	Work on Y for A.

		//	If on A end point ...
		//
		if (    (fabs ( Yi - A1y ) < Prec) 
			 || (fabs ( Yi - A2y ) < Prec) )  return TRUE;

		if (    (fabs ( A1y - A2y ) < Prec)		//	A a point?
			 && (fabs ( A1x - A2x ) < Prec) )  return FALSE;

		//	If inside A ...
		//
		if ( A1y > A2y )
		{
			return (Yi < A1y) && (Yi > A2y);
		}

		return (Yi < A2y) && (Yi > A1y);
	}


	//	Neither A or B are vertical.

	//	A's slope and y intercept.
	//
	double	Am = (A2y - A1y) / (A2x - A1x);

	double	Ab = A1y - (Am * A1x);

	//	B's slope and y intercept.
	//
	double	Bm = (B2y - B1y) / (B2x - B1x);

	double	Bb = B1y - (Bm * B1x);

	//	Parallel lines?
	//
	if ( fabs ( Am - Bm ) < Prec )
	{
		if ( fabs ( Ab - Bb ) > Prec )  return FALSE;

		//	Assume colinear.

		if ( fabs ( Am ) < 0.5 )	//	Work on the axis with greatest 
		{							//	definition.
			//	Work on X.

			if ( (fabs ( A1x - B1x ) < Prec) && (fabs ( A1x - B2x ) < Prec) )
			{
				Ix = A1x;	Iy = A1y;	return TRUE;
			}

			if ( (fabs ( A2x - B1x ) < Prec) && (fabs ( A2x - B2x ) < Prec) )
			{
				Ix = A2x;	Iy = A2y;	return TRUE;
			}

			if (    (fabs ( A1x - A2x ) < Prec)	   //	A a point?
				 && (fabs ( A1y - A2y ) < Prec) )  return FALSE;		

			if ( A1x > A2x )
			{
				if ( (B1x < A1x) && (B1x > A2x) )
				{
					Ix = B1x;	Iy = B1y;	return TRUE;
				}

				if ( (B2x < A1x) && (B2x > A2x) )
				{
					Ix = B1x;	Iy = B1y;	return TRUE;
				}

				return FALSE;
			}

			if ( (B1x < A2x) && (B1x > A1x) )
			{
				Ix = B1x;	Iy = B1y;	return TRUE;
			}

			if ( (B2x < A2x) && (B2x > A1x) )
			{
				Ix = B1x;	Iy = B1y;	return TRUE;
			}

			return FALSE;
		}

		//	Work on Y.

		if ( (fabs ( A1y - B1y ) < Prec) && (fabs ( A1y - B2y ) < Prec) )
		{
			Ix = A1x;	Iy = A1y;	return TRUE;
		}

		if ( (fabs ( A2y - B1y ) < Prec) && (fabs ( A2y - B2y ) < Prec) )
		{
			Ix = A2x;	Iy = A2y;	return TRUE;
		}

		if (    (fabs ( A1x - A2x ) < Prec)	   //	A a point?
			 && (fabs ( A1y - A2y ) < Prec) )  return FALSE;		

		if ( A1y > A2y )
		{
			if ( (B1y < A1y) && (B1y > A2y) )
			{
				Ix = B1x;	Iy = B1y;	return TRUE;
			}

			if ( (B2y < A1y) && (B2y > A2y) )
			{
				Ix = B1x;	Iy = B1y;	return TRUE;
			}

			return FALSE;
		}

		if ( (B1y < A2y) && (B1y > A1y) )
		{
			Ix = B1x;	Iy = B1y;	return TRUE;
		}

		if ( (B2y < A2y) && (B2y > A1y) )
		{
			Ix = B1x;	Iy = B1y;	return TRUE;
		}

		return FALSE;
	}

	//	Lines not parallel (Am != Bm).

	//	Intersection at Xi, Yi.
	//
	double	Xi = (Bb - Ab) / ( Am - Bm);		Ix = (float)Xi;

	double	Yi = (Am * Xi) + Ab;				Iy = (float)Yi;

	//	Test line A.

	if ( fabs ( Am ) < 0.5 )	//	Work on the axis with greatest 
	{							//	definition.
		//	Work on X for A.

		//	If not on A end point ...
		//
		if ( ! ((fabs ( Xi - A1x ) < Prec) || (fabs ( Xi - A2x ) < Prec)) )
		{
			if (    (fabs ( A1x - A2x ) < Prec)	   //	A a point?
				 && (fabs ( A1y - A2y ) < Prec) )  return FALSE;		

			//	If not inside A ...
			//
			if ( A1x > A2x )
			{
				if ( ! ((Xi < A1x) && (Xi > A2x)) )  return FALSE;
			}
			else
				if ( ! ((Xi < A2x) && (Xi > A1x)) )  return FALSE;
		}
	}
	else
	{
		//	Work on Y for A.

		//	If not on A end point ...
		//
		if ( ! ((fabs ( Yi - A1y ) < Prec) || (fabs ( Yi - A2y ) < Prec)) )
		{
			if (    (fabs ( A1x - A2x ) < Prec)	   //	A a point?
				 && (fabs ( A1y - A2y ) < Prec) )  return FALSE;		

			//	If not inside A ...
			//
			if ( A1y > A2y )
			{
				if ( ! ((Yi < A1y) && (Yi > A2y)) )  return FALSE;
			}
			else
				if ( ! ((Yi < A2y) && (Yi > A1y)) )  return FALSE;
		}
	}

	//	Xi, Yi is either on the end or inside of A.

	//	Test line B.

	if ( fabs ( Bm ) < 0.5 )
	{
		//	Work on X for B.

		if (    (fabs ( Xi - B1x ) < Prec)		//	On point of B?
			 || (fabs ( Xi - B2x ) < Prec) )  return TRUE;

		if (    (fabs ( B1y - B2y ) < Prec)		//	B a point?
			 && (fabs ( B1x - B2x ) < Prec) )  return FALSE;

		if ( B1x > B2x )
		{
			return (Xi < B1x) && (Xi > B2x);
		}

		return (Xi < B2x) && (Xi > B1x);
	}

	//	Work on Y for B.

	if (    (fabs ( Yi - B1y ) < Prec)		//	On point of B?
		 || (fabs ( Yi - B2y ) < Prec) )  return TRUE;

	if (    (fabs ( B1y - B2y ) < Prec)		//	B a point?
		 && (fabs ( B1x - B2x ) < Prec) )  return FALSE;

	if ( B1y > B2y )
	{
		return (Yi < B1y) && (Yi > B2y);
	}

	return (Yi < B2y) && (Yi > B1y);

}	//	IntersectPoint()


//	Note:	This funct does not completely check for the intersection being
//			inside the boundaries of the lines.  
//
//			Use IntersectPoint().
//
//			This function was originally written to test for a point being
//			inside a triangle where the intersection boundary on just one
//			line (the triangle edge) is important (and the correct "side"
//			of the other line).
//
PORT BOOL	LineIntersects ( double A1x, double A1y, double A2x, double A2y,
							 double B1x, double B1y, double B2x, double B2y )
{
	//	If a vector from A1 through A2 intersects B then return TRUE.
	//
	//	The intersection must be on the A2 side of A1 and be within B1 and
	//	B2.

	//	A vertical?
	//
	if ( fabs ( A1x - A2x ) < 0.0000001 )
	{
		//	B parallel?
		//
		if ( B1x == B2x )  return FALSE;

		//	A not within B?
		//
		if ( B1x < B2x )
		{
			if ( (A1x < B1x) || (A1x > B2x) )  return FALSE;
		}
		else
			if ( (A1x < B2x) || (A1x > B1x) )  return FALSE;

		//	B's slope and y intercept.
		//
		double	Bm = (B2y - B1y) / (B2x - B1x);

		double	Bb = B1y - (Bm * B1x);

		//	A intersecting B at A1x, Yi.
		//
		double	Yi = (Bm * A1x) + Bb;

		//	Not on correct side of A1?
		//
		if ( ((A2y - A1y) > 0) && ((Yi - A1y) < 0) )  return FALSE;
		if ( ((A2y - A1y) < 0) && ((Yi - A1y) > 0) )  return FALSE;

		return TRUE;
	}

	//	B vertical?
	//
	if ( fabs ( B1x - B2x ) < 0.0000001 )
	{
		//	A's slope and y intercept.
		//
		double	Am = (A2y - A1y) / (A2x - A1x);

		double	Ab = A1y - (Am * A1x);

		//	B intersecting A at B1x, Yi.
		//
		double	Yi = (Am * B1x) + Ab;

		//	Not within B?
		//
		if ( B1y < B2y )
		{
			if ( (Yi < B1y) || (Yi > B2y) )  return FALSE;
		}
		else
			if ( (Yi < B2y) || (Yi > B1y) )  return FALSE;

		//	Not on correct side of A1?
		//
		if ( (A2x < A1x) && (B1x > A1x) )  return FALSE;
		if ( (A2x > A1x) && (B1x < A1x) )  return FALSE;

		return TRUE;
	}


	//	A's slope and y intercept.
	//
	double	Am = (A2y - A1y) / (A2x - A1x);

	double	Ab = A1y - (Am * A1x);

	//	B's slope and y intercept.
	//
	double	Bm = (B2y - B1y) / (B2x - B1x);

	double	Bb = B1y - (Bm * B1x);

	//	Intersection at Xi, Yi.
	//
	double	Xi = (Bb - Ab) / ( Am - Bm);

	//	Not within B?
	//
	if ( B1x < B2x )
	{
		if ( (Xi < B1x) || (Xi > B2x) )  return FALSE;
	}
	else
		if ( (Xi < B2x) || (Xi > B1x) )  return FALSE;


	double	Yi = (Am * Xi) + Ab;

	//	Not on correct side of A1?
	//
	if ( ((A2x - A1x) > 0) && ((Xi - A1x) < 0) )  return FALSE;
	if ( ((A2x - A1x) < 0) && ((Xi - A1x) > 0) )  return FALSE;

	if ( ((A2y - A1y) > 0) && ((Yi - A1y) < 0) )  return FALSE;
	if ( ((A2y - A1y) < 0) && ((Yi - A1y) > 0) )  return FALSE;

	return TRUE;

}	//	LineIntersects()


PORT BOOL	PolysOverlap ( const dVector &	AVa, 
						   const dVector &	AVb, 
						   const dVector &	AVc, 
						   const dVector &	BVa, 
						   const dVector &	BVb, 
						   const dVector &	BVc,
								 double		Close )
{
	//	Assumes the polygons are in the same plane.

	//	Make it a 2D problem by working in the coordinate plane where
	//	the triangles' profiles are the greatest.
	//
	//	Find the area of the triangles in each of the three X Y Z coordinate
	//	planes.  Use the plane where the triangles have the greatest area.
	//
	double	Axy =   TriArea2D ( AVa.X, AVa.Y, AVb.X, AVb.Y, AVc.X, AVc.Y )
				  + TriArea2D ( BVa.X, BVa.Y, BVb.X, BVb.Y, BVc.X, BVc.Y );

	double	Ayz =   TriArea2D ( AVa.Y, AVa.Z, AVb.Y, AVb.Z, AVc.Y, AVc.Z )
				  + TriArea2D ( BVa.Y, BVa.Z, BVb.Y, BVb.Z, BVc.Y, BVc.Z );

	double	Azx =   TriArea2D ( AVa.Z, AVa.X, AVb.Z, AVb.X, AVc.Z, AVc.X )
				  + TriArea2D ( BVa.Z, BVa.X, BVb.Z, BVb.X, BVc.Z, BVc.X );


	//	float has FLT_DIG digits of precision.  See float.h.

	double	Prec = 1.0 / pow ( 10.0, (int)(FLT_DIG - 1) );

	if ( (Axy < Prec) && (Ayz < Prec) && (Azx < Prec) )
	{
		//	If all the projected areas are so small then...
		//
		return FALSE;
	}

	dVector	AMin ( DBL_MAX, DBL_MAX, DBL_MAX );
	dVector	AMax ( DBL_MIN, DBL_MIN, DBL_MIN );
	dVector	BMin ( DBL_MAX, DBL_MAX, DBL_MAX );
	dVector	BMax ( DBL_MIN, DBL_MIN, DBL_MIN );

	SetMinMax ( AVa, AMin, AMax );	SetMinMax ( BVa, BMin, BMax );
	SetMinMax ( AVb, AMin, AMax );	SetMinMax ( BVb, BMin, BMax );
	SetMinMax ( AVc, AMin, AMax );	SetMinMax ( BVc, BMin, BMax );

	double	Ix, Iy;

	if ( Axy > Ayz )
	{
		if ( Axy <= Azx )
		{
			//	Work in Z, X.
			//
			goto PolysIntersect_InZX;
		}

		//	Work in X, Y;
		//
		//	If either end of A is in B or either end of B is in A...
		//	(in both dimensions)
		//
	//	This misses cases where the two polys are directly on top of each
	//	other.
	//	if (    (   ((AMin.X - BMin.X > Close) && (BMax.X - AMin.X > Close))
	//		     || ((AMax.X - BMin.X > Close) && (BMax.X - AMax.X > Close))
	//		     || ((BMin.X - AMin.X > Close) && (AMax.X - BMin.X > Close))
	//		     || ((BMax.X - AMin.X > Close) && (AMax.X - BMax.X > Close)))
	//		 && (   ((AMin.Y - BMin.Y > Close) && (BMax.Y - AMin.Y > Close))
	//			 || ((AMax.Y - BMin.Y > Close) && (BMax.Y - AMax.Y > Close))
	//			 || ((BMin.Y - AMin.Y > Close) && (AMax.Y - BMin.Y > Close))
	//			 || ((BMax.Y - AMin.Y > Close) && (AMax.Y - BMax.Y > Close))) )
	//	{
			//	Not enough.
			//
			//	Got to test for edge intersections.

			//	Test each of the three edges of one triangle against each of
			//	the three of the other.
			//
			if ( IntersectPoint ( AVa.X, AVa.Y, AVb.X, AVb.Y,
								  BVa.X, BVa.Y, BVb.X, BVb.Y, Ix, Iy ) )
			{
				if (    Inside ( AVa.X, AVb.X, Ix, Close )
					 && Inside ( AVa.Y, AVb.Y, Iy, Close )
					 && Inside ( BVa.X, BVb.X, Ix, Close )
					 && Inside ( BVa.Y, BVb.Y, Iy, Close ) )  return TRUE;
			}

			if ( IntersectPoint ( AVa.X, AVa.Y, AVb.X, AVb.Y,
								  BVb.X, BVb.Y, BVc.X, BVc.Y, Ix, Iy ) )
			{
				if (    Inside ( AVa.X, AVb.X, Ix, Close )
					 && Inside ( AVa.Y, AVb.Y, Iy, Close )
					 && Inside ( BVb.X, BVc.X, Ix, Close )
					 && Inside ( BVb.Y, BVc.Y, Iy, Close ) )  return TRUE;
			}

			if ( IntersectPoint ( AVa.X, AVa.Y, AVb.X, AVb.Y,
								  BVc.X, BVc.Y, BVa.X, BVa.Y, Ix, Iy ) )
			{
				if (    Inside ( AVa.X, AVb.X, Ix, Close )
					 && Inside ( AVa.Y, AVb.Y, Iy, Close )
					 && Inside ( BVc.X, BVa.X, Ix, Close )
					 && Inside ( BVc.Y, BVa.Y, Iy, Close ) )  return TRUE;
			}



			if ( IntersectPoint ( AVb.X, AVb.Y, AVc.X, AVc.Y,
								  BVa.X, BVa.Y, BVb.X, BVb.Y, Ix, Iy ) )
			{
				if (    Inside ( AVb.X, AVc.X, Ix, Close )
					 && Inside ( AVb.Y, AVc.Y, Iy, Close )
					 && Inside ( BVa.X, BVb.X, Ix, Close )
					 && Inside ( BVa.Y, BVb.Y, Iy, Close ) )  return TRUE;
			}

			if ( IntersectPoint ( AVb.X, AVb.Y, AVc.X, AVc.Y,
								  BVb.X, BVb.Y, BVc.X, BVc.Y, Ix, Iy ) )
			{
				if (    Inside ( AVb.X, AVc.X, Ix, Close )
					 && Inside ( AVb.Y, AVc.Y, Iy, Close )
					 && Inside ( BVb.X, BVc.X, Ix, Close )
					 && Inside ( BVb.Y, BVc.Y, Iy, Close ) )  return TRUE;
			}

			if ( IntersectPoint ( AVb.X, AVb.Y, AVc.X, AVc.Y,
								  BVc.X, BVc.Y, BVa.X, BVa.Y, Ix, Iy ) )
			{
				if (    Inside ( AVb.X, AVc.X, Ix, Close )
					 && Inside ( AVb.Y, AVc.Y, Iy, Close )
					 && Inside ( BVc.X, BVa.X, Ix, Close )
					 && Inside ( BVc.Y, BVa.Y, Iy, Close ) )  return TRUE;
			}



			if ( IntersectPoint ( AVc.X, AVc.Y, AVa.X, AVa.Y,
								  BVa.X, BVa.Y, BVb.X, BVb.Y, Ix, Iy ) )
			{
				if (    Inside ( AVc.X, AVa.X, Ix, Close )
					 && Inside ( AVc.Y, AVa.Y, Iy, Close )
					 && Inside ( BVa.X, BVb.X, Ix, Close )
					 && Inside ( BVa.Y, BVb.Y, Iy, Close ) )  return TRUE;
			}

			if ( IntersectPoint ( AVc.X, AVc.Y, AVa.X, AVa.Y,
								  BVb.X, BVb.Y, BVc.X, BVc.Y, Ix, Iy ) )
			{
				if (    Inside ( AVc.X, AVa.X, Ix, Close )
					 && Inside ( AVc.Y, AVa.Y, Iy, Close )
					 && Inside ( BVb.X, BVc.X, Ix, Close )
					 && Inside ( BVb.Y, BVc.Y, Iy, Close ) )  return TRUE;
			}

			if ( IntersectPoint ( AVc.X, AVc.Y, AVa.X, AVa.Y,
								  BVc.X, BVc.Y, BVa.X, BVa.Y, Ix, Iy ) )
			{
				if (    Inside ( AVc.X, AVa.X, Ix, Close )
					 && Inside ( AVc.Y, AVa.Y, Iy, Close )
					 && Inside ( BVc.X, BVa.X, Ix, Close )
					 && Inside ( BVc.Y, BVa.Y, Iy, Close ) )  return TRUE;
			}

			//	It may be that one triangle is completely inside the other 
			//	so that no edges intersect, or that one lies exactly on the 
			//	other so that all edges are colinear, etc..
			//
			dVector	ACtr;	ACtr = TriCenter ( AVa, AVb, AVc );

			if ( InsideTriangle ( 1, 1.0, ACtr, BVa, BVb, BVc ) ) return TRUE;

			dVector	BCtr;	BCtr = TriCenter ( BVa, BVb, BVc );

			if ( InsideTriangle ( 1, 1.0, BCtr, AVa, AVb, AVc ) ) return TRUE;
	//	}

		return FALSE;
	}

	if ( Ayz > Azx )
	{
		//	Work in Y, Z.
		//
	//	if (    (   ((AMin.Y - BMin.Y > Close) && (BMax.Y - AMin.Y > Close))
	//		     || ((AMax.Y - BMin.Y > Close) && (BMax.Y - AMax.Y > Close))
	//		     || ((BMin.Y - AMin.Y > Close) && (AMax.Y - BMin.Y > Close))
	//		     || ((BMax.Y - AMin.Y > Close) && (AMax.Y - BMax.Y > Close)))
	//		 && (   ((AMin.Z - BMin.Z > Close) && (BMax.Z - AMin.Z > Close))
	//			 || ((AMax.Z - BMin.Z > Close) && (BMax.Z - AMax.Z > Close))
	//			 || ((BMin.Z - AMin.Z > Close) && (AMax.Z - BMin.Z > Close))
	//			 || ((BMax.Z - AMin.Z > Close) && (AMax.Z - BMax.Z > Close))) )
	//	{
			//	Not enough.
			//
			//	Got to test for edge intersections.

			//	Test each of the three edges of one triangle against each of
			//	the three of the other.
			//
			if ( IntersectPoint ( AVa.Y, AVa.Z, AVb.Y, AVb.Z,
								  BVa.Y, BVa.Z, BVb.Y, BVb.Z, Ix, Iy ) )
			{
				if (    Inside ( AVa.Y, AVb.Y, Ix, Close )
					 && Inside ( AVa.Z, AVb.Z, Iy, Close )
					 && Inside ( BVa.Y, BVb.Y, Ix, Close )
					 && Inside ( BVa.Z, BVb.Z, Iy, Close ) )  return TRUE;
			}

			if ( IntersectPoint ( AVa.Y, AVa.Z, AVb.Y, AVb.Z,
								  BVb.Y, BVb.Z, BVc.Y, BVc.Z, Ix, Iy ) )
			{
				if (    Inside ( AVa.Y, AVb.Y, Ix, Close )
					 && Inside ( AVa.Z, AVb.Z, Iy, Close )
					 && Inside ( BVb.Y, BVc.Y, Ix, Close )
					 && Inside ( BVb.Z, BVc.Z, Iy, Close ) )  return TRUE;
			}

			if ( IntersectPoint ( AVa.Y, AVa.Z, AVb.Y, AVb.Z,
								  BVc.Y, BVc.Z, BVa.Y, BVa.Z, Ix, Iy ) )
			{
				if (    Inside ( AVa.Y, AVb.Y, Ix, Close )
					 && Inside ( AVa.Z, AVb.Z, Iy, Close )
					 && Inside ( BVc.Y, BVa.Y, Ix, Close )
					 && Inside ( BVc.Z, BVa.Z, Iy, Close ) )  return TRUE;
			}



			if ( IntersectPoint ( AVb.Y, AVb.Z, AVc.Y, AVc.Z,
								  BVa.Y, BVa.Z, BVb.Y, BVb.Z, Ix, Iy ) )
			{
				if (    Inside ( AVb.Y, AVc.Y, Ix, Close )
					 && Inside ( AVb.Z, AVc.Z, Iy, Close )
					 && Inside ( BVa.Y, BVb.Y, Ix, Close )
					 && Inside ( BVa.Z, BVb.Z, Iy, Close ) )  return TRUE;
			}

			if ( IntersectPoint ( AVb.Y, AVb.Z, AVc.Y, AVc.Z,
								  BVb.Y, BVb.Z, BVc.Y, BVc.Z, Ix, Iy ) )
			{
				if (    Inside ( AVb.Y, AVc.Y, Ix, Close )
					 && Inside ( AVb.Z, AVc.Z, Iy, Close )
					 && Inside ( BVb.Y, BVc.Y, Ix, Close )
					 && Inside ( BVb.Z, BVc.Z, Iy, Close ) )  return TRUE;
			}

			if ( IntersectPoint ( AVb.Y, AVb.Z, AVc.Y, AVc.Z,
								  BVc.Y, BVc.Z, BVa.Y, BVa.Z, Ix, Iy ) )
			{
				if (    Inside ( AVb.Y, AVc.Y, Ix, Close )
					 && Inside ( AVb.Z, AVc.Z, Iy, Close )
					 && Inside ( BVc.Y, BVa.Y, Ix, Close )
					 && Inside ( BVc.Z, BVa.Z, Iy, Close ) )  return TRUE;
			}



			if ( IntersectPoint ( AVc.Y, AVc.Z, AVa.Y, AVa.Z,
								  BVa.Y, BVa.Z, BVb.Y, BVb.Z, Ix, Iy ) )
			{
				if (    Inside ( AVc.Y, AVa.Y, Ix, Close )
					 && Inside ( AVc.Z, AVa.Z, Iy, Close )
					 && Inside ( BVa.Y, BVb.Y, Ix, Close )
					 && Inside ( BVa.Z, BVb.Z, Iy, Close ) )  return TRUE;
			}

			if ( IntersectPoint ( AVc.Y, AVc.Z, AVa.Y, AVa.Z,
								  BVb.Y, BVb.Z, BVc.Y, BVc.Z, Ix, Iy ) )
			{
				if (    Inside ( AVc.Y, AVa.Y, Ix, Close )
					 && Inside ( AVc.Z, AVa.Z, Iy, Close )
					 && Inside ( BVb.Y, BVc.Y, Ix, Close )
					 && Inside ( BVb.Z, BVc.Z, Iy, Close ) )  return TRUE;
			}

			if ( IntersectPoint ( AVc.Y, AVc.Z, AVa.Y, AVa.Z,
								  BVc.Y, BVc.Z, BVa.Y, BVa.Z, Ix, Iy ) )
			{
				if (    Inside ( AVc.Y, AVa.Y, Ix, Close )
					 && Inside ( AVc.Z, AVa.Z, Iy, Close )
					 && Inside ( BVc.Y, BVa.Y, Ix, Close )
					 && Inside ( BVc.Z, BVa.Z, Iy, Close ) )  return TRUE;
			}


			dVector	ACtr;	ACtr = TriCenter ( AVa, AVb, AVc );

			if ( InsideTriangle ( 3, 1.0, ACtr, BVa, BVb, BVc ) ) return TRUE;

			dVector	BCtr;	BCtr = TriCenter ( BVa, BVb, BVc );

			if ( InsideTriangle ( 3, 1.0, BCtr, AVa, AVb, AVc ) ) return TRUE;
	//	}

		return FALSE;
	}

PolysIntersect_InZX:

	//	Work in Z, X.
	//
//	if (    (   ((AMin.Z - BMin.Z > Close) && (BMax.Z - AMin.Z > Close))
//			 || ((AMax.Z - BMin.Z > Close) && (BMax.Z - AMax.Z > Close))
//			 || ((BMin.Z - AMin.Z > Close) && (AMax.Z - BMin.Z > Close))
//			 || ((BMax.Z - AMin.Z > Close) && (AMax.Z - BMax.Z > Close)))
//		 && (   ((AMin.X - BMin.X > Close) && (BMax.X - AMin.X > Close))
//			 || ((AMax.X - BMin.X > Close) && (BMax.X - AMax.X > Close))
//			 || ((BMin.X - AMin.X > Close) && (AMax.X - BMin.X > Close))
//			 || ((BMax.X - AMin.X > Close) && (AMax.X - BMax.X > Close))) )
//	{
		//	Not enough.
		//
		//	Got to test for edge intersections.

		//	Test each of the three edges of one triangle against each of
		//	the three of the other.
		//
		if ( IntersectPoint ( AVa.Z, AVa.X, AVb.Z, AVb.X,
							  BVa.Z, BVa.X, BVb.Z, BVb.X, Ix, Iy ) )
		{
			if (    Inside ( AVa.Z, AVb.Z, Ix, Close )
				 && Inside ( AVa.X, AVb.X, Iy, Close )
				 && Inside ( BVa.Z, BVb.Z, Ix, Close )
				 && Inside ( BVa.X, BVb.X, Iy, Close ) )  return TRUE;
		}

		if ( IntersectPoint ( AVa.Z, AVa.X, AVb.Z, AVb.X,
							  BVb.Z, BVb.X, BVc.Z, BVc.X, Ix, Iy ) )
		{
			if (    Inside ( AVa.Z, AVb.Z, Ix, Close )
				 && Inside ( AVa.X, AVb.X, Iy, Close )
				 && Inside ( BVb.Z, BVc.Z, Ix, Close )
				 && Inside ( BVb.X, BVc.X, Iy, Close ) )  return TRUE;
		}

		if ( IntersectPoint ( AVa.Z, AVa.X, AVb.Z, AVb.X,
							  BVc.Z, BVc.X, BVa.Z, BVa.X, Ix, Iy ) )
		{
			if (    Inside ( AVa.Z, AVb.Z, Ix, Close )
				 && Inside ( AVa.X, AVb.X, Iy, Close )
				 && Inside ( BVc.Z, BVa.Z, Ix, Close )
				 && Inside ( BVc.X, BVa.X, Iy, Close ) )  return TRUE;
		}



		if ( IntersectPoint ( AVb.Z, AVb.X, AVc.Z, AVc.X,
							  BVa.Z, BVa.X, BVb.Z, BVb.X, Ix, Iy ) )
		{
			if (    Inside ( AVb.Z, AVc.Z, Ix, Close )
				 && Inside ( AVb.X, AVc.X, Iy, Close )
				 && Inside ( BVa.Z, BVb.Z, Ix, Close )
				 && Inside ( BVa.X, BVb.X, Iy, Close ) )  return TRUE;
		}

		if ( IntersectPoint ( AVb.Z, AVb.X, AVc.Z, AVc.X,
							  BVb.Z, BVb.X, BVc.Z, BVc.X, Ix, Iy ) )
		{
			if (    Inside ( AVb.Z, AVc.Z, Ix, Close )
				 && Inside ( AVb.X, AVc.X, Iy, Close )
				 && Inside ( BVb.Z, BVc.Z, Ix, Close )
				 && Inside ( BVb.X, BVc.X, Iy, Close ) )  return TRUE;
		}

		if ( IntersectPoint ( AVb.Z, AVb.X, AVc.Z, AVc.X,
							  BVc.Z, BVc.X, BVa.Z, BVa.X, Ix, Iy ) )
		{
			if (    Inside ( AVb.Z, AVc.Z, Ix, Close )
				 && Inside ( AVb.X, AVc.X, Iy, Close )
				 && Inside ( BVc.Z, BVa.Z, Ix, Close )
				 && Inside ( BVc.X, BVa.X, Iy, Close ) )  return TRUE;
		}



		if ( IntersectPoint ( AVc.Z, AVc.X, AVa.Z, AVa.X,
							  BVa.Z, BVa.X, BVb.Z, BVb.X, Ix, Iy ) )
		{
			if (    Inside ( AVc.Z, AVa.Z, Ix, Close )
				 && Inside ( AVc.X, AVa.X, Iy, Close )
				 && Inside ( BVa.Z, BVb.Z, Ix, Close )
				 && Inside ( BVa.X, BVb.X, Iy, Close ) )  return TRUE;
		}

		if ( IntersectPoint ( AVc.Z, AVc.X, AVa.Z, AVa.X,
							  BVb.Z, BVb.X, BVc.Z, BVc.X, Ix, Iy ) )
		{
			if (    Inside ( AVc.Z, AVa.Z, Ix, Close )
				 && Inside ( AVc.X, AVa.X, Iy, Close )
				 && Inside ( BVb.Z, BVc.Z, Ix, Close )
				 && Inside ( BVb.X, BVc.X, Iy, Close ) )  return TRUE;
		}

		if ( IntersectPoint ( AVc.Z, AVc.X, AVa.Z, AVa.X,
							  BVc.Z, BVc.X, BVa.Z, BVa.X, Ix, Iy ) )
		{
			if (    Inside ( AVc.Z, AVa.Z, Ix, Close )
				 && Inside ( AVc.X, AVa.X, Iy, Close )
				 && Inside ( BVc.Z, BVa.Z, Ix, Close )
				 && Inside ( BVc.X, BVa.X, Iy, Close ) )  return TRUE;
		}
//	}


	dVector	ACtr;	ACtr = TriCenter ( AVa, AVb, AVc );

	if ( InsideTriangle ( 2, 1.0, ACtr, BVa, BVb, BVc ) ) return TRUE;

	dVector	BCtr;	BCtr = TriCenter ( BVa, BVb, BVc );

	if ( InsideTriangle ( 2, 1.0, BCtr, AVa, AVb, AVc ) ) return TRUE;

	return FALSE;

}	//	PolysOverlap()


PORT double	Mag ( const dVector & V )
{
	return sqrt ( (V.X * V.X) + (V.Y * V.Y) + (V.Z * V.Z) );
}


PORT float	Mag ( const fVertex & V )
{
	return (float)sqrt ( (V.X * V.X) + (V.Y * V.Y) + (V.Z * V.Z) );
}


//	Intersection of a plane and line.
//
PORT BOOL	GetIntersection ( double A, double B, double C, double D,
							  const dVector * pPt1, 
							  const dVector * pPt2, 
									dVector * pI )
{
	//	A, B, C, and D represent the plane.
	//
	//	pPt1 and pPt2 are on the line.

	double	dx = pPt2->X - pPt1->X;
	double	dy = pPt2->Y - pPt1->Y;
	double	dz = pPt2->Z - pPt1->Z;

	//	A B C represent a normalized vector normal to the plane.
	//
	//	First see if the line is parallel with the plane.
	//
	dVector	V1 (  A,  B,  C );		NormalizeVector ( &V1 );
	dVector	V2 ( dx, dy, dz );		NormalizeVector ( &V2 );

	if ( fabs ( DotProduct ( V1, V2 ) ) < 0.000001 )  return FALSE;


	//	Not parallel.

	double	e, f, g, h;

	if ( fabs ( dx ) > 0.000001 )
	{
		e = dy / dx;

		f = -(pPt1->X * e) + pPt1->Y;

		g = dz / dx;

		h = -(pPt1->X * g) + pPt1->Z;

		pI->X = -((f * B) + (h * C) + D) / (A + (e * B) + (g * C));

		pI->Y = (pI->X * e) + f;

		pI->Z = (pI->X * g) + h;

		return TRUE;
	}

	if ( fabs ( dy ) > 0.000001 )
	{
		e = dx / dy;

		f = -(pPt1->Y * e) + pPt1->X;

		g = dz / dy;

		h = -(pPt1->Y * g) + pPt1->Z;

		pI->Y = -((f * A) + (h * C) + D) / ((e * A) + B + (g * C));

		pI->X = (pI->Y * e) + f;

		pI->Z = (pI->Y * g) + h;

		return TRUE;
	}

	if ( fabs ( dz ) > 0.000001 )
	{
		e = dx / dz;

		f = -(pPt1->Z * e) + pPt1->X;

		g = dy / dz;

		h = -(pPt1->Z * g) + pPt1->Y;

		pI->Z = -((f * A) + (h * B) + D) / ((e * A) + (g * B) + C);

		pI->X = (pI->Z * e) + f;

		pI->Y = (pI->Z * g) + h;

		return TRUE;
	}

	return FALSE;
}


//	Normal of a plane.  
//
void PORT	CalcNormal ( dVector * pPt1, dVector * pPt2, dVector * pPt3, 
						 dVector * pN )
{
	//	Form two vectors from the points on the plane.
	//
	double a[3], b[3];

	a[0] = pPt2->X - pPt1->X;
	a[1] = pPt2->Y - pPt1->Y;
	a[2] = pPt2->Z - pPt1->Z;
	b[0] = pPt3->X - pPt1->X;
	b[1] = pPt3->Y - pPt1->Y;
	b[2] = pPt3->Z - pPt1->Z;

	//	Calculate the cross product of the two vectors.
	//
	pN->X = (a[1] * b[2]) - (a[2] * b[1]);
	pN->Y = (a[2] * b[0]) - (a[0] * b[2]);
	pN->Z = (a[0] * b[1]) - (a[1] * b[0]);

	//	Normalize the new vector.
	//
	NormalizeVector ( pN );

}	//	CalcNormal()


void PORT	CalcNormal ( fVertex * pV1, fVertex * pV2, fVertex * pV3, 
						 float * n )
{
	//	Form two vectors from the points.
	//
	float a[3], b[3];
	/*
	a[0] = p2[0] - p1[0];
	a[1] = p2[1] - p1[1];
	a[2] = p2[2] - p1[2];
	b[0] = p3[0] - p1[0];
	b[1] = p3[1] - p1[1];
	b[2] = p3[2] - p1[2];
	*/
	a[0] = pV2->X - pV1->X;
	a[1] = pV2->Y - pV1->Y;
	a[2] = pV2->Z - pV1->Z;
	b[0] = pV3->X - pV1->X;
	b[1] = pV3->Y - pV1->Y;
	b[2] = pV3->Z - pV1->Z;

	//	Calculate the cross product of the two vectors.
	//
	n[0] = (a[1] * b[2]) - (a[2] * b[1]);
	n[1] = (a[2] * b[0]) - (a[0] * b[2]);
	n[2] = (a[0] * b[1]) - (a[1] * b[0]);

	//	Normalize the new vector.
	//
	/*
	float length = (float)sqrt (   (n[0] * n[0]) 
								 + (n[1] * n[1]) 
								 + (n[2] * n[2]) );
	if ( length != 0 )
	{
		n[0] = n[0] / length;
		n[1] = n[1] / length;
		n[2] = n[2] / length;
	}
	*/
	NormalizeVector ( (fVertex *)n );

}	//	CalcNormal()


void PORT	CalcNormal ( float * pV1, float * pV2, float * pV3, float * n )
{
	//	Form two vectors from the points.
	//
	float a[3], b[3];

	a[0] = pV2[0] - pV1[0];
	a[1] = pV2[1] - pV1[1];
	a[2] = pV2[2] - pV1[2];
	b[0] = pV3[0] - pV1[0];
	b[1] = pV3[1] - pV1[1];
	b[2] = pV3[2] - pV1[2];

	//	Calculate the cross product of the two vectors.
	//
	n[0] = (a[1] * b[2]) - (a[2] * b[1]);
	n[1] = (a[2] * b[0]) - (a[0] * b[2]);
	n[2] = (a[0] * b[1]) - (a[1] * b[0]);

	//	Normalize the new vector.
	//
	/*
	float length = (float)sqrt (   (n[0] * n[0]) 
								 + (n[1] * n[1]) 
								 + (n[2] * n[2]) );
	if ( length != 0 )
	{
		n[0] = n[0] / length;
		n[1] = n[1] / length;
		n[2] = n[2] / length;
	}
	*/
	NormalizeVector ( (fVertex *)n );

}	//	CalcNormal()


//	Normal to a line in 2D space.
//
void PORT	CalcNormal2D ( float   Xs, float   Ys, 
						   float   Xe, float   Ye,
						   float & Xn, float & Yn )
{
	//	One vector is the line in the X Y plane.
	//
	//	Imagine 3D space, another vector pointing +Z.
	//
	Xn = Ye - Ys;
	Yn = Xs - Xe;

	//	Normalize the new vector.
	//
	NormalizeVector2D ( Xn, Yn );

}	//	CalcNormal2D()


/*
void PORT	DiscXY ( double A, double B, fVertex * pV, int T4 )
{
#define	pi	3.141592653589793
	
	int		i, j;

	for ( i = 0; i < T4; i++ )
	{
		double	t = ((double)i * pi) / (2.0 * (double)T4);
		double	x = A * cos ( t );
		double	y = B * sin ( t );

		if ( i == 0 )
		{
			j = 0;			pV[j].X = (float) x;	pV[j].Y = (float) y;
			j = 2 * T4;		pV[j].X = (float)-x;	pV[j].Y = (float) y;
		}
		else
		{
			j = i;				pV[j].X = (float) x;	pV[j].Y = (float) y;
			j = (2 * T4) - i;	pV[j].X = (float)-x;	pV[j].Y = (float) y;
			j = (2 * T4) + i;	pV[j].X = (float)-x;	pV[j].Y = (float)-y;
			j = (4 * T4) - i;	pV[j].X = (float) x;	pV[j].Y = (float)-y;
		}
	}

	j = T4;			pV[j].X = 0.0f;	pV[j].Y = (float) B;
	j = 3 * T4;		pV[j].X = 0.0f;	pV[j].Y = (float)-B;

#undef	pi
}	//	DiscXY()
*/


dVector PORT	NormalizeVector ( const dVector & V )
{
	dVector	N ( V );

	NormalizeVector ( &N );

	return N;

}	//	NormalizeVector()


void PORT	NormalizeVector ( dVector * pN )
{
	double R = sqrt ( (pN->X * pN->X) + (pN->Y * pN->Y) + (pN->Z * pN->Z) );

	if ( R != 0 )
	{
		pN->X = pN->X / R;
		pN->Y = pN->Y / R;
		pN->Z = pN->Z / R;
	}

}	//	NormalizeVector()


void PORT	NormalizeVector ( fVertex * pN )
{
	float	R = (float)sqrt (   (pN->X * pN->X) 
							  + (pN->Y * pN->Y) 
							  + (pN->Z * pN->Z) );

	if ( R != 0 )
	{
		pN->X = pN->X / R;
		pN->Y = pN->Y / R;
		pN->Z = pN->Z / R;
	}

}	//	NormalizeVector()


void PORT	NormalizeVector2D ( float & Xn, float & Yn )
{
	float	R = (float)sqrt (   (Xn * Xn) 
							  + (Yn * Yn) );

	if ( R != 0 )
	{
		Xn = Xn / R;
		Yn = Yn / R;
	}

}	//	NormalizeVector()


int PORT	CheckNormal ( dVector * pN, double Close /* = 0.0 */ )
{
	double	R2 = (pN->X * pN->X) + (pN->Y * pN->Y) + (pN->Z * pN->Z);

	if ( Close != 0.0 )
	{
		if ( fabs ( R2 - 1.0f ) > Close )  return -1;
	}
	else
		if ( fabs ( R2 - 1.0 ) > 0.0000000001 )  return -1;

	return 0;

}	//	CheckNormal()


int PORT	CheckNormal ( fVertex * pN, float Close /* = 0.0f */ )
{
	float	R2 = (pN->X * pN->X) + (pN->Y * pN->Y) + (pN->Z * pN->Z);

	if ( Close != 0.0f )
	{
		if ( fabs ( R2 - 1.0f ) > Close )  return -1;
	}
	else
		if ( fabs ( R2 - 1.0f ) > 0.0000001 )  return -1;

	return 0;

}	//	CheckNormal()


double PORT	DotProduct ( const dVector & V1, const dVector & V2 )
{
	return (V1.X * V2.X) + (V1.Y * V2.Y) + (V1.Z * V2.Z);

}	//	DotProduct()



/*----------------------------  CrossProduct()  ----------------------------*/
/*
	Make two vectors, A and B, out of the three vertices.  V2 common 
	starting point of the two vectors.  Set N to the cross product of
	those two vectors.

	From p293 of Handbook of Mathmatical Formulas, ..., for Mathmaticians,
	Scientists, Engineers.

	Same as the CalcNormal() functions without the normalization.
*/
void PORT	CrossProduct ( const dVector & V1, 
						   const dVector & V2, 
						   const dVector & V3, dVector & N )
{
	dVector	A = V1 - V2;
	dVector	B = V3 - V2;

	N.X = (A.Y * B.Z) - (A.Z * B.Y);
	N.Y = (A.Z * B.X) - (A.X * B.Z);
	N.Z = (A.X * B.Y) - (A.Y * B.X);

}	//	CrossProduct()


/*----------------------------  CrossProduct()  ----------------------------*/
/*
*/
dVector PORT	CrossProduct ( const dVector & A, const dVector & B )
{
	dVector	CP;

	CP.X = (A.Y * B.Z) - (A.Z * B.Y);
	CP.Y = (A.Z * B.X) - (A.X * B.Z);
	CP.Z = (A.X * B.Y) - (A.Y * B.X);

	return CP;

}	//	CrossProduct()


/*-----------------------------  IsParallel()  -----------------------------*/
/*
*/
BOOL PORT	IsParallel ( const dVector & A, const dVector & B )
{
	//	2009-Oct-05		A and B probably should be normalized ...

	double	dp = fabs ( DotProduct ( A, B ) );

	return fabs ( dp - 1.0 ) < 0.0001;

}	//	IsParallel()


/*-----------------------------  IsParallel()  -----------------------------*/
/*
*/
BOOL PORT	IsParallel ( const dVector & A, const dVector & B, double Close )
{
	//	2009-Oct-05

	dVector	AN ( A );		NormalizeVector ( &AN );
	dVector	BN ( B );		NormalizeVector ( &BN );

	double	dp = fabs ( DotProduct ( AN, BN ) );

	return fabs ( dp - 1.0 ) < Close;

}	//	IsParallel()




/*----------------------------  XYZToSphere()  -----------------------------*/
/*
*/
void PORT	XYZToSphere ( double   X, double   Y, double   Z,
						  double & R, double & T, double & P )
{
	R = sqrt ( (X * X) + (Y * Y) + (Z * Z) );

	if ( R == 0.0 ) 
	{
		T = 0.0;	P = 0.0;

		return;
	}

	T = acos ( Y / R );

	P = atan2 ( X, Z );

}	//	XYZToSphere()


/*----------------------------  SphereToXYZ()  -----------------------------*/
/*
*/
void PORT	SphereToXYZ ( double   R, double   T, double   P,
						  double & X, double & Y, double & Z )
{
	X = R * sin ( T ) * sin ( P );

	Y = R * cos ( T );

	Z = R * sin ( T ) * cos ( P );

}	//	SphereToXYZ()


/*---------------------------------  Dist()  -------------------------------*/
/*
	Return distance between points Va and Vb.
*/
double PORT	Dist ( const dVector & PtA, const dVector & PtB )
{
	double	dX = PtA.X - PtB.X;
	double	dY = PtA.Y - PtB.Y;
	double	dZ = PtA.Z - PtB.Z;

	return sqrt ( (dX * dX) + (dY * dY) + (dZ * dZ) );

}	//	Dist()


/*---------------------------------  Dist()  -------------------------------*/
/*
	Return distance between points Va and Vb.
*/
float PORT	Dist ( const fVertex & Va, const fVertex & Vb )
{
	double	dX = Va.X - Vb.X;
	double	dY = Va.Y - Vb.Y;
	double	dZ = Va.Z - Vb.Z;

	return (float)sqrt ( (dX * dX) + (dY * dY) + (dZ * dZ) );

}	//	Dist()


/*---------------------------------  Dist()  -------------------------------*/
/*
*/
double PORT	Dist ( double aX, double aY, double aZ,
				   double bX, double bY, double bZ )
{
	double	dX = aX - bX;
	double	dY = aY - bY;
	double	dZ = aZ - bZ;

	return sqrt ( (dX * dX) + (dY * dY) + (dZ * dZ) );

}	//	Dist()


/*--------------------------------  Dist2D()  ------------------------------*/
/*
	Return 2 dimensional distance between points.
*/
float PORT	Dist2D ( float Xs, float Ys, float Xe, float Ye )
{
	double	dX = Xe - Xs;
	double	dY = Ye - Ys;

	return (float)sqrt ( (dX * dX) + (dY * dY) );

}	//	Dist2D()


/*--------------------------------  Dist2D()  ------------------------------*/
/*
	Return 2 dimensional distance between points.
*/
double PORT	Dist2D ( double Xs, double Ys, double Xe, double Ye )
{
	double	dX = Xe - Xs;
	double	dY = Ye - Ys;

	return sqrt ( (dX * dX) + (dY * dY) );

}	//	Dist2D()


/*------------------------------  DistPt2Line()  ---------------------------*/
/*
	Return shortest distance from point to line.

	HWN 2000-Jan-20 pgs 1, 2.

		Pt1, Pt2	Points defining the line.

		PtA			The point from which we find the shortest distance to 
					the line.
*/
double PORT	DistPt2Line ( const dVector & Pt1, 
						  const dVector & Pt2,
						  const dVector & PtA )
{
	double	dX = Pt2.X - Pt1.X;
	double	dY = Pt2.Y - Pt1.Y;
	double	dZ = Pt2.Z - Pt1.Z;

	double	d  = sqrt ( (dX * dX) + (dY * dY) + (dZ * dZ) );

	double	l  = dX / d;
	double	m  = dY / d;
	double	n  = dZ / d;

			dX = PtA.X - Pt1.X;
			dY = PtA.Y - Pt1.Y;
			dZ = PtA.Z - Pt1.Z;

	double	t  =   ((l * dX) + (m * dY) + (n * dZ)) 
				 /
				   ((l * l ) + (m * m ) + (n * n ));

			dX -= (l * t);
			dY -= (m * t);
			dZ -= (n * t);

	return sqrt ( (dX * dX) + (dY * dY) + (dZ * dZ) );

}	//	DistPt2Line()


/*------------------------------  DistPt2Line()  ---------------------------*/
/*
*/
float PORT	DistPt2Line ( const fVertex & Pt1, 
						  const fVertex & Pt2,
						  const fVertex & PtA )
{
	dVector	dV1 ( Pt1 );
	dVector	dV2 ( Pt2 );
	dVector	dVA ( PtA );

	return (float)DistPt2Line ( dV1, dV2, dVA );

}	//	DistPt2Line()


/*----------------------------  DistPt2Line2D()  ---------------------------*/
/*
	Return shortest distance from point to line.

	See the double version of DistPt2Line for 3D points.  Here, Z is removed.
*/
float PORT	DistPt2Line2D ( float X1, float Y1,
						    float X2, float Y2,
						    float XA, float YA )
{
	double	dX = X2 - X1;
	double	dY = Y2 - Y1;

	double	d  = sqrt ( (dX * dX) + (dY * dY) );

	double	l  = dX / d;
	double	m  = dY / d;

			dX = XA - X1;
			dY = YA - Y1;

	double	t  =   ((l * dX) + (m * dY)) 
				 /
				   ((l * l ) + (m * m ));

			dX -= (l * t);
			dY -= (m * t);

	return (float)sqrt ( (dX * dX) + (dY * dY) );

}	//	DistPt2Line2D()


/*----------------------------  LinePtAtMinDist()  -------------------------*/
/*
	2003-Feb-22

	Return point on line (defined by Pt1, Pt2) from which it is the shortest 
	distance to a point not on the line (PtA).

	Like DistPt2Line() we find t at the minimum distance from PtA to
	the line.  Since we know the line we can then get the point on the line
	directly.
*/
dVector PORT	LinePtAtMinDist ( const dVector & Pt1, 
								  const dVector & Pt2,
								  const dVector & PtA )
{
	double	dX = Pt2.X - Pt1.X;
	double	dY = Pt2.Y - Pt1.Y;
	double	dZ = Pt2.Z - Pt1.Z;

	double	d  = sqrt ( (dX * dX) + (dY * dY) + (dZ * dZ) );

	double	l  = dX / d;
	double	m  = dY / d;
	double	n  = dZ / d;

			dX = PtA.X - Pt1.X;
			dY = PtA.Y - Pt1.Y;
			dZ = PtA.Z - Pt1.Z;

	double	t  =   ((l * dX) + (m * dY) + (n * dZ)) 
				 /
				   ((l * l) + (m * m) + (n * n));

	return dVector ( Pt1.X + (l * t), Pt1.Y + (m * t), Pt1.Z + (n * t) );

}	//	LinePtAtMinDist()


/*------------------------------  DistLine2Z()  ----------------------------*/
/*
	Return shortest distance from line to Z axis.

	See HWN 2000-Mar-01 p2, 3.

		Pt1, Pt2		Points defining the line.

		P			Point on the line where the minimum distance occurs.
					A line from this point to the Z axis will be 
					perpendicular with the line and Z axis.
*/
double PORT	DistLine2Z ( const dVector & Pt1, 
						 const dVector & Pt2,
							   dVector & P )
{
	double	dX = Pt2.X - Pt1.X;
	double	dY = Pt2.Y - Pt1.Y;
	double	dZ = Pt2.Z - Pt1.Z;

	double	d  = sqrt ( (dX * dX) + (dY * dY) + (dZ * dZ) );

	double	l  = dX / d;
	double	m  = dY / d;
	double	n  = dZ / d;

	//	If the line is (or nearly) parallel with Z ...
	//
	if ( (fabs ( l ) < 0.000001) && (fabs ( m ) < 0.0000001) )
	{
		P.X = Pt1.X;
		P.Y = Pt1.Y;				
		P.Z = (Pt2.Z + Pt1.Z) / 2.0;

		return sqrt ( (P.X * P.X) + (P.Y * P.Y) );
	}

	if ( fabs ( l ) < 0.000001 )		//	2009-Mar-19
	{
		P.X = Pt1.X;
		P.Y = 0;
		P.Z = Pt1.Z + ((-Pt1.Y * dZ) / dY);

		return fabs ( P.X );
	}

	if ( fabs ( m ) < 0.000001 )		//	2009-Mar-19
	{
		P.X = 0;
		P.Y = Pt1.Y;
		P.Z = Pt1.Z + ((-Pt1.X * dZ) / dX);
	
		return fabs ( P.Y );
	}

//	double	t  = ((Pt1.X * l) + (Pt1.Y * m)) / (2.0 * ((l * l) + (m * m)));
//	//
	//	2009-Mar-18
	//
//	double	t = (Pt1.X + Pt1.Y) / (l + m);		2009-Sep-21
//	//
	double	t = ((Pt1.X * l) + (Pt1.Y * m)) / ((l * l) + (m * m));

	double	X  = Pt1.X - (l * t);
	double	Y  = Pt1.Y - (m * t);

	P.X = X;
	P.Y = Y;

	if ( fabs ( l ) >= 0.000001 )
	{
		P.Z = ((n / l) * (X - Pt1.X)) + Pt1.Z;
	}
	else
		P.Z = ((n / m) * (Y - Pt1.Y)) + Pt1.Z;

	return sqrt ( (X * X) + (Y * Y) );

}	//	DistLine2Z()


/*------------------------------  DistPt2Plane()  ---------------------------*/
/*
	See .h.
*/
double PORT	DistPt2Plane ( double A, double B, double C, double D,
						   const dVector & P )
{
	double	n  = (A * P.X) + (B * P.Y) + (C * P.Z) + D;

	double	d  = sqrt ( (A * A) + (B * B) + (C * C) );

	return fabs ( n / d );

}	//	DistPt2Plane()


/*-----------------------------  DistLine2Line()  ---------------------------*/
/*
	See .h.
*/
double PORT	DistLine2Line ( const dVector & PtA1, const dVector & PtA2,
						    const dVector & PtB1, const dVector & PtB2,
							      dVector & PtA,        dVector & PtB )
{
	//	Use DistLine2Z() to get the center point.
	//
	//	One of the lines must be transformed to the Z axis.  So, use
	//	a contrived coordinate frame whose Z is one of the lines.
	//
	dVector	d  = PtA2 - PtA1;
	double  dx = fabs ( d.X ) > 0.0001 ? d.X : 0;
	double  dy = fabs ( d.Y ) > 0.0001 ? d.Y : 0;
	double  dz = fabs ( d.Z ) > 0.0001 ? d.Z : 0;
	double	ay = atan2 (  dx,  dz );
//	double	ay = atan2 ( -dz,  dx );
//	double	ax = atan2 (  dy,  sqrt ( (dx * dx) + (dz * dz) ) );
	double	ax = atan2 ( -dy,  sqrt ( (dx * dx) + (dz * dz) ) );

	CM4	Ry; Ry.RotateY ( ay );
	CM4	Rx;	Rx.RotateX ( ax );
//	CM4	T1;	T1.SetXYZ (      0,      0, PtA1.Mag() );
	CM4	T1;	T1.SetXYZ ( PtA1.X, PtA1.Y, PtA1.Z     );

//	CM4	Z;	Z = (Rx * Ry) * T1;
//	CM4	Z;	Z = (Ry * Rx) * T1;
	CM4	Z;	Z = Rx * Z;
			Z = Ry * Z;
			Z = T1 * Z;

	CM4 nZ;	nZ = ~Z;

	dVector	zPt21 = PtB1;	nZ.Point ( &zPt21 );
	dVector	zPt22 = PtB2;	nZ.Point ( &zPt22 );
//	dVector	zPt21 = PtB1;	 Z.Point ( &zPt21 );
//	dVector	zPt22 = PtB2;	 Z.Point ( &zPt22 );

	double D = DistLine2Z ( zPt21, zPt22, PtB );		//	PtB is on line B.

	 Z.Point ( &PtB );
//	nZ.Point ( &PtB );

	PtA = LinePtAtMinDist ( PtA1, PtA2, PtB );
	
	return D;

}	//	DistLine2Line()


/*-------------------------------  XYZToM4()  ------------------------------*/
/*
	See .h.
*/
void PORT	XYZToM4 ( double X, double Y, double Z, CM4 & M )
{
#define	RXZ_LIMIT	0.0000000001

	//	See illustration in HWN 2003-Nov-05 pg 1.

	M.SetI();

	double	rxz = sqrt ( (X * X) + (Z * Z) );

	if ( rxz < RXZ_LIMIT )
	{
		M.RotateX ( -90.0 * DEG2PI );

		return;
	}

	double	ThetaY = atan2 ( X, Z );

	M.RotateY (  ThetaY );

	double	ThetaX = atan2 ( Y, rxz );

	M.RotateX ( -ThetaX );

#undef	RXZ_LIMIT

}	//	XYZToM4()


/*-------------------------------  TriArea2D()  ----------------------------*/
/*
	From p160 of Handbook of Mathmatical Formulas, ..., for Mathmaticians,
	Scientists, Engineers.
*/
PORT double	TriArea2D ( double x1, double y1,
						double x2, double y2,
						double x3, double y3 )
{
	double	A;

	A = (x1 * y2) + (y1 * x3) + (y3 * x2) - (y2 * x3) - (y1 * x2) - (x1 * y3);

	A = A / 2.0;

	return fabs ( A );

}	//	TriArea2D()

	
/*-------------------------------  TriCenter()  ----------------------------*/
/*
	Returns the center of the triangle by averaging all the points.
*/
PORT dVector	TriCenter ( const dVector & Pt1,
						    const dVector &	Pt2,
							const dVector &	Pt3 )
{
	dVector	V;	V = Pt1 + Pt2 + Pt3;

	V /= 3.0;

	return V;

}	//	TriCenter()

	
/*------------------------------  LinesAngle()  ---------------------------*/
/*
	Return angle (degrees) between lines Pt1, PtA and PtA, Pt2.

	From p181 of "Handbook of Mathmatical Formulas, ..., for Mathmaticians,
	Scientists, Engineers".
*/
double PORT	LinesAngle ( const dVector & Pt1, 
						 const dVector & PtA,
						 const dVector & Pt2 )
{
	//	What if the points are colinear?

	double	dX = Pt1.X - PtA.X;
	double	dY = Pt1.Y - PtA.Y;
	double	dZ = Pt1.Z - PtA.Z;

	double	d  = sqrt ( (dX * dX) + (dY * dY) + (dZ * dZ) );

	double	l1 = dX / d;
	double	m1 = dY / d;
	double	n1 = dZ / d;

			dX = Pt2.X - PtA.X;
			dY = Pt2.Y - PtA.Y;
			dZ = Pt2.Z - PtA.Z;

			d  = sqrt ( (dX * dX) + (dY * dY) + (dZ * dZ) );

	double	l2 = dX / d;
	double	m2 = dY / d;
	double	n2 = dZ / d;

	return acos ( (l1 * l2) + (m1 * m2) + (n1 * n2) ) * PI2DEG;

}	//	LinesAngle()



/*-----------------------------  IsPointInside()  --------------------------*/
/*
*/
int	PORT	IsPointInside ( const fVertex & P, 
						    const fVertex &	V1, 
							const fVertex &	V2 )
{
	dVector	dP; 		dP  = P;
	dVector	dV1;		dV1 = V1;
	dVector	dV2;		dV2 = V2;

	return IsPointInside ( dP, dV1, dV2 );

}	//	IsPointInside()


/*-----------------------------  IsPointInside()  --------------------------*/
/*
	Assumes P is on a line defined by Pt1, Pt2.

	Return TRUE if P is within the boundaries of Pt1, Pt2.
*/
int	PORT	IsPointInside ( const dVector & P, 
						    const dVector &	Pt1, 
							const dVector &	Pt2,
								  double	Close )		//	= 0.0
{
/*
#define	PREC	0.000001		//	Precision.
*/
#define	PREC	0.0001			//	Precision.		2003-Jan-11

	if ( fabs ( Pt2.X - Pt1.X ) < PREC )
	{
		if ( fabs ( P.X - ((Pt2.X + Pt1.X) / 2.0) ) > PREC )  return FALSE;
	}
	else
	{
		//	if ( Pt2.X > Pt1.X )
		//	{
		//		if ( (P.X > Pt2.X) || (P.X < Pt1.X) )  return FALSE;
		//	}
		//	else
		//		if ( (P.X < Pt2.X) || (P.X > Pt1.X) )  return FALSE;
		//
		if ( ! Inside ( Pt2.X, Pt1.X, P.X, Close ) )  return FALSE;
	}

	if ( fabs ( Pt2.Y - Pt1.Y ) < PREC )
	{
		if ( fabs ( P.Y - ((Pt2.Y + Pt1.Y) / 2.0) ) > PREC )  return FALSE;
	}
	else
	{
		//	if ( Pt2.Y > Pt1.Y )
		//	{
		//		if ( (P.Y > Pt2.Y) || (P.Y < Pt1.Y) )  return FALSE;
		//	}
		//	else
		//		if ( (P.Y < Pt2.Y) || (P.Y > Pt1.Y) )  return FALSE;
		//
		if ( ! Inside ( Pt2.Y, Pt1.Y, P.Y, Close ) )  return FALSE;
	}

	if ( fabs ( Pt2.Z - Pt1.Z ) < PREC )
	{
		if ( fabs ( P.Z - ((Pt2.Z + Pt1.Z) / 2.0) ) > PREC )  return FALSE;
	}
	else
	{
		//	if ( Pt2.Z > Pt1.Z )
		//	{
		//		if ( (P.Z > Pt2.Z) || (P.Z < Pt1.Z) )  return FALSE;
		//	}
		//	else
		//		if ( (P.Z < Pt2.Z) || (P.Z > Pt1.Z) )  return FALSE;
		//
		if ( ! Inside ( Pt2.Z, Pt1.Z, P.Z, Close ) )  return FALSE;
	}

	return TRUE;

}	//	IsPointInside()


/*-----------------------------  IsPointOnLine()  --------------------------*/
/*
*/
int	PORT	IsPointOnLine (		  int		CP,
								  double	OnLineClose,
						    const dVector & P, 
						    const dVector &	V1, 
							const dVector &	V2 )
{
	dVector	dP; 		dP  = P;
	dVector	dV1;		dV1 = V1;
	dVector	dV2;		dV2 = V2;

	if ( DistPt2Line ( dV1, dV2, dP ) > OnLineClose )  return FALSE;

	switch ( CP )
	{
		case 1:		//	X, Y

			dP.Z = dV1.Z = dV2.Z = 0;	

			break;

		case 2:		//	Z, X

			dP.Y = dV1.Y = dV2.Y = 0;	

			break;

		case 3:		//	Y, Z

			dP.X = dV1.X = dV2.X = 0;	

			break;

		default:

			_ASSERTE ( "Oops." == 0 );
	}

	//	return IsPointInside ( dP, dV1, dV2 );
	//
	//	2003-Mar-13
	//
	return IsPointInside ( dP, dV1, dV2, -OnLineClose );

}	//	IsPointOnLine()


/*-----------------------------  IsPointOnLine()  --------------------------*/
/*
*/
int	PORT	IsPointOnLine ( const dVector & P, 
						    const dVector &	V1, 
							const dVector &	V2 )
{
/*
#define	PREC	0.000001		//	Precision.
*/
#define	PREC	0.0001			//	Precision.		2003-Jan-11

//	dVector	dP; 		dP  = P;
//	dVector	dV1;		dV1 = V1;
//	dVector	dV2;		dV2 = V2;

//	return IsPointOnLine ( PREC, dP, dV1, dV2 );
	return IsPointOnLine ( PREC,  P,  V1,  V2 );

#undef	PREC

}	//	IsPointOnLine()


/*-----------------------------  IsPointOnLine()  --------------------------*/
/*
	Assumes P is on a line defined by V1, V2.

	Return TRUE if P is within the boundaries of V1, V2.
*/
int	PORT	IsPointOnLine (		  double	OnLineClose,
						    const dVector & P, 
						    const dVector &	V1, 
							const dVector &	V2,
								  double	InLineClose )	//	= 0.0
{
	if ( DistPt2Line ( V1, V2, P ) > OnLineClose )  return FALSE;

	return IsPointInside ( P, V1, V2, InLineClose );

}	//	IsPointOnLine()


/*-------------------------------  LinePtZ()  ------------------------------*/
/*
	See .h.
*/
float PORT	LinePtZ ( const fVertex & V1, 
					  const fVertex & V2, float X, float Y )
{
	dVector	dV1 ( V1 );
	dVector dV2 ( V2 );

	return (float)LinePtZ ( dV1, dV2, X, Y );

}	//	LinePtZ()


/*-------------------------------  LinePtY()  ------------------------------*/
/*
	See .h.
*/
float PORT	LinePtY ( const fVertex & V1, 
					  const fVertex & V2, float X, float Z )
{
	dVector	dV1 ( V1 );
	dVector dV2 ( V2 );

	return (float)LinePtY ( dV1, dV2, X, Z );

}	//	LinePtY()


/*-------------------------------  LinePtX()  ------------------------------*/
/*
	See .h.
*/
float PORT	LinePtX ( const fVertex & V1, 
					  const fVertex & V2, float Y, float Z )
{
	dVector	dV1 ( V1 );
	dVector dV2 ( V2 );

	return (float)LinePtX ( dV1, dV2, Y, Z );

}	//	LinePtX()


/*-------------------------------  LinePtZ()  ------------------------------*/
/*
*/
double PORT	LinePtZ ( const dVector & V1, 
					  const dVector & V2, double X, double Y )
{
/*
#define	PREC	0.000001		//	Precision.
*/
#define	PREC	0.0001			//	Precision.		2003-Jan-11

	double	dX = V2.X - V1.X;
	double	dY = V2.Y - V1.Y;
	double	dZ = V2.Z - V1.Z;

	if ( fabs ( dZ ) <  PREC )  return (V2.Z + V1.Z) / 2;

	if ( fabs ( dX ) >= PREC )
	{
		return ((X - V1.X) * (dZ / dX)) + V1.Z;
	}

	_ASSERTE ( fabs ( dY ) >= PREC );

	return ((Y - V1.Y) * (dZ / dY)) + V1.Z;

#undef	PREC

}	//	LinePtZ()


/*-------------------------------  LinePtY()  ------------------------------*/
/*
*/
double PORT	LinePtY ( const dVector & V1, 
					  const dVector & V2, double X, double Z )
{
/*
#define	PREC	0.000001		//	Precision.
*/
#define	PREC	0.0001			//	Precision.		2003-Jan-11

	double	dX = V2.X - V1.X;
	double	dY = V2.Y - V1.Y;
	double	dZ = V2.Z - V1.Z;

	if ( fabs ( dY ) <  PREC )  return (V2.Y + V1.Y) / 2;

	if ( fabs ( dX ) >= PREC )
	{
		return ((X - V1.X) * (dY / dX)) + V1.Y;
	}

	_ASSERTE ( fabs ( dY ) >= PREC );

	return ((Z - V1.Z) * (dY / dZ)) + V1.Y;

#undef	PREC

}	//	LinePtY()


/*-------------------------------  LinePtX()  ------------------------------*/
/*
*/
double PORT	LinePtX ( const dVector & V1, 
					  const dVector & V2, double Y, double Z )
{
/*
#define	PREC	0.000001		//	Precision.
*/
#define	PREC	0.0001			//	Precision.		2003-Jan-11

	double	dX = V2.X - V1.X;
	double	dY = V2.Y - V1.Y;
	double	dZ = V2.Z - V1.Z;

	if ( fabs ( dX ) <  PREC )  return (V2.X + V1.X) / 2;

	if ( fabs ( dY ) >= PREC )
	{
		return ((Y - V1.Y) * (dX / dY)) + V1.X;
	}

	_ASSERTE ( fabs ( dZ ) >= PREC );

	return ((Z - V1.Z) * (dX / dZ)) + V1.X;

#undef	PREC

}	//	LinePtX()


/*-----------------------------  PointAverage()  ---------------------------*/
/*
*/
fVertex PORT	PointAverage ( const fVertex & V1, const fVertex & V2 )
{
	dVector	dV1 ( V1 ), dV2 ( V2 );

	fVertex	Avg;	Avg = PointAverage ( dV1, dV2 );

	return Avg;

}	//	PointAverage()


dVector PORT	PointAverage ( const dVector & V1, const dVector & V2 )
{
	dVector	Avg;

	Avg.X = (V1.X + V2.X) / 2.0;
	Avg.Y = (V1.Y + V2.Y) / 2.0;
	Avg.Z = (V1.Z + V2.Z) / 2.0;

	return Avg;

}	//	PointAverage()


/*------------------------------  SetPolarXY()  ----------------------------*/
/*
*/
void PORT	SetPolarXY ( float & X, float & Y, float R, float Theta )
{
	double	dX = X;
	double	dY = Y;

	SetPolarXY ( dX, dY, R, Theta );

	X = (float)dX;
	Y = (float)dY;

}	//	SetPolarXY()


void PORT	SetPolarXY ( double & X, double & Y, double R, double Theta )
{
	double	piTheta = Theta * PI2DEG;

	X = R * cos ( piTheta );

	Y = R * sin ( piTheta );

}	//	SetPolarXY()



/*---------------------------  SetSphericalXYZ()  --------------------------*/
/*
	From Handbook of Mathematical Formulas, etc, for Mathematicians, etc.,
	pg 185, 186.
*/
void PORT	SetSphericalXYZ ( double & X, double & Y, double & Z,
							  double   R, double   T, double   P )
{
	double	piT = T * PI2DEG;	//	Theta.
	double	piP = P * PI2DEG;	//	Phi.

	double	CT = cos ( piT );
	double	ST = sin ( piT );
	double	CP = cos ( piP );
	double	SP = sin ( piP );

	X = R * ST * CP;
	Y = R * ST * SP;
	Z = R * CT;

}	//	SetSphericalXYZ()



/*------------------------------  SetMinMax()  -----------------------------*/
/*
*/
void PORT	SetMinMax ( const dVector & V, dVector & Min, dVector & Max )
{
	if ( V.X < Min.X )  Min.X = V.X;
	if ( V.X > Max.X )  Max.X = V.X;

	if ( V.Y < Min.Y )  Min.Y = V.Y;
	if ( V.Y > Max.Y )  Max.Y = V.Y;

	if ( V.Z < Min.Z )  Min.Z = V.Z;
	if ( V.Z > Max.Z )  Max.Z = V.Z;

}	//	SetMinMax()



/*----------------------------  QuarticRoots()  ----------------------------*/
/*
	See .h.

	See HWN 2003-Feb-03 pg 7.
*/
void PORT	QuarticRoots ( double a3, double a2, double a1, double a0,
						   BOOL & b1, double & z1, 
						   BOOL & b2, double & z2, 
						   BOOL & b3, double & z3, 
						   BOOL & b4, double & z4,
						   double MaxErr )
{
	b1 = b2 = b3 = b4 = FALSE;		//	No roots found.


	double	u1;				//	Cubic root.

	if ( ! CubicRoot ( -a2, 
					   (a1 * a3) - (4.0 * a0),
					   -((a1 * a1) + (a0 * a3 * a3) - (4.0 * a0 * a2)),
					   u1 ) )
	{
		return;
	}


	//	Based on Wolfram Research's Quatric Equation.
	//
	double	s, R, v1, v2, v3, D, E, s1;

	s = (a3 * a3 / 4.0) - a2 + u1;

	if ( s < 0.0 )  return;

	if ( s < 0.0000000001 )  s = 0.0;

	R  = sqrt ( s );

	v1 = 3.0 * a3 * a3 / 4.0;

	v2 = 2.0 * a2;

	v3 = ((4.0 * a3 * a2) - (8.0 * a1) - (a3 * a3 * a3)) / 4.0;

	s1 = (u1 * u1) - (4.0 * a0);

	if ( R != 0.0 )
	{
		s = v1 - (R * R) - v2 + (v3 / R);

		if ( s >= 0.0 )
		{
			D = sqrt ( s );

			z1 = -(a3 / 4.0) + (R / 2.0) + (D / 2.0);

			b1 = MaxErr > 0 ? TestQuarticRoot ( a3, a2, a1, a0, z1, MaxErr ) 
							: TRUE;
			if ( D > 0.0 )
			{
				z2 = -(a3 / 4.0) + (R / 2.0) - (D / 2.0);

				b2 = MaxErr > 0 ? TestQuarticRoot ( a3, a2, a1, a0, z2, 
													MaxErr )
								: TRUE;
			}
		}
	}
	else
	{
		if ( s1 >= 0.0 )
		{
			s = v1 - v2 + (2.0 * sqrt ( s1 ));

			if ( s > 0.0 )
			{
				D = sqrt ( s );

				z1 = -(a3 / 4.0) + (D / 2.0);

				b1 = MaxErr > 0 ? TestQuarticRoot ( a3, a2, a1, a0, z1,
													MaxErr )
								: TRUE;
				if ( D > 0.0 )
				{
					z2 = -(a3 / 4.0) - (D / 2.0);

					b2 = MaxErr > 0 ? TestQuarticRoot ( a3, a2, a1, a0, z2,
														MaxErr )
									: TRUE;
				}
			}
		}
	}


	if ( R != 0.0 )
	{
		s = v1 - (R * R) - v2 - (v3 / R);

		if ( s >= 0.0 )
		{
			E = sqrt ( s );

			z3 = -(a3 / 4.0) - (R / 2.0) + (E / 2.0);
				
			b3 = MaxErr > 0 ? TestQuarticRoot ( a3, a2, a1, a0, z3, MaxErr )
							: TRUE;
			if ( E > 0.0 )
			{
				z4 = -(a3 / 4.0) - (R / 2.0) - (E / 2.0);
					
				b4 = MaxErr > 0 ? TestQuarticRoot ( a3, a2, a1, a0, z4,
													MaxErr )
								: TRUE;
			}
		}
	}
	else
	{
		if ( s1 >= 0.0 )
		{
			s = v1 - v2 - (2.0 * sqrt ( s1 ));

			if ( s > 0.0 )
			{
				E = sqrt ( s );

				z3 = -(a3 / 4.0) + (E / 2.0);	
				
				b3 = MaxErr > 0 ? TestQuarticRoot ( a3, a2, a1, a0, z3,
													MaxErr )
								: TRUE;
				if ( E > 0.0 )
				{
					z4 = -(a3 / 4.0) - (E / 2.0);	
					
					b4 = MaxErr > 0 ? TestQuarticRoot ( a3, a2, a1, a0, z4,
														MaxErr )
									: TRUE;
				}
			}
		}
	}

}	//	QuarticRoots()


/*------------------------------  CubicRoot()  -----------------------------*/
/*
	See .h.
*/
BOOL PORT	CubicRoot ( double a2, double a1, double a0, double & Root )
{
	//	See HWN 2003-Feb-03 pg 5.
	//
	//	Based on Handbook of Math Formulas, etc. pg 8, and on Wolfram
	//	Research Mathmatic web page "Cubic Equation".
	//
	double	Q = (a1 / 3.0) - ((a2 * a2) / 9.0);

	double	R = (((a1 * a2) - (3.0 * a0)) / 6.0) - ((a2 * a2 * a2) / 27.0);

	double	D = (Q * Q * Q) + (R * R);

	if ( D < 0.0 )
	{
		double	Theta = acos ( R / sqrt ( -(Q * Q * Q) ) );

		Root = (2.0 * sqrt ( -Q ) * cos ( Theta / 3.0 )) - (a2 / 3.0);

		return TRUE;
	}

	double	Dsqrt = sqrt ( D );

	double	S = CubedRoot ( R + Dsqrt );

	double	T = CubedRoot ( R - Dsqrt );

	Root = (-a2 / 3.0) + S + T;

	return TestCubicRoot ( a2, a1, a0, Root );

}	//	CubicRoot()


/*------------------------------  CubedRoot()  -----------------------------*/
/*
	See .h.
*/
double PORT	CubedRoot ( double a )
{
	double	s = a < 0.0 ? -1.0 : 1.0;

	a *= s;

	return s * pow ( a, 1.0 / 3.0 );

}	//	CubedRoot()



/*
	"re-polygonizing"
*/
struct	CTriangle
{
	dVector	V[3];

};	//	CTriangle

int PORT	TrianglesIntersect ( CTriangle T[], uint & nT )
{
	//	Original triangles.
	//
//	CTriangle	Ta = T[0];

//	CTriangle	Tb = T[1];

	return TRUE;

}	//	TrianglesIntersect



//	Private.

/*--------------------------  TestQuarticRoot()  ---------------------------*/
/*
*/
static BOOL	TestQuarticRoot ( double a3, double a2, double a1, double a0,
							  double z,  double	MaxErr )
{
	double	Zero =   (     z * z * z * z)
				   + (a3 * z * z * z)
				   + (a2 * z * z)
				   + (a1 * z)
				   +  a0;

	return fabs ( Zero ) <= MaxErr;

}	//	TestQuarticRoot()


/*---------------------------  TestCubicRoot()  ----------------------------*/
/*
*/
static BOOL	TestCubicRoot ( double a2, double a1, double a0,
							double z )
{
	double	Zero =   (     z * z * z)
				   + (a2 * z * z)
				   + (a1 * z)
				   +  a0;

	return fabs ( Zero ) < 0.0001;

}	//	TestCubicRoot()


/*----------------------------  InsideTriangle()  --------------------------*/
/*
*/
static BOOL	InsideTriangle (	   int			CP,
							 const dVector &	_P1,
							 const dVector &	_P2,
							 const dVector &	V1, 
							 const dVector &	V2,
							 const dVector &	V3,
								   double		InLineClose )	//	= 0.0
{

	dVector	P1 = _P1;

	dVector	P2 = _P2;

	dVector	P3;

	int		nI = 0;

	switch ( CP )
	{
		case 1:		//	X, Y

			P1.Z = P2.Z = P3.Z = 0;	

			if (    IntersectPoint ( P1.X, P1.Y, P2.X, P2.Y,
									 V1.X, V1.Y, V2.X, V2.Y, P3.X, P3.Y )
				 &&
					IsPointInside ( P3, P1, P2, InLineClose ) )  nI++;

			if (    IntersectPoint ( P1.X, P1.Y, P2.X, P2.Y,
									 V2.X, V2.Y, V3.X, V3.Y, P3.X, P3.Y )
				 &&
					IsPointInside ( P3, P1, P2, InLineClose ) )  nI++;

			if (    IntersectPoint ( P1.X, P1.Y, P2.X, P2.Y,
									 V3.X, V3.Y, V1.X, V1.Y, P3.X, P3.Y )
				 &&
					IsPointInside ( P3, P1, P2, InLineClose ) )  nI++;

			break;

		case 2:		//	Z, X

			P1.Y = P2.Y = P3.Y = 0;	

			if (    IntersectPoint ( P1.Z, P1.X, P2.Z, P2.X,
									 V1.Z, V1.X, V2.Z, V2.X, P3.Z, P3.X )
				 &&
					IsPointInside ( P3, P1, P2, InLineClose ) )  nI++;

			if (    IntersectPoint ( P1.Z, P1.X, P2.Z, P2.X,
									 V2.Z, V2.X, V3.Z, V3.X, P3.Z, P3.X )
				 &&
					IsPointInside ( P3, P1, P2, InLineClose ) )  nI++;

			if (    IntersectPoint ( P1.Z, P1.X, P2.Z, P2.X,
									 V3.Z, V3.X, V1.Z, V1.X, P3.Z, P3.X )
				 &&
					IsPointInside ( P3, P1, P2, InLineClose ) )  nI++;

			break;

		case 3:		//	Y, Z

			P1.X = P2.X = P3.X = 0;	

			if (    IntersectPoint ( P1.Y, P1.Z, P2.Y, P2.Z,
									 V1.Y, V1.Z, V2.Y, V2.Z, P3.Y, P3.Z )
				 &&
					IsPointInside ( P3, P1, P2, InLineClose ) )  nI++;

			if (    IntersectPoint ( P1.Y, P1.Z, P2.Y, P2.Z,
									 V2.Y, V2.Z, V3.Y, V3.Z, P3.Y, P3.Z )
				 &&
					IsPointInside ( P3, P1, P2, InLineClose ) )  nI++;

			if (    IntersectPoint ( P1.Y, P1.Z, P2.Y, P2.Z,
									 V3.Y, V3.Z, V1.Y, V1.Z, P3.Y, P3.Z )
				 &&
					IsPointInside ( P3, P1, P2, InLineClose ) )  nI++;

			break;

		default:

			_ASSERTE ( "Oops." == 0 );
	}

	return nI == 1;

}	//	InsideTriangle()


/*----------------------------  InsidePolygon()  ---------------------------*/
/*
	2003-Jan-24
*/
static BOOL	InsidePolygon (		  int		CP,
							const dVector &	_P1,
							const dVector &	_P2,
							const dVector	PPt[], uint nPPts,
								   double	InLineClose )	//	= 0.0
{

	dVector	P1 = _P1;

	dVector	P2 = _P2;

	dVector	P3;

	uint	i, j, nI = 0;

	switch ( CP )
	{
		case 1:		//	X, Y

			P1.Z = P2.Z = P3.Z = 0;	

			for ( j = nPPts - 1, i = 0; i < nPPts; j = i++ )
			{
				if (    IntersectPoint ( P1.X,     P1.Y, 
										 P2.X,     P2.Y,
										 PPt[j].X, PPt[j].Y, 
										 PPt[i].X, PPt[i].Y, 
										 P3.X,     P3.Y )
					 &&
						IsPointInside ( P3, P1, P2, InLineClose ) )
				{
					nI++;
				}
			}

			break;

		case 2:		//	Z, X

			P1.Y = P2.Y = P3.Y = 0;	

			for ( j = nPPts - 1, i = 0; i < nPPts; j = i++ )
			{
				if (    IntersectPoint ( P1.Z,     P1.X, 
										 P2.Z,     P2.X,
										 PPt[j].Z, PPt[j].X, 
										 PPt[i].Z, PPt[i].X, 
										 P3.Z,     P3.X )
					 &&
						IsPointInside ( P3, P1, P2, InLineClose ) )
				{
					nI++;
				}
			}

			break;

		case 3:		//	Y, Z

			P1.X = P2.X = P3.X = 0;	

			for ( j = nPPts - 1, i = 0; i < nPPts; j = i++ )
			{
				if (    IntersectPoint ( P1.Y,     P1.Z, 
										 P2.Y,     P2.Z,
										 PPt[j].Y, PPt[j].Z, 
										 PPt[i].Y, PPt[i].Z, 
										 P3.Y,     P3.Z )
					 &&
						IsPointInside ( P3, P1, P2, InLineClose ) )
				{
					nI++;
				}
			}

			break;

		default:

			_ASSERTE ( "Oops." == 0 );
	}

	return nI % 2 > 0;

}	//	InsidePolygon()


/*-------------------------------  Inside()  -------------------------------*/
/*
	Return TRUE if Vc is between Va and Vb by more than Close.

	That is, if Vc is between Va and Vb and Vc is more than Close 
	away from both Va and Vb then return TRUE.

->	Assume used in the context of determining if an intersection point
	is inside line end points.  Therefore, if Va == Vb assume the line
	is parallel with some axis and return TRUE if Vc is * within * 
	Close of the two.  This should be ok, especially for PolysOverlap(),
	since it calls Inside() for both dimensions of the intersection
	point - if one call errantly returns TRUE because of this assumption
	the next call should get it right when the test is along the axis.

	2003-Mar-05		Allowing Close to be negative.
	
	If close is negative then returns TRUE if Vc is within fabs ( Close )
	of either Va or Vb.
*/
static BOOL	Inside ( double Va, double Vb, double Vc, double Close )
{
	double	Tmp = Va;		double	AbsClose = fabs ( Close );

	if ( Vb < Va )  { Va = Vb;	Vb = Tmp; }

	if ( Vb - Va < AbsClose )	//	See the -> comment above. 
	{
		return     (fabs ( Vc - Va ) < AbsClose) 
				&& (fabs ( Vc - Vb ) < AbsClose);
	}

	return (Vc - Va > Close) && (Vb - Vc > Close);

}	//	Inside()



//	rs_math.cpp

