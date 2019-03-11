/*
		 1         2         3         4         5         6         7         8
12345678901234567890123456789012345678901234567890123456789012345678901234567890

	File:	m4

	Function members of the CM4 class.
*/


/*------------------------------- Dependencies  ----------------------------*/


#include	"stdio.h"

#include	"memory.h"


#include	"abb_rr_m4.h"

//	End Dependencies



#define	RPY_PRECISION		(0.00000001)

#define	AY90_PRECISION		(0.000001)			//	Degrees.


static void	GetRT ( const double M [4][4], double & Ax, 
										   double & Ay, 
										   double & Az );


//	int	CM4::nNews = 0;

/*---------------------------  uCM4::operator = ()  -------------------------*/
/*
*/
void	uCM4::operator = ( const CM4 & C )
{
	int				i, j;

	for ( i = 0; i < 4; i++ )
	for ( j = 0; j < 4; j++ )
	{
		M[i][j] = C.M[i][j];
	}

}	//	uCM4::operator = ()


/*---------------------------  uCM4::operator*()  --------------------------*/
/*
	C.M = M * T.M   or in other words   C = *this * T
*/
uCM4 	uCM4::operator * ( const CM4 & T ) const
{
	int		i, j, k;
	double	sum;
	uCM4	C;

	for ( i = 0; i < 4; i++ )
	for ( j = 0; j < 4; j++ )
	{
		sum = 0.0;

		for ( k = 0; k < 4; k++ )  sum = sum + (M[i][k] * T.M[k][j]);

		C.M[i][j] = sum;
	}

	return C;

}	//	uCM4::operator * ()


/*------------------------------  uCM4::SetI()  ----------------------------*/
/*
*/
void	uCM4::SetI ( void )
{
	int		i, j;

	for ( i = 0; i < 4; i++ )
	for ( j = 0; j < 4; j++ )
	{
		if ( i != j )
			M[i][j] = 0.0;
		else
			M[i][j] = 1.0;
	}

}	//	uCM4::SetI()


/*------------------------  uCM4::GetXYZRPY_Hosed()  -----------------------*/
/*

*/
void	uCM4::GetXYZRPY_Hosed ( double &Px, double &Py, double &Pz,
								double &Ax, double &Ay, double &Az	) const
{
	CM4	P ( *this );

	P.GetXYZRPY_Hosed ( Px, Py, Pz, Ax, Ay, Az );

}	//	uCM4::GetXYZRPY_Hosed()


/*-----------------------------  uCM4::GetRT()  ----------------------------*/
/*
*/
void	uCM4::GetRT ( double & Ax, double & Ay, double & Az ) const
{
	::GetRT ( M, Ax, Ay, Az );

}	//	uCM4::GetRT()


/*-----------------------------  uCM4::Point()  ----------------------------*/
/*
*/
void	uCM4::Point ( dVector * pP ) const
{
	double	X = pP->X, Y = pP->Y, Z = pP->Z;

	pP->X = (M[0][0] * X) + (M[0][1] * Y) + (M[0][2] * Z) + M[0][3];
	pP->Y = (M[1][0] * X) + (M[1][1] * Y) + (M[1][2] * Z) + M[1][3];
	pP->Z = (M[2][0] * X) + (M[2][1] * Y) + (M[2][2] * Z) + M[2][3];

}	//	uCM4::Point()







/*--------------------------------  CM4::CM4()  ----------------------------*/
/*
*/
CM4::CM4 ( double Px, double Py, double Pz,
		   double Ax, double Ay, double Az )
{
	//	TRP is used here because it always has been.
	//
	SetXYZRPY_Hosed ( Px, Py, Pz, Ax, Ay, Az );

}	//	CM4::CM4()


/*--------------------------------  CM4::CM4()  ----------------------------*/
/*
*/
CM4::CM4 ( const CM4 & A )
{
	/*
	int		i, j;

	for ( i = 0; i < 4; i++ )
	for ( j = 0; j < 4; j++ )
	{
		M[i][j] = A.M[i][j];
	}
	*/
	memcpy ( M, A.M, sizeof(M) );

}	//	CM4::CM4()


/*--------------------------------  CM4::CM4()  ----------------------------*/
/*
*/
CM4::CM4 ( const dVector &	n,
		   const dVector &	o,
		   const dVector &	a,
		   const dVector &	p )
{
	M[0][0] = n.X;	M[1][0] = n.Y;	M[2][0] = n.Z;	M[3][0] = 0.0;
	M[0][1] = o.X;	M[1][1] = o.Y;	M[2][1] = o.Z;	M[3][1] = 0.0;
	M[0][2] = a.X;	M[1][2] = a.Y;	M[2][2] = a.Z;	M[3][2] = 0.0;
	M[0][3] = p.X;	M[1][3] = p.Y;	M[2][3] = p.Z;	M[3][3] = 1.0;

}	//	CM4::CM4()


/*--------------------------------  CM4::CM4()  ----------------------------*/
/*
*/
CM4::CM4 ( const uCM4 & A )
{
	memcpy ( M, A.M, sizeof(M) );

}	//	CM4::CM4()


/*-------------------------------  CM4::~CM4()  ----------------------------*/
/*
*/
CM4::~CM4 ( void)
{
	// Just here so we can tell when the destructor is called while
	// debugging.

}	//	CM4::CM4()


/*------------------------------  CM4::Export()  ---------------------------*/
/*
*/
#ifndef	JS
int		CM4::Export ( CExport & Exp, const char * Desc, ... )
{
	va_list	Args;

	va_start ( Args, Desc );

	char	Buffer[1024];

	vsprintf ( Buffer, Desc, Args );

	va_end ( Args );

	char	Str [16];

	strncpy ( Str, Buffer, sizeof(Str) - 1 );

	Str[sizeof(Str) - 1] = '\0';


	uint	nBuffC = strlen ( Str ) + 0;

	char	Fmt [80];

	sprintf ( Fmt, "%%-%ds %%16.6f %%16.6f %%16.6f %%16.6f\n", nBuffC );

	Exp.Submit ( Fmt, Str, M[0][0], M[0][1], M[0][2], M[0][3] );

	Exp.Submit ( Fmt, "",  M[1][0], M[1][1], M[1][2], M[1][3] );

	Exp.Submit ( Fmt, "",  M[2][0], M[2][1], M[2][2], M[2][3] );

	Exp.Submit ( Fmt, "",  M[3][0], M[3][1], M[3][2], M[3][3] );

	return 0;

}	//	CM4::Export()
#endif

/*------------------------------  CM4::Import()  ---------------------------*/
/*
*/
#ifndef	JS
int		CM4::Import ( CImport & Imp, const char * Desc, ... )
{
	va_list	Args;

	va_start ( Args, Desc );

	char	Buffer[1024];

	vsprintf ( Buffer, Desc, Args );

	va_end ( Args );

	char	Str [64];

	strncpy ( Str, Buffer, sizeof(Str) - 1 );

	Str[sizeof(Str) - 1] = '\0';

	Imp.Accept ( Str, "dddd", &M[0][0], &M[0][1], &M[0][2], &M[0][3] );

	Imp.Accept ( "",  "dddd", &M[1][0], &M[1][1], &M[1][2], &M[1][3] );

	Imp.Accept ( "",  "dddd", &M[2][0], &M[2][1], &M[2][2], &M[2][3] );

	Imp.Accept ( "",  "dddd", &M[3][0], &M[3][1], &M[3][2], &M[3][3] );

	return 0;

}	//	CM4::Import()
#endif

