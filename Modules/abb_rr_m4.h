/*
		 1         2         3         4         5         6         7
12345678901234567890123456789012345678901234567890123456789012345678901234567

	File:	m4.h

	Class definition for the Play2 4 X 4 matrix.


	1999-Jul-09

	Porting to the 
	
		DOD			Do Or Die

		RWS1		Real Working System #1

		PS			Pre-Shipping

		P$			Pre-Money

	version of R & S DS.
*/

#ifndef		__M4_H
#define		__M4_H

/*------------------------------- Dependencies  ----------------------------*/

#ifndef		__STDLIB_H
#include	<stdlib.h>
#endif

#ifndef		__MATH_H
#include	<math.h>
#endif

#include	"abb_rr_rs_typedefs.h"

#include	"abb_rr_rs_math.h"

//	End Dependencies


#define PORT


class	CM4;


//  Use CM4::SetRT() and CM4::GetRT() to convert to and from a CM4.
//
struct	P6
{
	double Px;
	double Py;
	double Pz;
	double Ax;
	double Ay;
	double Az;

};	//	P6


enum	TransformOrder
{
	toUndefined		= 0x00000000,

	toRT			= 0x00000001,	//	Use CM4::Set/GetRT()

	toTR            = 0x00000002	//	Use CM4::Set/GetTR()

};	//	TransformOrder


struct PORT	uCM4			//	Something that can be a member of a union.
{						
	double	M [4][4];

	void	operator = ( const CM4 & It );

	uCM4	operator * ( const CM4 & A ) const;

	void	SetI ( void );

	void	GetXYZRPY_Hosed ( double &Px, double &Py, double &Pz,
							  double &Ax, double &Ay, double &Az ) const;

	void	GetRT ( double & Ax, double & Ay, double & Az ) const;

	void	Point ( dVector * pP ) const;

};	//	uCM4


/*-----------------------------  class CM4  -------------------------------*/
/*
*/
class PORT	CM4
{
	friend struct	uCM4;

	public:

		CM4 ( double	Px = 0.0,		//	Note that this sets things
			  double	Py = 0.0,		//	according to the "TRP" method
			  double	Pz = 0.0,		//	(see below) only because it 
			  double	Ax = 0.0,		//	always has.
			  double	Ay = 0.0,
			  double	Az = 0.0 );


		CM4 ( const CM4 & A );


		CM4 ( const dVector &	n,		//	Initializes to the specifiec
			  const dVector &	o,		//	columns.
			  const dVector &	a,
			  const dVector &	p );

		CM4 ( const uCM4 & A );

	   ~CM4();



		const double *	GetM() const;

		void			SetI ( void );

		void			SetElement ( int Row, int Col, double Val );

		double			GetElement ( int Row, int Col );

		double			GetX() const;

		double			GetY() const;

		double			GetZ() const;

		void			GetXYZ ( double & Px, double & Py, double & Pz ) const;

		void			SetXYZ ( double   Px, double   Py, double   Pz );


		//	TRP		See HWN 2001-Nov-03 and OLH (On Line Help).
		//
		//	This is what Paul describes pgs 70, 71.
		//
		//	Use this when working with relative positions of robot links.
		//
		//	2003-Dec-17		Hosed...
		//
		//		Not implemented as described in Paul.  The order of rotations
		//		is wrong.
		//
		//	Until replaced/fixed/renamed-properly, and all code that uses 
		//	this mistake is fixed, renaming from _TRP to _Hosed.
		//	
		void			SetXYZRPY_Hosed ( double Px = 0.0,
									 	  double Py = 0.0,
										  double Pz = 0.0,
										  double Ax = 0.0,
										  double Ay = 0.0,
										  double Az = 0.0 );

		void			GetXYZRPY_Hosed ( double &, double &, double &,
										  double &, double &, double & ) const;

		void			GetRPY_Hosed ( double & Ax, 
									   double & Ay, 
									   double & Az ) const;


		//	2003-Dec-17
		//
		//	RT		Rotate, then Translate.
		//
		//	No P in the notation - it is confusing.  It is not a part of the
		//	internal stuff here.  The resulting transform will have different
		//	affect if pre or post multiplied the current P.
		//
		//	->	And, know that the rotation is done in Z Y X order.
		//
		void			SetRT ( double Px = 0.0,
								double Py = 0.0,
								double Pz = 0.0,
								double Ax = 0.0,
								double Ay = 0.0,
								double Az = 0.0 );

