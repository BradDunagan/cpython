
#include	<stdio.h>

#include	"abb_rr_genrobot01.h"

#include	"abb_rr_libarg.h"

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

	return pD->ConfigureLink ( pCB, iJ, EId, bRot, bKin, Theta,  Dist, 
														 Length, Alpha );

}	//	ConfigureLink()


/*--------------------------------  MovTo()  -------------------------------*/
/*

PORT Ret *	MovTo ( PECB * pCB, CData * pObj, int * nArgs, LibArg * pArgs )
{
	int	ENo;	const char *	sW = F("MovTo()");

	CDGenRobot01 *	pO = 0;

	if ( pObj )  
	{
		pO = CheckObj ( sW, pObj, nArgs ); 
		
		if ( pO == 0 ) return 0;
	}

	MovToRtn	Rtn = mtrOK;		double	PX, PY, PZ, AX, AY, AZ;

	strcpy ( ErrStr, "no error" );

	if ( *nArgs >= 1 )
	{
		//	First arg a double?
		//
		if ( ArgOneOf ( &pArgs[0], 1, atDOUBLE ) )
		{
			//	Move the current object to the position specified by
			//	6 doubles.
			//
			if ( CheckArgs ( nArgs, pArgs,
							 "MovTo", 6, atDOUBLE, atDOUBLE, atDOUBLE,
										 atDOUBLE, atDOUBLE, atDOUBLE ) == 0 )
			{
				if ( pObj == 0 )		//	If just checking things.
				{ 
					*nArgs = 0;		return 0;
				}

				PX = *(double *)pArgs[0].pArg;
				PY = *(double *)pArgs[1].pArg;
				PZ = *(double *)pArgs[2].pArg;
				AX = *(double *)pArgs[3].pArg;	//	Angles in degrees.
				AY = *(double *)pArgs[4].pArg;
				AZ = *(double *)pArgs[5].pArg;

				CM4	P ( PX, PY, PZ, AX * DEG2PI, 
									AY * DEG2PI, 
									AZ * DEG2PI );

				//	if ( (ENo = pO->MovRbtTo ( &P )) != 0 )
				//
				//	Check for LAR_ return.
				//
				ENo = pO->MovRbtTo ( pCB, &P, Rtn );

				if ( ENo && ((ENo & LAR_NOT_AN_ENO) == 0) )
				{
				//	DelE ( T ( ENo, sW, "Deleting error.  Returning error "
				//						"indicator to RSP." ) );
				//	*nArgs = -1;
				//
				//	Error ( "MovTo:  Could not perform move.  "
				//			"See diagnostics." );
				//
				//	//	2011-May-07
					//
					*nArgs = -1;


					int	AE = GetAE ( ENo );		if ( AE == AE_INT_ERROR )
					{
						DelE ( T ( ENo, sW, "Deleting error.  Returning error "
											"indicator to RSP." ) );
					
						Error ( "MovTo:  Could not perform move.  "
								"See diagnostics." );
					}
					else
					{
						char	Msg [256];		int	R;

						if ( (R = GetE ( ENo, AE, Msg, sizeof(Msg) )) >= 0 )
						{
							Error ( "MovTo:  %s", Msg );
						}
						else
							DelE ( ENo );
					}
				}
				else
					*nArgs = ENo;		//	May be LAR_IN_PROGRESS
			}
		}
		else
		//	First arg must be a m4.
		//
		if ( ArgOneOf ( &pArgs[0], 1, atM4 ) )
		{
			//	If that is the only arg then move the current object to
			//	that positon.
			//
			if ( *nArgs == 1 )
			{
				if ( CheckArgs ( nArgs, pArgs, "MovTo", 1, atM4 ) == 0 )
				{
					if ( pObj == 0 )		//	If just checking things.
					{ 
						*nArgs = 0;		return 0;
					}


					CM4 *	pM = (CM4 *)pArgs[0].pArg;

					//	if ( (ENo = pO->MovRbtTo ( pM )) != 0 )
					//
					//	Check for LAR_ return.
					//
					ENo = pO->MovRbtTo ( pCB, pM, Rtn );

					if ( ENo && ((ENo & LAR_NOT_AN_ENO) == 0) )
					{
				//		DelE ( T ( ENo, sW, "Deleting error.  Returning error "
				//							"indicator to RSP." ) );
				//		*nArgs = -1;
				//
				//		Error ( "MovTo:  Could not perform move.  "
				//				"See diagnostics." );
				//
				//	//	2011-May-07
					//
					*nArgs = -1;

					int	AE = GetAE ( ENo );		if ( AE == AE_INT_ERROR )
					{
						DelE ( T ( ENo, sW, "Deleting error.  Returning error "
											"indicator to RSP." ) );

						Error ( "MovTo:  Could not perform move.  "
								"See diagnostics." );
					}
					else
					{
						char	Msg [256];		int	R;

						if ( (R = GetE ( ENo, AE, Msg, sizeof(Msg) )) >= 0 )
						{
							Error ( "MovTo:  %s", Msg );
						}
						else
							DelE ( ENo );
					}
					}
					else
						*nArgs = ENo;		//	May be LAR_IN_PROGRESS
				}
			}
			else
			{	//	Otherwise we are moving the m4 of the first arg to 
				//	a new position.  The remaining arguments should be either
				//	another m4 or 6 doubles.
				//	
				if ( ArgOneOf ( &pArgs[1], 1, atDOUBLE ) )
				{
					//	Move the first m4 argument to the position specified 
					//	by 6 doubles.
					//
					if ( CheckArgs ( nArgs, pArgs, "MovTo", 7, 
									 atM4,
									 atDOUBLE, atDOUBLE, atDOUBLE,
									 atDOUBLE, atDOUBLE, atDOUBLE ) == 0 )
					{
						if ( pObj == 0 )		//	If just checking things.
						{ 
							*nArgs = 0;		return 0;
						}


						CM4 *	pM = (CM4 *)pArgs[0].pArg;

						PX = *(double *)pArgs[1].pArg;
						PY = *(double *)pArgs[2].pArg;
						PZ = *(double *)pArgs[3].pArg;
						AX = *(double *)pArgs[4].pArg;	//	Angles in degrees.
						AY = *(double *)pArgs[5].pArg;
						AZ = *(double *)pArgs[6].pArg;

						pM->SetXYZRPY_Hosed ( PX, PY, PZ, AX * DEG2PI,
														  AY * DEG2PI,
														  AZ * DEG2PI );
						*nArgs = 0;
					}
				}
				else
				if ( ArgOneOf ( &pArgs[1], 1, atM4 ) )
				{
					if ( CheckArgs ( nArgs, pArgs, "MovTo", 2, 
									 atM4, atM4 ) == 0 )
					{
						if ( pObj == 0 )		//	If just checking things.
						{ 
							*nArgs = 0;		return 0;
						}


						CM4 *	pM1 = (CM4 *)pArgs[0].pArg;

						CM4 *	pM2 = (CM4 *)pArgs[1].pArg;

						*pM1 = *pM2;

						*nArgs = 0;
					}
				}
				else
				{
					*nArgs = -1;

					Error ( "MovTo: After a m4, expected another "
							"m4 or 6 floats." );
				}
			}
		}
		else
		{
			*nArgs = -1;

			Error ( "MovTo: Expected first argument to be a float or m4." );
		}
	}
	else
	{
		*nArgs = -1;

		Error ( "MovTo: Expected m4 or 6 floats." );
	}

	Ret *	pRet = new Ret ( rtINT );		pRet->r.i = (int)Rtn;

	return pRet;

}	//	MovTo()
*/