/*------------------------------  CM4::GetM()  -----------------------------*/
/*
*/
const double *	CM4::GetM ( void ) const
{
	return &M[0][0];

}	//	CM4::GetM()




/*-------------------------------  CM4::SetI()  ----------------------------*/
/*
*/
void	CM4::SetI ( void )
{
	int		i, j;

	for ( i = 0; i < 4; i++ )
	for ( j = 0; j < 4; j++ )
	{
		if ( i != j )
			M[i][j] = 0.0;
		else
			M[i][j] = 1.0;
	}

}	//	CM4::SetI()




/*-----------------------------  CM4::SetElement()  ------------------------*/
/*
*/
void	CM4::SetElement ( int Row, int Col, double Val )
{
	M[Row][Col] = Val;

}	//	CM4::SetElement()




/*----------------------------  CM4::GetElement()  -------------------------*/
/*
*/
double
CM4::GetElement ( int Row, int Col )
{
	return M[Row][Col];

}	//	CM4::GetElement()



/*------------------------------  CM4::GetX()  -----------------------------*/
/*
*/
double	CM4::GetX() const
{
	return M[0][3];

}	//	CM4::GetX()


/*------------------------------  CM4::GetY()  -----------------------------*/
/*
*/
double	CM4::GetY() const
{
	return M[1][3];

}	//	CM4::GetY()


/*------------------------------  CM4::GetZ()  -----------------------------*/
/*
*/
double	CM4::GetZ() const
{
	return M[2][3];

}	//	CM4::GetZ()


/*-----------------------------  CM4::GetXYZ()  ----------------------------*/
/*
*/
void	CM4::GetXYZ ( double & Px, double & Py, double & Pz ) const
{
	Px = M[0][3];
	Py = M[1][3];
	Pz = M[2][3];

}	//	CM4::GetXYZ()


/*-----------------------------  CM4::SetXYZ()  ----------------------------*/
/*
*/
void	CM4::SetXYZ ( double Px, double Py, double Pz )
{
	M[0][3] = Px;
	M[1][3] = Py;
	M[2][3] = Pz;

}	//	CM4::SetXYZ()



/*-------------------------  CM4::SetXYZRPY_Hosed()  -----------------------*/
/*
	Per Paul, pg 47, eq 2.20, this specifies the coordinates to be located
	at Px, Py, Pz, and to be rotated first by Ax, then Ay, then Az.

	TRP		HWN 2001-Nov-03 and OLH (On Line Help).	
*/
void	CM4::SetXYZRPY_Hosed ( double Px, double Py, double Pz,
							   double Ax, double Ay, double Az )
{
	double	s1, c1, s2, c2, s3, c3 ;

	s1 = sin ( Ax );	// about x
	c1 = cos ( Ax );
	s2 = sin ( Ay );	// about y
	c2 = cos ( Ay );
	s3 = sin ( Az );	// about z
	c3 = cos ( Az );

	//	This is Pauls roll pitch and yaw.  The rotations are done in the
	//	X Y Z order but they are pre-multiplied.  That is, the rotations
	//	are about the origin.
	//		
	M[0][0] =   c3 * c2;
	M[0][1] =  (c3 * s2 * s1) - (s3 * c1);
	M[0][2] =  (c3 * s2 * c1) + (s3 * s1);
	M[0][3] =   Px;
	M[1][0] =   s3 * c2;
	M[1][1] =  (s3 * s2 * s1) + (c3 * c1);
	M[1][2] =  (s3 * s2 * c1) - (c3 * s1);
	M[1][3] =   Py;
	M[2][0] =  -s2;
	M[2][1] =   c2 * s1;
	M[2][2] =   c2 * c1;
	M[2][3] =   Pz;
	M[3][0] =   0.0;
	M[3][1] =   0.0;
	M[3][2] =   0.0;
	M[3][3] =   1.0;


}	//	CM4::SetXYZRPY_Hosed()


/*-------------------------  CM4::GetXYZRPY_Hosed()  -----------------------*/
/*
	This returns the location of the coordinates, and the angle Az (+- 180),
	Ay and Ax.  See Paul, pgs 70, 71.

*/
void	CM4::GetXYZRPY_Hosed ( double &Px, double &Py, double &Pz,
							   double &Ax, double &Ay, double &Az	) const
{
	Px = M[0][3];
	Py = M[1][3];
	Pz = M[2][3];

	GetRPY_Hosed ( Ax, Ay, Az );

}	//	CM4::GetXYZRPY_Hosed()


/*---------------------------  CM4::GetRPY_Hosed()  ------------------------*/
/*
	See Paul, pgs 70, 71.
*/
void	CM4::GetRPY_Hosed ( double & Ax, double & Ay, double & Az ) const
{
	double	ny = M[1][0];		if ( fabs ( ny ) < RPY_PRECISION )  ny = 0;

	double	nx = M[0][0];		if ( fabs ( nx ) < RPY_PRECISION )  nx = 0;

	double	nz = M[2][0];		if ( fabs ( nz ) < RPY_PRECISION )  nz = 0;

	double	ax = M[0][2];		if ( fabs ( ax ) < RPY_PRECISION )  ax = 0;

	double	ay = M[1][2];		if ( fabs ( ay ) < RPY_PRECISION )  ay = 0;

	double	ox = M[0][1];		if ( fabs ( ox ) < RPY_PRECISION )  ox = 0;

	double	oy = M[1][1];		if ( fabs ( oy ) < RPY_PRECISION )  oy = 0;


	Az = atan2 ( ny, nx );	//	+- Pi.

	double	Cz = cos ( Az ),	Sz = sin ( Az );

	Ay = atan2 ( -nz, (Cz * nx) + (Sz * ny) );

	Ax = atan2 ( (Sz * ax) - (Cz * ay), -(Sz * ox) + (Cz * oy) );

}	//	CM4::GetRPY_Hosed()