		void			SetRT ( struct P6 * P );

		void			GetRT ( double &, double &, double &,
								double &, double &, double & ) const;

		void			GetRT ( struct P6 * P ) const;

		void			GetRT ( double & Ax, 
								double & Ay, 
								double & Az ) const;


		//	TPR		See HWN 2001-Nov-03 and OLH (On Line Help).
		//
		void			SetXYZRPY_TPR ( double Px = 0.0,
										double Py = 0.0,
										double Pz = 0.0,
										double Ax = 0.0,
										double Ay = 0.0,
										double Az = 0.0 );

		void			GetXYZRPY_TPR ( double &, double &, double &,
										double &, double &, double & ) const;

		void			GetRPY_TPR ( double & Ax, 
									 double & Ay, 
									 double & Az ) const;

		//	2003-Dec-19
		//
		//	TR		Translate then rotate.
		//
		//	Rotate in X Y Z order.
		//
		void			SetTR ( double Px = 0.0,
								double Py = 0.0,
								double Pz = 0.0,
								double Ax = 0.0,
								double Ay = 0.0,
								double Az = 0.0 );

		void			GetTR ( double &, double &, double &,
								double &, double &, double & ) const;

		void			GetTR ( double & Ax, 
								double & Ay, 
								double & Az ) const;



		//	For robot link theta.
		//
	//	double			GetAZ() const;
	//
	//	Use GetRT  ( double & Ax, double & Ay, double & Az ).


	//	CM4	& operator	= ( const CM4 & A );
		CM4	  operator	= ( const CM4 & A );

		CM4	  operator	= ( const uCM4 & U );

	//	CM4	& operator	* ( const CM4 & A ) const;
		CM4	  operator	* ( const CM4 & A ) const;

		CM4	  operator	* ( const uCM4 & A ) const;

	//	CM4	& operator	~ ();
		CM4	  operator	~ () const;

		int operator	== ( const CM4 & A ) const;

		int operator	!= ( const CM4 & A ) const;

		void			Transpose ( void );

		double			ColumnDotProduct ( int c1, int c2 ) const;

		//	*pP = M * *pP;
		//
	//	void			Point ( dVertex * pP ) const;
		void			Point ( dVector * pP ) const;

		void			Point ( fVertex * pP ) const;

		void			Point ( double * pX, double * pY, double * pZ ) const;


		//	Pt = M * Pt;
		//
		dVector	&		operator * ( const dVector & Pt );


		//	iV is 0 then increment X by dV.
		//	iV is 1 then increment Y by dV, etc.
		//
		//	iV is 5 then increment AZ by dV.
		//
		//	Returns new value.
		//
	//	double			Inc ( uint iV, double dV );

	//	double			IncPX ( double dX );

	//	double			IncPY ( double dY );

	//	double			IncPZ ( double dZ );

	//	double			IncAX ( double dAX );

	//	double			IncAY ( double dAY );

	//	double			IncAZ ( double dAZ );


		//	Rotate about the current X, Y or Z axis the specified amount.
		//
		//		dA		Radians
		//
		void			RotateX ( double dA );

		void			RotateY ( double dA );

		void			RotateZ ( double dA );


		//	Rotates this * in place * about the specifed axis * of the 
		//	origin *.
		//
		//	That is, this is translated to the origin, rotated about the
		//	specified origin axis, and translated back to its original
		//	location.
		//
		//		Axis	1	X of origin
		//				2	Y of origin
		//				3	Z of origin
		//
		//		dA		Radians.
		//
		void			RotateByOrg ( int Axis, double dA );



	//	static int		nNews;

		/*
		void * operator	new ( size_t );

		void operator	delete ( void * ptr );
		*/


		dVector			n();	//	Returns the n column.

		dVector			o();	//	Returns the o column.

		dVector			a();	//	Returns the a column.

		dVector			p();	//	Returns the p column.


		//	2009-Feb-27
		//
		//	Set an 'A' matrix according to Richard P. Paul's suggestion.
		//
		//	See pg 53 of Paul's "Robot Manipulators  Mathmatics, Programming
		//	and Control".  
		//
		//	Theta and Alpha are expected to be in radians.
		//
		void	SetPaulA ( double Theta, 
						   double Dist, 
						   double Length, 
						   double Alpha );


		void	GetFloats ( float F[] );




	protected:

		double			M[4][4];


	private:

		int				iuMax ( double v1, double v2, double v3 ) const;


};	//	class	CM4



#endif

//	m4.h
