
#include	<stdio.h>

#include	"abb_rr_genrobot01.h"

#include	"abb_rr_libarg.h"

#include	"abb_rr_m4.h"

#include	"abb_rr_error.h"

#define	PORT


#define	F(f)	"(" __FILE__ ")" "  " f


extern "C"
{
//	int		RR_Test ( const char * s, int i );


//	Identifies the robot - by name - which other robot calls are applied to.
//
//	Prompts for entities that represent the robot base, links, fingers and
//	grip origin.
//
//	DLL Args -
//
//		char *			Robot name.
//
//	int		IdRbt ( PECB * pCB, const char * Name );


//	2009-Feb-27		To set joint configuration.
//
//	Arguments
//
//		int			Joint index.  From 0 through 9.  In this case the
//					base does not count.  So, the index of the first link
//					off the base is 0.
//
//		int			Entity Id of the link.
//
//		int			Boolean:  true (not 0) to specify the joint as being
//					rotary.  Otherwise the movement will be a translation.
//
//		int			Boolean:  true if the joint's value is to be solved with
//					an inverse kinematics routine.  Otherwise the joint value
//					is a non-variable during the inverse kinematics run.
//
//		double		Paul's A matrix theta.  Degrees.
//					Rotation about parent link's Z axis.
//
//		double		Paul's A matrix distance.
//					Translation along parent link's Z axis.
//
//		double		Paul's A matrix length.
//					Translation along parent link's X axis.
//
//		double		Paul's A matrix alpha.  Degrees.
//					Rotation about parent link's X axis.
//
//	Returns
//
//		void
//
//	int		ConfigureLink ( PECB *		pCB,
//							CData *		pObj, 
//							int *		nArgs, 
//							LibArg *	pArgs );


//	Move the robot's gripper to the specified position WRT the system
//	coords.
//
//	DLL Args -
//
//		m4			Position to move to WRT the system.
//
//	Return -
//
//		int			0		Move performed ok.
//
//					1		Joint limit.
/*
PORT Ret *	MovTo ( PECB * pCB, CData * pObj, int * nArgs, LibArg * pArgs );
*/
}


int		RR_Test ( const char * s, int i )
{
	const char * sW = "RR_Test()";

	E ( sW, "not really an error" );

	printf ( "%s %d\n", s, i );

	return 0;

}	//	RR_Test()


static CDGenRobot01 *	pD = 0;

/*--------------------------------  IdRbt()  -------------------------------*/
/*
*/
int		IdRbt ( PECB * pCB, const char * Name )
{
	int	ENo;	const char *	sW = F("IdRbt()");

	if ( ! pD ) {
		pD = new CDGenRobot01(); }

	if ( (ENo = pD->IdRbt ( Name )) != 0 )
	{
		DelE ( T ( ENo, sW, "Deleting error.  Returing error indicator to "
							"RSP." ) );
	}

	return 0;

}	//	IdRbt()


/*---------------------------  ConfigureLink()  -----------------------------*/
/*
	See declaration above.
*/
int		ConfigureLink ( PECB *		pCB, 
						int			iJ,
						int			EId,
						int 		bRot,
						int			bKin,
						double		Theta,
						double		Dist,
						double		Length,
						double		Alpha )
{
	if ( ! pD ) {
		pD = new CDGenRobot01(); }

	return pD->ConfigureLink ( pCB, iJ, EId, bRot, bKin, Theta * DEG2PI,  
														 Dist, 
														 Length, 
														 Alpha * DEG2PI );

}	//	ConfigureLink()


/*--------------------------------  MovTo()  -------------------------------*/
/*
*/ 
int		MovTo ( PECB * pCB, P6 * 	pB6, 	//	Base WRT system
					//	    P6 * 	pL6, 	//	Last link WRT system
						    P6 * 	pPs ) 	//	Last link target WRT system
{
	int	ENo;	const char *	sW = F("MovTo()");

	if ( ! pD ) {
		pD = new CDGenRobot01(); }

	MovToRtn	Rtn = mtrOK;

//	strcpy ( ErrStr, "no error" );

	//	Base WRT system.
	P6	Bs;		Bs.Px = pB6->Px;	Bs.Py = pB6->Py;	Bs.Pz = pB6->Pz;
				Bs.Ax = pB6->Ax * DEG2PI;
				Bs.Ay = pB6->Ay * DEG2PI;
				Bs.Az = pB6->Az * DEG2PI;

	//	Target WRT system.
	CM4	P ( pPs->Px, pPs->Py, pPs->Pz, pPs->Ax * DEG2PI, 
									   pPs->Ay * DEG2PI, 
		 							   pPs->Az * DEG2PI );

	//	Check for LAR_ return.
	//
//	ENo = pD->MovRbtTo ( pCB, pB6, pL6, &P, Rtn );
//	ENo = pD->MovRbtTo ( pCB, pB6,      &P, Rtn );
	ENo = pD->MovRbtTo ( pCB, &Bs,      &P, Rtn );

	if ( ENo && ((ENo & LAR_NOT_AN_ENO) == 0) )
	{
		//	2011-May-07
		//
//		*nArgs = -1;

		int	AE = GetAE ( ENo );		if ( AE == AE_INT_ERROR )
		{
			DelE ( T ( ENo, sW, "Deleting error.  Returning error "
								"indicator to RSP." ) );
					
//			Error ( "MovTo:  Could not perform move.  "
//					"See diagnostics." );
		}
		else
		{
//			char	Msg [256];		int	R;
//
//			if ( (R = GetE ( ENo, AE, Msg, sizeof(Msg) )) >= 0 )
//			{
//				Error ( "MovTo:  %s", Msg );
//			}
//			else
				DelE ( ENo );
		}
	}
//	else
//		*nArgs = ENo;		//	May be LAR_IN_PROGRESS

	return (int)Rtn;

}	//	MovTo()