/*-----------------------------  CM4::SetRT()  -----------------------------*/
/*
*/
void	CM4::SetRT ( double Px, double Py, double Pz,
					 double Ax, double Ay, double Az )
{
	double	s1, c1, s2, c2, s3, c3 ;

	s1 = sin ( Ax );	// about x
	c1 = cos ( Ax );
	s2 = sin ( Ay );	// about y
	c2 = cos ( Ay );
	s3 = sin ( Az );	// about z
	c3 = cos ( Az );

	//	//	2003-Dec-17
	//	//
	//	//	RT		See HWN of this date page 1.
	//	//
	//	//	The rotation is the same as that previously used for what was 
	//	//	called TPR (HWN 2001-Nov-03 pg 1).  Here, that rotation is 
	//	//	_post_multiplied by the translation.
	//	//
	//	M[0][0] =   c2 * c3;
	//	M[0][1] = -(c2 * s3); 
	//	M[0][2] =   s2;
	//	M[0][3] =  (M[0][0] * Px) + (M[0][1] * Py) + (M[0][2] * Pz);
	//	M[1][0] =  (s1 * s2 * c3) + (c1 * s3);
	//	M[1][1] = -(s1 * s2 * s3) + (c1 * c3);
	//	M[1][2] = -(s1 * c2);
	//	M[1][3] =  (M[1][0] * Px) + (M[1][1] * Py) + (M[1][2] * Pz);
	//	M[2][0] = -(c1 * s2 * c3) + (s1 * s3);
	//	M[2][1] =  (c1 * s2 * s3) + (s1 * c3);
	//	M[2][2] =  (c1 * c2);
	//	M[2][3] =  (M[2][0] * Px) + (M[2][1] * Py) + (M[2][2] * Pz);
	//	M[3][0] =   0.0;
	//	M[3][1] =   0.0;
	//	M[3][2] =   0.0;
	//	M[3][3] =   1.0;
	//
	//	2003-Dec-19
	//
	//	Rotate in Z Y X order.
	//
	M[0][0] =   c3 * c2;
	M[0][1] =  (c3 * s2 * s1) - (s3 * c1);
	M[0][2] =  (c3 * s2 * c1) + (s3 * s1);
	M[0][3] =  (M[0][0] * Px) + (M[0][1] * Py) + (M[0][2] * Pz);
	M[1][0] =   s3 * c2;
	M[1][1] =  (s3 * s2 * s1) + (c3 * c1);
	M[1][2] =  (s3 * s2 * c1) - (c3 * s1);
	M[1][3] =  (M[1][0] * Px) + (M[1][1] * Py) + (M[1][2] * Pz);
	M[2][0] =  -s2;
	M[2][1] =   c2 * s1;
	M[2][2] =   c2 * c1;
	M[2][3] =  (M[2][0] * Px) + (M[2][1] * Py) + (M[2][2] * Pz);
	M[3][0] =   0.0;
	M[3][1] =   0.0;
	M[3][2] =   0.0;
	M[3][3] =   1.0;


}	//	CM4::SetRT()


/*-----------------------------  CM4::SetRT()  -----------------------------*/
/*
*/
void	CM4::SetRT ( struct P6 * P )
{
	SetRT ( P->Px, P->Py, P->Pz, P->Ax, P->Ay, P->Az );
	
}	//	CM4::SetRT()


/*-----------------------------  CM4::GetRT()  -----------------------------*/
/*

void	CM4::GetRT ( double &Px, double &Py, double &Pz,
					 double &Ax, double &Ay, double &Az	)	const
{
	//	See HWN 2003-Dec-17
	//
	//	3 equations and 3 unknowns.
	//
	//	This stuff tries to avoid denominators close to 0.
	//
	double	A0 = M[0][0], A1 = M[1][0], A2 = M[2][0];
	double	B0 = M[0][1], B1 = M[1][1], B2 = M[2][1];
	double	C0 = M[0][2], C1 = M[1][2], C2 = M[2][2];
	double	D0 = M[0][3], D1 = M[1][3], D2 = M[2][3];

	double	d01, d02, d11, d12, d21, d22;		//	Denominators.

	double	sm0, sm1, sm2;						//	Sums of denominators.

	double	aR, bR, cR, aL, bL, cL;

	//	X.  See HWN 2003-Dec-17 pg 5.
	//
	d01 = (C1 * B0) - (C0 * B1);
	d02 = (C1 * B2) - (C2 * B1);	sm0 = fabs ( d01 ) + fabs ( d02 );

	d11 = (C0 * B1) - (C1 * B0);
	d12 = (C0 * B2) - (C2 * B0);	sm1 = fabs ( d11 ) + fabs ( d12 );

	d21 = (C2 * B0) - (C0 * B2);
	d22 = (C2 * B1) - (C1 * B2);	sm2 = fabs ( d21 ) + fabs ( d22 );

	switch ( iMax ( sm0, sm1, sm2 ) )
	{
		case 0:	aR = (C1 * D0) - (C0 * D1);		
				bR = (C0 * A1) - (C1 * A0);
				cR = d01;

				aL = (C1 * D2) - (C2 * D1);
				bL = (C2 * A1) - (C1 * A2);
				cL = d02;

				break;

		case 1:	aR = (C0 * D1) - (C1 * D0);	
				bR = (C1 * A0) - (C0 * A1);
				cR = d11;

				aL = (C0 * D2) - (C2 * D0);
				bL = (C2 * A0) - (C0 * A2);
				cL = d12;

				break;
	
		case 2:	aR = (C2 * D0) - (C0 * D2);
				bR = (C0 * A2) - (C2 * A0);
				cR = d21;

				aL = (C2 * D1) - (C1 * D2);
				bL = (C1 * A2) - (C2 * A1);
				cL = d22;

				break;
	}

	Px = ((cR * aR) - (cL * aL)) / ((cL * bL) - (cR * bR));


	//	Y.	See HWN 2003-Dec-17 pg 6.
	//
	d01 = (A1 * C0) - (A0 * C1);
	d02 = (A1 * C2) - (A2 * C1);	sm0 = fabs ( d01 ) + fabs ( d02 );

	d11 = (A0 * C1) - (A1 * C0);
	d12 = (A0 * C2) - (A2 * C0);	sm1 = fabs ( d11 ) + fabs ( d12 );

	d21 = (A2 * C0) - (A0 * C2);
	d22 = (A2 * C1) - (A1 * C2);	sm2 = fabs ( d21 ) + fabs ( d22 );

	switch ( iMax ( sm0, sm1, sm2 ) )
	{
		case 0:	aR = (A1 * D0) - (A0 * D1);
				bR = (A0 * B1) - (A1 * B0);
				cR = d01;

				aL = (A1 * D2) - (A2 * D1);
				bL = (A2 * B1) - (A1 * B2);
				cL = d02;

				break;

		case 1:	aR = (A0 * D1) - (A1 * D0);
				bR = (A1 * B0) - (A0 * B1);
				cR = d11;

				aL = (A0 * D2) - (A2 * D0);
				bL = (A2 * B0) - (A0 * B2);
				cL = d12;

				break;
	
		case 2:	aR = (A2 * D0) - (A0 * D2);
				bR = (A0 * B2) - (A2 * B0);
				cR = d21;

				aL = (A2 * D1) - (A1 * D2);
				bL = (A1 * B2) - (A2 * B1);
				cL = d22;

				break;
	}

	Py = ((cR * aR) - (cL * aL)) / ((cL * bL) - (cR * bR));


	//	Z.	See HWN 2003-Dec-17 pgs 3, 4.
	//
	d01 = (B1 * A0) - (B0 * A1);
	d02 = (B1 * A2) - (B2 * A1);	sm0 = fabs ( d01 ) + fabs ( d02 );

	d11 = (B2 * A0) - (B0 * A2);
	d12 = (B2 * A1) - (B1 * A2);	sm1 = fabs ( d11 ) + fabs ( d12 );

	d21 = (B0 * A1) - (B1 * A0);
	d22 = (B0 * A2) - (B2 * A0);	sm2 = fabs ( d21 ) + fabs ( d22 );

	switch ( iMax ( sm0, sm1, sm2 ) )
	{
		case 0:	aR = (B1 * D0) - (B0 * D1);
				bR = (B0 * C1) - (B1 * C0);
				cR = d01;

				aL = (B1 * D2) - (B2 * D1);
				bL = (B2 * C1) - (B1 * C2);
				cL = d02;

				break;

		case 1:	aR = (B2 * D0) - (B0 * D2);
				bR = (B0 * C2) - (B2 * C0);
				cR = d11;

				aL = (B2 * D1) - (B1 * D2);
				bL = (B1 * C2) - (B2 * C1);
				cL = d12;

				break;
	
		case 2:	aR = (B0 * D1) - (B1 * D0);
				bR = (B1 * C0) - (B0 * C1);
				cR = d21;

				aL = (B0 * D2) - (B2 * D0);
				bL = (B2 * C0) - (B0 * C2);
				cL = d22;

				break;
	}

	Pz = ((cR * aR) - (cL * aL)) / ((cL * bL) - (cR * bR));
	

	GetRT ( Ax, Ay, Az );

}	//	CM4::GetRT()
*/
/*-----------------------------  CM4::GetRT()  -----------------------------*/
/*
*/
void	CM4::GetRT ( double &Px, double &Py, double &Pz,
					 double &Ax, double &Ay, double &Az	)	const
{
	//	See HWN 2003-Dec-17
	//
	//	3 equations and 3 unknowns.
	//
	//	This stuff tries to avoid denominators close to 0.
	//
	double	A0 = M[0][0], A1 = M[1][0], A2 = M[2][0];
	double	B0 = M[0][1], B1 = M[1][1], B2 = M[2][1];
	double	C0 = M[0][2], C1 = M[1][2], C2 = M[2][2];
	double	D0 = M[0][3], D1 = M[1][3], D2 = M[2][3];

	double	XaR0, XbR0, XcR0, XaL0, XbL0, XcL0;
	double	XaR1, XbR1, XcR1, XaL1, XbL1, XcL1;
	double	XaR2, XbR2, XcR2, XaL2, XbL2, XcL2;

	double	YaR0, YbR0, YcR0, YaL0, YbL0, YcL0;
	double	YaR1, YbR1, YcR1, YaL1, YbL1, YcL1;
	double	YaR2, YbR2, YcR2, YaL2, YbL2, YcL2;

	double	ZaR0, ZbR0, ZcR0, ZaL0, ZbL0, ZcL0;
	double	ZaR1, ZbR1, ZcR1, ZaL1, ZbL1, ZcL1;
	double	ZaR2, ZbR2, ZcR2, ZaL2, ZbL2, ZcL2;

	double	n0, d0, n1, d1, n2, d2;				//	Numerators and Denomitors.

	double	Xn = 0, Xd = 0, Yn = 0, Yd = 0, Zn = 0, Zd = 0;

	//	X.  See HWN 2003-Dec-17 pg 5.
	//
	XaR0 = (C1 * D0) - (C0 * D1);		
	XbR0 = (C0 * A1) - (C1 * A0);
	XcR0 = (C1 * B0) - (C0 * B1);
	XaL0 = (C1 * D2) - (C2 * D1);
	XbL0 = (C2 * A1) - (C1 * A2);
	XcL0 = (C1 * B2) - (C2 * B1);

	n0 = ((XcL0 * XaR0) - (XcR0 * XaL0));
	d0 = ((XcR0 * XbL0) - (XcL0 * XbR0));


	XaR1 = (C0 * D1) - (C1 * D0);	
	XbR1 = (C1 * A0) - (C0 * A1);
	XcR1 = (C0 * B1) - (C1 * B0);
	XaL1 = (C0 * D2) - (C2 * D0);
	XbL1 = (C2 * A0) - (C0 * A2);
	XcL1 = (C0 * B2) - (C2 * B0);

	n1 = ((XcL1 * XaR1) - (XcR1 * XaL1));
	d1 = ((XcR1 * XbL1) - (XcL1 * XbR1));


	XaR2 = (C2 * D0) - (C0 * D2);
	XbR2 = (C0 * A2) - (C2 * A0);
	XcR2 = (C2 * B0) - (C0 * B2);
	XaL2 = (C2 * D1) - (C1 * D2);
	XbL2 = (C1 * A2) - (C2 * A1);
	XcL2 = (C2 * B1) - (C1 * B2);

	n2 = ((XcL2 * XaR2) - (XcR2 * XaL2));
	d2 = ((XcR2 * XbL2) - (XcL2 * XbR2));


	switch ( iuMax ( d0, d1, d2 ) )
	{
		case 0:	Xn = n0;	Xd = d0;	break;

		case 1:	Xn = n1;	Xd = d1;	break;

		case 2:	Xn = n2;	Xd = d2;	break;
	}


	//	Y.	See HWN 2003-Dec-17 pg 6.
	//
	YaR0 = (A1 * D0) - (A0 * D1);
	YbR0 = (A0 * B1) - (A1 * B0);
	YcR0 = (A1 * C0) - (A0 * C1);
	YaL0 = (A1 * D2) - (A2 * D1);
	YbL0 = (A2 * B1) - (A1 * B2);
	YcL0 = (A1 * C2) - (A2 * C1);

	n0 = ((YcL0 * YaR0) - (YcR0 * YaL0));
	d0 = ((YcR0 * YbL0) - (YcL0 * YbR0));


	YaR1 = (A0 * D1) - (A1 * D0);
	YbR1 = (A1 * B0) - (A0 * B1);
	YcR1 = (A0 * C1) - (A1 * C0);
	YaL1 = (A0 * D2) - (A2 * D0);
	YbL1 = (A2 * B0) - (A0 * B2);
	YcL1 = (A0 * C2) - (A2 * C0);

	n1 = ((YcL1 * YaR1) - (YcR1 * YaL1));
	d1 = ((YcR1 * YbL1) - (YcL1 * YbR1));


	YaR2 = (A2 * D0) - (A0 * D2);
	YbR2 = (A0 * B2) - (A2 * B0);
	YcR2 = (A2 * C0) - (A0 * C2);
	YaL2 = (A2 * D1) - (A1 * D2);
	YbL2 = (A1 * B2) - (A2 * B1);
	YcL2 = (A2 * C1) - (A1 * C2);

	n2 = ((YcL2 * YaR2) - (YcR2 * YaL2));
	d2 = ((YcR2 * YbL2) - (YcL2 * YbR2));


	switch ( iuMax ( d0, d1, d2 ) )
	{
		case 0:	Yn = n0;	Yd = d0;	break;

		case 1:	Yn = n1;	Yd = d1;	break;

		case 2:	Yn = n2;	Yd = d2;	break;
	}



	//	Z.	See HWN 2003-Dec-17 pgs 3, 4.
	//
	ZaR0 = (B1 * D0) - (B0 * D1);
	ZbR0 = (B0 * C1) - (B1 * C0);
	ZcR0 = (B1 * A0) - (B0 * A1);
	ZaL0 = (B1 * D2) - (B2 * D1);
	ZbL0 = (B2 * C1) - (B1 * C2);
	ZcL0 = (B1 * A2) - (B2 * A1);

	n0 = ((ZcL0 * ZaR0) - (ZcR0 * ZaL0));
	d0 = ((ZcR0 * ZbL0) - (ZcL0 * ZbR0));


	ZaR1 = (B2 * D0) - (B0 * D2);
	ZbR1 = (B0 * C2) - (B2 * C0);
	ZcR1 = (B2 * A0) - (B0 * A2);
	ZaL1 = (B2 * D1) - (B1 * D2);
	ZbL1 = (B1 * C2) - (B2 * C1);
	ZcL1 = (B2 * A1) - (B1 * A2);

	n1 = ((ZcL1 * ZaR1) - (ZcR1 * ZaL1));
	d1 = ((ZcR1 * ZbL1) - (ZcL1 * ZbR1));


	ZaR2 = (B0 * D1) - (B1 * D0);
	ZbR2 = (B1 * C0) - (B0 * C1);
	ZcR2 = (B0 * A1) - (B1 * A0);
	ZaL2 = (B0 * D2) - (B2 * D0);
	ZbL2 = (B2 * C0) - (B0 * C2);
	ZcL2 = (B0 * A2) - (B2 * A0);

	n2 = ((ZcL2 * ZaR2) - (ZcR2 * ZaL2));
	d2 = ((ZcR2 * ZbL2) - (ZcL2 * ZbR2));


	switch ( iuMax ( d0, d1, d2 ) )
	{
		case 0:	Zn = n0;	Zd = d0;	break;

		case 1:	Zn = n1;	Zd = d1;	break;

		case 2:	Zn = n2;	Zd = d2;	break;
	}


	//	Now based on the best denominator in X, Y, Z...
	//
	switch ( iuMax ( Xd, Yd, Zd ) )
	{
		case 0:

			Px = Xn / Xd;

			switch ( iuMax ( XcR0, XcL0, XcL1 ) )
			{
				case 0:	Py = (XaR0 + (XbR0 * Px)) / XcR0;	break;

				case 1:	Py = (XaL0 + (XbL0 * Px)) / XcL0;	break;

				case 2:	Py = (XaL1 + (XbL1 * Px)) / XcL1;	break;
			}

			switch ( iuMax ( C0, C1, C2 ) )
			{
				case 0:	Pz = (D0 - ((A0 * Px) + (B0 * Py))) / C0;	break;

				case 1:	Pz = (D1 - ((A1 * Px) + (B1 * Py))) / C1;	break;

				case 2:	Pz = (D2 - ((A2 * Px) + (B2 * Py))) / C2;	break;
			}

			break;

		case 1:

			Py = Yn / Yd;

			switch ( iuMax ( YcR0, YcL0, YcL1 ) )
			{
				case 0:	Pz = (YaR0 + (YbR0 * Py)) / YcR0;	break;

				case 1:	Pz = (YaL0 + (YbL0 * Py)) / YcL0;	break;

				case 2:	Pz = (YaL1 + (YbL1 * Py)) / YcL1;	break;
			}

			switch ( iuMax ( A0, A1, A2 ) )
			{
				case 0:	Px = (D0 - ((B0 * Py) + (C0 * Pz))) / A0;	break;

				case 1:	Px = (D1 - ((B1 * Py) + (C1 * Pz))) / A1;	break;

				case 2:	Px = (D2 - ((B2 * Py) + (C2 * Pz))) / A2;	break;
			}

			break;

		case 2:

			Pz = Zn / Zd;

			switch ( iuMax ( ZcR0, ZcL0, ZcR1 ) )
			{
				case 0:	Px = (ZaR0 + (ZbR0 * Pz)) / ZcR0;	break;

				case 1:	Px = (ZaL0 + (ZbL0 * Pz)) / ZcL0;	break;

				case 2:	Px = (ZaR1 + (ZbR1 * Pz)) / ZcR1;	break;
			}

			switch ( iuMax ( B0, B1, B2 ) )
			{
				case 0:	Py = (D0 - ((A0 * Px) + (C0 * Pz))) / B0;	break;

				case 1:	Py = (D1 - ((A1 * Px) + (C1 * Pz))) / B1;	break;

				case 2:	Py = (D2 - ((A2 * Px) + (C2 * Pz))) / B2;	break;
			}

			break;
	}

	GetRT ( Ax, Ay, Az );

}	//	CM4::GetRT()


/*-----------------------------  CM4::GetRT()  -----------------------------*/
/*
*/
void	CM4::GetRT ( struct P6 * P ) const
{
	GetRT ( P->Px, P->Py, P->Pz, P->Ax, P->Ay, P->Az );

}	//	CM4::GetRT()


/*------------------------------  CM4::GetRT()  ----------------------------*/
/*
*/
void	CM4::GetRT ( double & Ax, double & Ay, double & Az ) const
{
	::GetRT ( M, Ax, Ay, Az );

}	//	CM4::GetRT()




/*--------------------------  CM4::SetXYZRPY_TPR()  ------------------------*/
/*
*/
void	CM4::SetXYZRPY_TPR ( double Px, double Py, double Pz,
							 double Ax, double Ay, double Az )
{
	double	s1, c1, s2, c2, s3, c3 ;

	s1 = sin ( Ax );	// about x
	c1 = cos ( Ax );
	s2 = sin ( Ay );	// about y
	c2 = cos ( Ay );
	s3 = sin ( Az );	// about z
	c3 = cos ( Az );

	//	2001-Nov-03
	//
	//	TPR		See HWN of this date page 1.
	//
	//	The translation is pre-multiplied and then the rotations are done
	//	about the translated frame.
	//
	M[0][0] =   c2 * c3;
	M[0][1] = -(c2 * s3); 
	M[0][2] =   s2;
	M[0][3] =   Px;
	M[1][0] =  (s1 * s2 * c3) + (c1 * s3);
	M[1][1] = -(s1 * s2 * s3) + (c1 * c3);
	M[1][2] = -(s1 * c2);
	M[1][3] =   Py;
	M[2][0] = -(c1 * s2 * c3) + (s1 * s3);
	M[2][1] =  (c1 * s2 * s3) + (s1 * c3);
	M[2][2] =  (c1 * c2);
	M[2][3] =   Pz;
	M[3][0] =   0.0;
	M[3][1] =   0.0;
	M[3][2] =   0.0;
	M[3][3] =   1.0;


}	//	CM4::SetXYZRPY_TPR()


/*--------------------------  CM4::GetXYZRPY_TPR()  ------------------------*/
/*
*/
void	CM4::GetXYZRPY_TPR ( double &Px, double &Py, double &Pz,
							 double &Ax, double &Ay, double &Az	)	const
{
	Px = M[0][3];
	Py = M[1][3];
	Pz = M[2][3];

	GetRPY_TPR ( Ax, Ay, Az );

}	//	CM4::GetXYZRPY_TPR()


/*----------------------------  CM4::GetRPY_TPR()  -------------------------*/
/*
	TPR		HWN 2001-Nov-03 pgs 1, 3, 4.
*/
void	CM4::GetRPY_TPR ( double & Ax, double & Ay, double & Az ) const
{
	double	ny = M[1][0];		if ( fabs ( ny ) < RPY_PRECISION )  ny = 0;

	double	nz = M[2][0];		if ( fabs ( nz ) < RPY_PRECISION )  nz = 0;

	double	ax = M[0][2];		if ( fabs ( ax ) < RPY_PRECISION )  ax = 0;

	double	ay = M[1][2];		if ( fabs ( ay ) < RPY_PRECISION )  ay = 0;

	double	az = M[2][2];		if ( fabs ( az ) < RPY_PRECISION )  az = 0;

	double	oy = M[1][1];		if ( fabs ( oy ) < RPY_PRECISION )  oy = 0;

	double	oz = M[2][1];		if ( fabs ( oz ) < RPY_PRECISION )  oz = 0;

	//	2001-Nov-03
	//
	Ax = atan2 ( -ay, az );

	double	Cx = cos ( Ax ),	Sx = sin ( Ax );

	Ay = atan2 ( ax, (-Sx * ay) + (Cx * az) );

	Az = atan2 ( (Cx * ny) + (Sx * nz), (Cx * oy) + (Sx * oz) );

}	//	CM4::GetRPY_TPR()


/*-----------------------------  CM4::SetTR()  -----------------------------*/
/*
*/
void	CM4::SetTR ( double Px, double Py, double Pz,
					 double Ax, double Ay, double Az )
{
	SetXYZRPY_TPR ( Px, Py, Pz, Ax, Ay, Az );

}	//	CM4::SetTR()


/*------------------------------  CM4::GetTR()  ----------------------------*/
/*
*/
void	CM4::GetTR ( double & Px, double & Py, double & Pz,
					 double & Ax, double & Ay, double & Az	) const
{
	GetXYZRPY_TPR ( Px, Py, Pz, Ax, Ay, Az );

}	//	CM4::GetTR()


/*------------------------------  CM4::GetTR()  ----------------------------*/
/*
	TPR		HWN 2001-Nov-03 pgs 1, 3, 4.
*/
void	CM4::GetTR ( double & Ax, double & Ay, double & Az ) const
{
	GetRPY_TPR ( Ax, Ay, Az );

}	//	CM4::GetTR()


/*------------------------------  CM4::GetAZ()  ----------------------------*/
/*
	Use GetRT ( double & Ax, double & Ay, double & Az );

double	CM4::GetAZ() const
{
	double	ny = M[1][0];		if ( fabs ( ny ) < RPY_PRECISION )  ny = 0;

	double	nx = M[0][0];		if ( fabs ( nx ) < RPY_PRECISION )  nx = 0;

	
	double	Az = atan2 ( ny, nx );	//	+- Pi.

	return Az;

}	//	CM4::GetAZ()
*/



/*---------------------------  CM4::operator = ()  -------------------------*/
/*
	M = T.M;

CM4 &	CM4::operator =	( const CM4 & T )
*/
CM4		CM4::operator =	( const CM4 & T )
{
	int	i, j;

	for ( i = 0; i < 4; i++ )
	for ( j = 0; j < 4; j++ )
	{
		M[i][j] = T.M[i][j];
	}

	return *this;

}	//	CM4::operator = ()


/*---------------------------  CM4::operator = ()  -------------------------*/
/*
	M = T.M;
*/
CM4		CM4::operator =	( const uCM4 & U )
{
	int				i, j;

	for ( i = 0; i < 4; i++ )
	for ( j = 0; j < 4; j++ )
	{
		M[i][j] = U.M[i][j];
	}

	return *this;

}	//	CM4::operator = ()


/*----------------------------  CM4::operator*()  --------------------------*/
/*
	C.M = M * T.M   or in other words   C = *this * T

	12-Apr-96
	*this transformed by T?   For example, if T is a rotation only matrix,
	does this rotate *this by the rotation so that the location of *this
	does not change?

CM4 &	CM4::operator *	( const CM4 & T ) const
*/
CM4 	CM4::operator *	( const CM4 & T ) const
{
	int				i, j, k;
	double			sum;
//	static CM4		C;
	CM4				C;

	for ( i = 0; i < 4; i++ )
	for ( j = 0; j < 4; j++ )
	{
		sum = 0.0;

		for ( k = 0; k < 4; k++ )  sum = sum + (M[i][k] * T.M[k][j]);

		C.M[i][j] = sum;
	}

	return C;

}	//	CM4::operator * ()


/*----------------------------  CM4::operator*()  --------------------------*/
/*
	C.M = M * T.M   or in other words   C = *this * T
*/
CM4 	CM4::operator *	( const uCM4 & T ) const
{
	int				i, j, k;
	double			sum;
	CM4				C;

	for ( i = 0; i < 4; i++ )
	for ( j = 0; j < 4; j++ )
	{
		sum = 0.0;

		for ( k = 0; k < 4; k++ )  sum = sum + (M[i][k] * T.M[k][j]);

		C.M[i][j] = sum;
	}

	return C;

}	//	CM4::operator * ()


/*-----------------------------  CM4::operator~()  -------------------------*/
/*
	Operator for inverting a homogeneous matrix.

	Return the homogeneous inverse of this matrix.

	Ref. Paul, "Robot Manipulators", 1981, pg 25.

CM4 &	CM4::operator~()
*/
CM4 	CM4::operator~() const
{
//	static CM4		Inv;
	CM4				Inv;

	Inv.M[0][0] = M[0][0];
	Inv.M[0][1] = M[1][0];
	Inv.M[0][2] = M[2][0];
	Inv.M[0][3] = - ColumnDotProduct ( 3, 0 );
	Inv.M[1][0] = M[0][1];
	Inv.M[1][1] = M[1][1];
	Inv.M[1][2] = M[2][1];
	Inv.M[1][3] = - ColumnDotProduct ( 3, 1 );
	Inv.M[2][0] = M[0][2];
	Inv.M[2][1] = M[1][2];
	Inv.M[2][2] = M[2][2];
	Inv.M[2][3] = - ColumnDotProduct ( 3, 2 );
	Inv.M[3][0] = 0.0;
	Inv.M[3][1] = 0.0;
	Inv.M[3][2] = 0.0;
	Inv.M[3][3] = 1.0;

	return Inv;

}	//	CM4::operator ~ ()


/*---------------------------  CM4::operator == ()  ------------------------*/
/*
*/
int		CM4::operator == ( const CM4 & A ) const
{
	int		i, j;

	for ( i = 0; i < 4; i++ )
	for ( j = 0; j < 4; j++ )
	{
		if ( M[i][j] != A.M[i][j] )  return FALSE;
	}

	return TRUE;

}	//	CM4::operator == ()


/*---------------------------  CM4::operator != ()  ------------------------*/
/*
*/
int		CM4::operator != ( const CM4 & A ) const
{
	return ! (*this == A);

}	//	CM4::operator != ()


/*------------------------------  CM4::Transpose()  ------------------------*/
/*
*/
void	CM4::Transpose ( void )
{
	CM4	T ( *this );
	
	int	i, j;

	for ( i = 0; i < 4; i++ )
	for ( j = 0; j < 4; j++ )
	{
		M[i][j] = T.M[j][i];
	}

}	//	CM4::Transpose()



/*--------------------------  CM4::ColumnDotProduct()  ---------------------*/
/*
	Calculate the dot product of two columns of this matrix.
*/
double	CM4::ColumnDotProduct ( int c1, int c2 ) const
{
	double	dot;
	int     r;

	for ( dot = 0.0, r = 0; r < 4; r++ )  dot += M[r][c1] * M[r][c2];

	return dot;

}	//	CM4::ColumnDotProduct()



/*------------------------------  CM4::Point()  ----------------------------*/
/*
*/
void	CM4::Point ( dVector * pP ) const
{
	double	X = pP->X, Y = pP->Y, Z = pP->Z;

	pP->X = (M[0][0] * X) + (M[0][1] * Y) + (M[0][2] * Z) + M[0][3];
	pP->Y = (M[1][0] * X) + (M[1][1] * Y) + (M[1][2] * Z) + M[1][3];
	pP->Z = (M[2][0] * X) + (M[2][1] * Y) + (M[2][2] * Z) + M[2][3];

}	//	CM4::Point()


/*-----------------------------  CM4::Point()  -----------------------------*/
/*
*/
void	CM4::Point ( fVertex * pP ) const
{
	double	X = pP->X, Y = pP->Y, Z = pP->Z;

	pP->X = (float)((M[0][0] * X) + (M[0][1] * Y) + (M[0][2] * Z) + M[0][3]);
	pP->Y = (float)((M[1][0] * X) + (M[1][1] * Y) + (M[1][2] * Z) + M[1][3]);
	pP->Z = (float)((M[2][0] * X) + (M[2][1] * Y) + (M[2][2] * Z) + M[2][3]);

}	//	CM4::Point()


/*------------------------------  CM4::Point()  ----------------------------*/
/*
*/
void	CM4::Point ( double * pX, double * pY, double * pZ ) const
{
	double	X = *pX, Y = *pY, Z = *pZ;

	*pX = (M[0][0] * X) + (M[0][1] * Y) + (M[0][2] * Z) + M[0][3];
	*pY = (M[1][0] * X) + (M[1][1] * Y) + (M[1][2] * Z) + M[1][3];
	*pZ = (M[2][0] * X) + (M[2][1] * Y) + (M[2][2] * Z) + M[2][3];

}	//	CM4::Point()


/*----------------------------  CM4::operator*()  --------------------------*/
/*
*/
dVector &	CM4::operator *	( const dVector & Pt )
{
	static	dVector	V = Pt;

	V.X = Pt.X;
	V.Y = Pt.Y;
	V.Z = Pt.Z;

	Point ( &V );

	return V;

}	//	CM4::operator * ()


/*------------------------------  CM4::Inc()  ------------------------------*/
/*

double		CM4::Inc	( uint iV, double dV )
{
	_ASSERTE ( (iV >= 0) && (iV < 6) );

	switch ( iV )
	{
		case 0:	return IncPX ( dV );

		case 1:	return IncPY ( dV );

		case 2:	return IncPZ ( dV );

		case 3:	return IncAX ( dV );

		case 4:	return IncAY ( dV );

		case 5:	return IncAZ ( dV );
	}

	return 0;

}	//	CM4::Inc()
*/

/*-----------------------------  CM4::IncPX()  -----------------------------*/
/*

double		CM4::IncPX	( double dX )
{
	double	X, Y, Z, AX, AY, AZ;

	GetXYZRPY ( X, Y, Z, AX, AY, AZ );

	X += dX;

	SetXYZRPY ( X, Y, Z, AX, AY, AZ );

	return X;

}	//	CM4::IncPX()
*/

/*-----------------------------  CM4::IncPY()  -----------------------------*/
/*

double		CM4::IncPY	( double dY )
{
	double	X, Y, Z, AX, AY, AZ;

	GetXYZRPY_TRP ( X, Y, Z, AX, AY, AZ );

	Y += dY;

	SetXYZRPY_TRP ( X, Y, Z, AX, AY, AZ );

	return Y;

}	//	CM4::IncPY()
*/

/*-----------------------------  CM4::IncPZ()  -----------------------------*/
/*

double		CM4::IncPZ	( double dZ )
{
	double	X, Y, Z, AX, AY, AZ;

	GetXYZRPY_TRP ( X, Y, Z, AX, AY, AZ );

	Z += dZ;

	SetXYZRPY_TRP ( X, Y, Z, AX, AY, AZ );

	return Z;

}	//	CM4::IncPZ()
*/

/*-----------------------------  CM4::IncAX()  -----------------------------*/
/*

double		CM4::IncAX	( double dAX )
{
	double	X, Y, Z, AX, AY, AZ;

	GetXYZRPY ( X, Y, Z, AX, AY, AZ );

	AX += dAX;

	SetXYZRPY ( X, Y, Z, AX, AY, AZ );

	return AX;

}	//	CM4::IncAX()
*/

/*-----------------------------  CM4::IncAY()  -----------------------------*/
/*

double		CM4::IncAY	( double dAY )
{
	double	X, Y, Z, AX, AY, AZ;

	GetXYZRPY ( X, Y, Z, AX, AY, AZ );

	AY += dAY;

	SetXYZRPY ( X, Y, Z, AX, AY, AZ );

	return AY;

}	//	CM4::IncAY()
*/

/*-----------------------------  CM4::IncAZ()  -----------------------------*/
/*

double		CM4::IncAZ	( double dAZ )
{
	double	X, Y, Z, AX, AY, AZ;

	GetXYZRPY ( X, Y, Z, AX, AY, AZ );

	AZ += dAZ;

	SetXYZRPY ( X, Y, Z, AX, AY, AZ );

	return AZ;

}	//	CM4::IncAZ()
*/

/*----------------------------  CM4::RotateX()  ----------------------------*/
/*
	See .h.
*/
void	CM4::RotateX ( double dA )
{
	CM4	T ( 0, 0, 0, dA, 0, 0 );

	*this = *this * T;

}	//	CM4::RotateX()


/*----------------------------  CM4::RotateY()  ----------------------------*/
/*
	See .h.
*/
void	CM4::RotateY ( double dA )
{
	CM4	T ( 0, 0, 0, 0, dA, 0 );

	*this = *this * T;

}	//	CM4::RotateY()


/*----------------------------  CM4::RotateZ()  ----------------------------*/
/*
	See .h.
*/
void	CM4::RotateZ ( double dA )
{
	CM4	T ( 0, 0, 0, 0, 0, dA );

	*this = *this * T;

}	//	CM4::RotateZ()


/*--------------------------  CM4::RotateByOrg()  --------------------------*/
/*
	See .h.
*/
void	CM4::RotateByOrg ( int Axis, double dA )
{
	double	X, Y, Z;		GetXYZ ( X, Y, Z );

	CM4	L  (  X,  Y,  Z, 0, 0, 0 );

	CM4	nL ( -X, -Y, -Z, 0, 0, 0 );

	//	Translate to the origin.
	//
	CM4	P  = nL * *this;

	//	Rotate it in origin coordinates.
	//
	switch ( Axis )
	{
		case 1:	P = CM4 ( 0, 0, 0, dA, 0, 0 ) * P;		break;

		case 2:	P = CM4 ( 0, 0, 0, 0, dA, 0 ) * P;		break;

		case 3:	P = CM4 ( 0, 0, 0, 0, 0, dA ) * P;		break;

		default:	_ASSERTE ( "Oops." == 0 );
	}

	//	Translate it back.
	//
	*this = L * P;

}	//	CM4::RotateByOrg()


/*--------------------------------  CM4::n()  ------------------------------*/
/*
*/
dVector	CM4::n()
{
	return dVector ( M[0][0], M[1][0], M[2][0] );

}	//	CM4::n()


/*--------------------------------  CM4::o()  ------------------------------*/
/*
*/
dVector	CM4::o()
{
	return dVector ( M[0][1], M[1][1], M[2][1] );

}	//	CM4::o()


/*--------------------------------  CM4::a()  ------------------------------*/
/*
*/
dVector	CM4::a()
{
	return dVector ( M[0][2], M[1][2], M[2][2] );

}	//	CM4::a()


/*--------------------------------  CM4::p()  ------------------------------*/
/*
*/
dVector	CM4::p()
{
	return dVector ( M[0][3], M[1][3], M[2][3] );

}	//	CM4::p()


/*-----------------------------  CM4::SetPaulA()  ---------------------------*/
/*
	See .h.
*/
void	CM4::SetPaulA ( double Theta, 
						double Dist, 
						double Length, 
						double Alpha )
{
	double	ST = sin ( Theta );
	double	CT = cos ( Theta );
	double	SA = sin ( Alpha );
	double	CA = cos ( Alpha );

	M[0][0] =   CT;
	M[0][1] =  -ST * CA;
	M[0][2] =   ST * SA;
	M[0][3] =   Length * CT;

	M[1][0] =   ST;
	M[1][1] =   CT * CA;
	M[1][2] =  -CT * SA;
	M[1][3] =   Length * ST;

	M[2][0] =   0;
	M[2][1] =   SA;
	M[2][2] =   CA;
	M[2][3] =   Dist;

	M[3][0] = 0;
	M[3][1] = 0;
	M[3][2] = 0;
	M[3][3] = 1;

}	//	CM4::SetPaulA()


/*-----------------------------  CM4::GetFloats()  ---------------------------*/
/*
*/
void	CM4::GetFloats ( float F[] )
{
	F[ 0] = (float)M[0][0];
	F[ 1] = (float)M[0][1];
	F[ 2] = (float)M[0][2];
	F[ 3] = (float)M[0][3];

	F[ 4] = (float)M[1][0];
	F[ 5] = (float)M[1][1];
	F[ 6] = (float)M[1][2];
	F[ 7] = (float)M[1][3];

	F[ 8] = (float)M[2][0];
	F[ 9] = (float)M[2][1];
	F[10] = (float)M[2][2];
	F[11] = (float)M[2][3];

	F[12] = (float)M[3][0];
	F[13] = (float)M[3][1];
	F[14] = (float)M[3][2];
	F[15] = (float)M[3][3];

}	//	CM4::GetFloats()







//	Private.

/*-----------------------------  CM4::iuMax()  -----------------------------*/
/*
*/
int		CM4::iuMax ( double v1, double v2, double v3 ) const
{
	v1 = fabs ( v1 );

	v2 = fabs ( v2 );

	v3 = fabs ( v3 );

	if ( (v1 >= v2) && (v1 >= v3) )  return 0;

	if ( (v2 >= v1) && (v2 >= v3) )  return 1;

	return 2;

}	//	CM4::iuMax()




static void	GetRT ( const double M [4][4], double & Ax, 
										   double & Ay, 
										   double & Az )
{
	//	//	2003-Dec-17
	//	//
	//	//	The rotations are the same as those used in SetXYZRPY_TPR().
	//	//
	//	//	Therefore, this is the same as GetXYZRPY_TPR().
	//
	//	double	ny = M[1][0];		if ( fabs ( ny ) < RPY_PRECISION )  ny = 0;
	//
	//	double	nz = M[2][0];		if ( fabs ( nz ) < RPY_PRECISION )  nz = 0;
	//
	//	double	ax = M[0][2];		if ( fabs ( ax ) < RPY_PRECISION )  ax = 0;
	//
	//	double	ay = M[1][2];		if ( fabs ( ay ) < RPY_PRECISION )  ay = 0;
	//
	//	double	az = M[2][2];		if ( fabs ( az ) < RPY_PRECISION )  az = 0;
	//
	//	double	oy = M[1][1];		if ( fabs ( oy ) < RPY_PRECISION )  oy = 0;
	//
	//	double	oz = M[2][1];		if ( fabs ( oz ) < RPY_PRECISION )  oz = 0;
	//
	//	//	2001-Nov-03
	//	//
	//	Ax = atan2 ( -ay, az );
	//
	//	double	Cx = cos ( Ax ),	Sx = sin ( Ax );
	//
	//	Ay = atan2 ( ax, (-Sx * ay) + (Cx * az) );
	//
	//	Az = atan2 ( (Cx * ny) + (Sx * nz), (Cx * oy) + (Sx * oz) );
	//
	//	2003-Dec-19
	//
	//	Make the rotations Z Y X.  Then, since Z is first, that rotation will
	//	always move about the refence frame's Z.  (That is, if Z were after
	//	X and Y, for example, then the rotation would be about the Z resulting
	//	from those first two rotations.)
	//
	double	ny = M[1][0];		if ( fabs ( ny ) < RPY_PRECISION )  ny = 0;

	double	nx = M[0][0];		if ( fabs ( nx ) < RPY_PRECISION )  nx = 0;

	double	nz = M[2][0];		if ( fabs ( nz ) < RPY_PRECISION )  nz = 0;

	double	ax = M[0][2];		if ( fabs ( ax ) < RPY_PRECISION )  ax = 0;

	double	ay = M[1][2];		if ( fabs ( ay ) < RPY_PRECISION )  ay = 0;

	double	ox = M[0][1];		if ( fabs ( ox ) < RPY_PRECISION )  ox = 0;

	double	oy = M[1][1];		if ( fabs ( oy ) < RPY_PRECISION )  oy = 0;

	Az = atan2 ( ny, nx );	//	+- Pi.

	double	Cz = cos ( Az ),	Sz = sin ( Az );

	Ay = atan2 ( -nz, (Cz * nx) + (Sz * ny) );

	Ax = atan2 ( (Sz * ax) - (Cz * ay), -(Sz * ox) + (Cz * oy) );

	//	If Ay is close to +/- 90 then Ax and Az are "redundant".
	//
	if ( (90.0 - (fabs ( Ay ) * PI2DEG)) < AY90_PRECISION )
	{
		//	Set Az to Ax and Ax to 0.
		//
		if ( Ay > 0 )  Az = -Ax;
		else
			Az = Ax;

		Ax = 0;
	}

}	//	GetRT()



//	m4.cpp

