/*
		 1         2         3         4         5         6         7		   8
12345678901234567890123456789012345678901234567890123456789012345678901234567890

	File:	GenRobot01.cpp


	Kind of a template for getting at the record.

	const char *	sW = F("CDGenRobot01::Function()");

	uint	nB;		BYTE *	pD = 0;		SRGenRobot01_v01 *	pR = 0;

	try
	{
		pD = Get ( nB );

		pR = (SRGenRobot01_v01 *)(pD + DF_NUM_HDR_BYTES
									 + NUM_RELEASE_VERSION_BYTES);


		pD = 0;		Put();
	}
	catch ( int ENoC )
	{
		if ( pD )  PutC();
	
		throw T ( ENoC, sW, "Throwing up." );
	}
*/

/*----------------------------- Dependencies  -----------------------------*/

#include	<stdlib.h>

#include	<stdarg.h>

#include	<memory.h>

#include	"abb_rr_genrobot01.h"

#include	"abb_rr_rgenrobot01.h"

#include    "abb_rr_world.h"

#include	"abb_rr_diags.h"

#include	"abb_rr_error.h"

//	End Dependencies



//	A macro for specifying filename and function name in diagnostics calls.
//
#define	F(f)	"(" __FILE__ ")" "  " f



extern "C"
{
PORT int Test_InvKin ( const double Tgt[], double JCur[], double JNew[] );

PORT int InvKin ( const double Tgt[], double JCur[], 
                                      double JNew[][2], double MaxDelta, 
                                      int     Odr[] );
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                GenRobot01                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/



//	Strings that specify which entity is being worked on.
//
/*
static const char *	ENT_BASE =			"Base";
static const char *	ENT_LINK0 =			"Link[0]";
static const char *	ENT_LINK1 =			"Link[1]";
static const char *	ENT_LINK2 =			"Link[2]";
static const char *	ENT_LINK3 =			"Link[3]";
static const char *	ENT_LINK4 =			"Link[4]";
static const char *	ENT_LINK5 =			"Link[5]";
static const char *	ENT_LINK6 =			"Link[6]";
static const char *	ENT_FINGER1	=		"Finger1";
static const char *	ENT_FINGER2	=		"Finger2";
static const char *	ENT_GRIP_ORG =		"GripOrg";

static const char *	ENT_GRSP_ENT =		"GrspEnt";
*/


static void	InProgress ( PECB * pCB )
{
	int ENo = 0;	const char * sW = F("InProgress()");

	//	Give the app a chance to process messages.

	pCB->iP  = iP_NULL;

	pCB->Cmd = PECB_IN_PROGRESS;

	if ( (ENo = pCB->CallBack ( pCB )) != 0 )  
	{
		DelE ( T ( ENo, sW, "Deleting error." ) );
	}

}	//	InProgress()


/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              CDGenRobot01                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/


CDGenRobot01::CDGenRobot01()
{
	//	Initialize members of this instance.
	//
	ZeroEverything();

};	//	CDGenRobot01::CDGenRobot01()


/*---------------------  CDGenRobot01::~CDGenRobot01()  --------------------*/
/*
*/
CDGenRobot01::~CDGenRobot01 ( void )
{
	int ENo;	const char * sW = F("CDGenRobot01::~CDGenRobot01()");

	if ( (ENo = CleanUp ( sW )) != 0 )
	{
		DelE ( T ( ENo, sW, "Deleting error." ) );
	}

};	//	CDGenRobot01::~CDGenRobot01()


/*-----------------------  CDGenRobot01::Print()  --------------------------*/
/*
*/
void	CDGenRobot01::Print (		int		Level, 
							  const char *	sW, 
							  const char *	Fmt, ... )
{
	if ( ! bDiagsOn )  return;

//	if ( Level < DL )  return;

	va_list		alist;

	char		Buf [256];

	va_start ( alist, Fmt );

	_vsnprintf ( Buf, sizeof(Buf) - 1, Fmt, alist );

	va_end ( alist );

	Buf[sizeof(Buf) - 1] = '\0';

//	::Print ( Level, sW, Buf );
	::Print (    DA, sW, Buf );

}	//	CDGenRobot01::Print()


/*-----------------------  CDGenRobot01::PrintM()  -------------------------*/
/*
*/
void	CDGenRobot01::PrintM ( const char *	sW,
							   const CM4 &	M,
							   const char *	Desc, ... )
{
	va_list		alist;

	char		Buf [256];

	va_start ( alist, Desc );

	_vsnprintf ( Buf, sizeof(Buf) - 1, Desc, alist );

	va_end ( alist );

	Buf[sizeof(Buf) - 1] = '\0';

	uint	i;

	for ( i = 0; i < 4; i++ )
	{
		const double *	Row = M.GetM() + (i * 4);

		Print ( DL, sW, "%s %12.6f %12.6f %12.6f %12.6f",
						Buf, Row[0], Row[1], Row[2], Row[3] );
	}

}	//	CDGenRobot01::PrintM()


/*-----------------------  CDGenRobot01::IdRbt()  --------------------------*/
/*
*/
int		CDGenRobot01::IdRbt ( const char * RobotName )
{
	const char *	sW = F("CDGenRobot01::IdRbt()");

	Print ( DA, sW, "The robot's name is: %s  What must be done here?",
		    RobotName );

	return 0;

}	//	CDGenRobot01::IdRbt()


/*-------------------  CDGenRobot01::ConfigureLink()  -----------------------*/
/*
*/
int		CDGenRobot01::ConfigureLink ( PECB * pCB,
									  int  iJ,   LID  EId,
									  BOOL bRot, BOOL bKin, double Theta,
															double Dist,
															double Length,
															double Alpha )
{
	int	ENo = 0; const char * sW = F("CDGenRobot01::ConfigureLink()");

	SRGenRobot01_vXX * pR = 0;

	try
	{
		pR = Get();

		if ( iJ <= pR->nJoints )		//	2020
			pR->nJoints = iJ + 1;		//	

		if ( (iJ < 0) || (iJ >= GR01_MAX_JOINTS) )
		{
			throw M ( AE_RSP_INVALID_JOINT_INDEX, sW,
					  "Invalid joint index (%d).  Must be %d <= iJ < %d.",
					  iJ, 0, GR01_MAX_JOINTS );
		}

		pR->LinkId[iJ] = EId;

		if ( bRot )
		{
			if ( bKin )  pR->JType[iJ] = jtRotating;
			else
				pR->JType[iJ] = jtRotatingNIK;
		}
		else
		{
			if ( bKin )  pR->JType[iJ] = jtTranslating;
			else
				pR->JType[iJ] = jtTranslatingNIK;
		}

		pR->Theta[iJ]  = Theta;
		pR->Dist[iJ]   = Dist;
		pR->Length[iJ] = Length;
		pR->Alpha[iJ]  = Alpha;

		//	Positions the link.
		//
		if ( (ENo = SetPaulJoint ( pCB, iJ, pR )) != 0 ) {
			DelE ( T ( ENo, sW, "Deleting error." ) ); }

		pR = 0;		Put();
	}
	catch ( int ENoC )
	{
		if ( pR )  PutC();
	
		return T ( ENoC, sW, "Returning error." );
	}

	return ENo;

}	//	CDGenRobot01::ConfigureLink()


/*----------------------  CDGenRobot01::MovRbtTo()  --------------------------*/
/*
*/
int		CDGenRobot01::MovRbtTo ( PECB * 	pCB, 
								 P6 * 		pB6, 	//	Base WRT system
						//		 P6 * 		pL6, 	//	Last link WRT system
								 CM4 * 		pPs, 	//	Last link target WRT system
								 MovToRtn & Rtn )
{
#define	MAX_ITb		16

	int	ENo = 0;	const char * sW = F("CDGenRobot01::MovRbtTo()");

	DL = 4;

	int		iJk0 = -1;		//	First joint (of the arrary of joints) to be 
							//	inverse kinematically solved.
	int		iJkL = -1;		//	Last of such joints.

	int		iJ;

	double	JCur [GR01_MAX_JOINTS];		//	bool bVPsSyncd = false;

//	double	JIT [GR01_MAX_JOINTS];

	SRGenRobot01_vXX * pR = 0;

	SRGenRobot01_vXX	RBuf;		//	2010-Aug-12
	SRGenRobot01_vXX	ROrg;		//	2010-Aug-12		For reset.

	RbtMovCBData *	pCBD  = 0;		//	2011-Feb-21

	Rtn = mtrOK;

	try
	{
		pR = Get();

		//	2010-Aug-12		Long op.
		//
		//	With ORL, can't hold the lock.  So, buffer and indicate "busy".
		//
		memcpy ( &RBuf, pR, sizeof(SRGenRobot01_vXX) );
		memcpy ( &ROrg, pR, sizeof(SRGenRobot01_vXX) );

		pR->bBusy = TRUE;

		sprintf ( pR->BusyMsg, "PE %d is moving the robot.", pCB->PE_Id );

		pR = 0;		Put();

		pR = &RBuf;


		if ( pR->K  ==  kSymKin )
		{
			if ( pPath == 0 )  pPath = new MvPath ( 1000 );

			pPath->nS = 0;

		//	pPath->B.GetRT ( pB6 );
			pPath->B.SetRT ( pB6 );

			printf ( "%s: pPath->B  x: %8.2f  y: %8.2f  z: %8.2f \n", sW,
					 pPath->B.GetX(), pPath->B.GetY(), pPath->B.GetZ() );

		//	pPath->nB = ~pW->GetS ( pR->BaseId );
			pPath->nB = ~pPath->B;
		}
		else
		{
			if ( pPath )  delete pPath;

			pPath = 0;

			printf ( "%s: no path\n", sW );
		}

		GetIKJoints ( pR, iJk0, iJkL );

		GetJoints2 ( pR, iJk0, iJkL - iJk0 + 1, JCur );


		double	CPx, CPy, CPz, CAx, CAy, CAz;
		
		//	Motion Planning
		//
		//		If the target is ~180 deg around the base-link1 axis
		//		then find intermediate target positions.
		//
		//	Get current and target position of last link WRT base.
		//
	//	CM4	Cb, Tb;		CM4	Ls = pW->GetS ( pR->LinkId[pR->nJoints - 1] );
	//	CM4	Cb, Tb;		CM4	Ls;	Ls.GetRT ( pL6 );
		CM4	Cb, Tb;		CM4	Ls = GetLastLinkS ( pR );
		
		Ls.GetXYZRPY_TPR ( CPx, CPy, CPz, CAx, CAy, CAz );

		Print (  3, sW, "Ls: %7.2f %7.2f %7.2f  %7.2f %7.2f %7.2f",
						CPx, CPy, CPz, CAx * PI2DEG,
									   CAy * PI2DEG,
									   CAz * PI2DEG );

		Get_Cb_Tb ( *pPs, Ls, pR, Cb, Tb );

		Cb.GetXYZRPY_TPR ( CPx, CPy, CPz, CAx, CAy, CAz );

		Print (  3, sW, "Cb: %7.2f %7.2f %7.2f  %7.2f %7.2f %7.2f",
						CPx, CPy, CPz, CAx * PI2DEG,
									   CAy * PI2DEG,
									   CAz * PI2DEG );

		printf ( "%s: Tb x: %8.2f  y: %8.2f  z: %8.2f\n", sW,
				  Tb.GetX(), Tb.GetY(), Tb.GetZ() );

		CM4	ITb[MAX_ITb];	int i, nITb = 0;		//	Intermediate targets.

		//	The base-link1 axis is the Z axis of the base (generally, links
		//	(in this case, link1) translate/rotate along/around the previous
		//	link's (in this case, the base) Z axis).
		//
		//	So we can work in the X-Y plane of the base coordinate frame.
		//
		double	Xc = Cb.GetX(), Yc = Cb.GetY();
		
		double	Ac = atan2 ( Yc, Xc );	//	Angle of current WRT base Z.
		
		double	Xt = Tb.GetX(), Yt = Tb.GetY();

		double	At = atan2 ( Yt, Xt );	//	Angle of target WRT base Z.

		//	Are the target and current more than 80% of PI apart?
		//
		double	da = DeltaAngle ( Ac, At );

		if ( fabs ( da ) > (PI * 0.80) )
		{
			CM4	ICb = Cb;

			//	Move closer to the base.
			//
			double r = sqrt ( (Xc * Xc) + (Yc * Yc) );

			if ( r > (pR->Reach * 0.30) )
			{
				double	Xc2 = (Xc * 0.30 * pR->Reach) / r;
				double	Yc2 = (Yc * 0.30 * pR->Reach) / r;

				double	X, Y, Z, Ax, Ay, Az;

				ICb.GetXYZRPY_TPR ( X, Y, Z, Ax, Ay, Az );

				ICb.SetXYZRPY_TPR ( Xc2, Yc2, Z, Ax, Ay, Az );

				ITb[nITb++] = ICb;
			}

			//	Rotate around the base.
			//
			int		nR = (int)((fabs ( da ) - (PI * 0.45)) / (PI * 0.10));

			double	dR = PI * 0.10;		if ( da > 0 )  dR = -dR;

			_ASSERTE ( nR < (MAX_ITb - nITb - 1) );

			CM4	R ( 0, 0, 0, 0, 0, dR );

			for ( i = 0; i < nR; i++ )
			{
				ICb = R * ICb;

				ITb[nITb++] = ICb;
			}
		}

		ITb[nITb++] = Tb;		//	Put the final target in the Intermediate
								//	Target array just to make the loop easier.


	//	GetJoints2 ( pR, iJk0, iJkL - iJk0 + 1, JIT );

		int	nStartOvers = 0;		bool bStartOver = false;

		int	iJointLimit = -1;	//	int	nJointLimits = 0;

		for ( i = 0; (ENo == 0) && (i < nITb); )
		{
			bool b180Adj = false;

			Tb = ITb[i];

			if ( pPath )
			{
				if ( iJointLimit >= 0 )		//	2011-Apr-08
				{
				//	if ( ++nJointLimits < 2 )
				//	{
				//		//	Try a different solution, for the first step of the
				//		//	move, for that joint.
				//		//
				//		MvSln * pS = pPath->pS;
				//	
				//		for ( int iS = 0; iS < pPath->nS; iS++ )
				//		{
				//		//	int	O = pS[iS].Odr[iJointLimit];
				//		//
				//		//	if ( O_LA(O) )  O = (O & 0xFFFFFFF0) | oLB;
				//		//	else
				//		//		O = (O & 0xFFFFFFF0) | oLA;
				//		//
				//		//	pS[iS].Odr[iJointLimit] = O;
				//		//	//
				//			for ( iJ = 0; iJ < 7; iJ++ )
				//			{
				//				int	O = pS[iS].Odr[iJ];
				//	
				//				if ( O_LA(O) )  O = (O & 0xFFFFFFF0) | oLB;
				//				else
				//					O = (O & 0xFFFFFFF0) | oLA;
				//	
				//				pS[iS].Odr[iJ] = O;
				//			}
				//		}
				//	
				//		iJointLimit = -1;
				//	}
				//	else
				//	{
						//	2011-May-11		Let the app handle it.
						//
						Rtn = mtrJointLimit;	//	Joint limit indicator.
					
						break;
				//	}
				}
				else
					pPath->ResetOdr();

				pPath->nS = 0;

			//	pPath->Es = pW->GetS ( pR->LinkId[pR->nJoints - 1] );
			//	pPath->Es.GetRT ( pL6 );
				pPath->Es = GetLastLinkS ( pR );

			//	pPath->Sb.Set ( Cb.GetX(), Cb.GetY(), Cb.GetZ() );

				//	Remember starting distance from target for sanity checks
				//	as we move.
				//
				dVector	v ( Cb.GetX() - Tb.GetX(), 
						    Cb.GetY() - Tb.GetY(), 
							Cb.GetZ() - Tb.GetZ() );

				pPath->DistT0 = v.Mag();

			//	ResetJoints ( pCB, &ROrg, iJk0, iJkL - iJk0 + 1, JIT, FALSE );

				GetJoints2 ( pR, iJk0, iJkL - iJk0 + 1, pPath->JCur );
			}

			bStartOver = false;

			StepCnt = 0;

			while ( 1 )	
			{
			//	if ( (ENo  = FIP_MoveTo_Setup ( iJk0, 
			//									iJkL - iJk0 + 1,   *pPs, pR )) 
				if ( (ENo  = FIP_MoveTo_Setup ( iJk0, 
												iJkL - iJk0 + 1, b180Adj,
												Cb, Tb, pR )) 
						  != 0 )
				{
					throw ENo;
				}

				if ( pPath == 0 )  break;

				if ( ((int)pR->JType[iJkL] & (int)jtRotating) == 0 )  break;


				//	2009-Dec-25		Kind of a kludge.
				//
				//	See FIP_MoveTo_Setup().  It seems that when Tc's DAx and 
				//	DAz are 180 (indicated by b180Adj being true) that at times
				//	there is a singularity.
				//
				if ( ! b180Adj )  break;

				//	Try rotating the last link 180 (in 5 deg steps) before 
				//	moving.
				//
				double	d = 5.0 * DEG2PI;		int i, iJ;

				if ( pR->Theta[iJkL] > 0 )  d = -d;

				for ( i = 0; i < 36; i++ )
				{
					MvSln * pS = &pPath->pS[pPath->nS++];

					for ( iJ = iJk0; iJ < iJkL; iJ++ )
					{
						if ( (int)pR->JType[iJ] & (int)jtRotating )  
							pS->J[iJ - iJk0] = pR->Theta[iJ];
						else
							pS->J[iJ - iJk0] = pR->Dist[iJ];
					}

					pS->J[iJkL - iJk0] = (pR->Theta[iJkL] += d);
				}

				//	System position of the last link.
				//
			//	pPath->Es = pW->GetS ( pR->LinkId[iJkL - 1] );
			//	pPath->Es.GetRT ( pL6 );
				pPath->Es = GetLastLinkS ( pR );

				CM4	A;	A.SetPaulA ( pR->Theta[iJkL], 
									 pR->Dist[iJkL],
									 pR->Length[iJkL],
									 pR->Alpha[iJkL] );

				pPath->Es = pPath->Es * A;

				//	New Cb.  And setup again to get Tc.
				//
				Get_Cb_Tb ( *pPs, pPath->Es, pR, Cb, Tb );

				if ( (ENo  = FIP_MoveTo_Setup ( iJk0, 
												iJkL - iJk0 + 1, b180Adj,
												Cb, Tb, pR )) 
						  != 0 )
				{
					throw ENo;
				}

				break;
			}

			//	Do the first step.
			//
		//	if ( FVs.iStep < FVs.nSteps )
		//	{
		//		ENo = FVs.Step ( *this, pR );
		//	}
		//	else
		//		ENo = FVs.Finish ( *this, pR );
		//	//
			//	2009-Jun-19		Do a callback if necessary.
			//
			//	Instead of the LAR_ and "continue" stuff.
			//
			ENo = GetMovePath ( pCB, pR, iJk0, iJkL, Tb, Rtn,
														 nStartOvers,
													     bStartOver, 
														 iJointLimit );

			if ( Rtn == mtrInvKin )  break;

			if ( iJointLimit >= 0 )  continue;

			if ( bStartOver )  continue;

			i++;		Cb = Tb;		nStartOvers = 0;
		
		//	GetJoints2 ( pR, iJk0, iJkL - iJk0 + 1, JIT );

			if ( pPath )
			{
				//	Now all the joint solutions for this segment of the move,
				//	do the move.

			//	MoveOnPath ( pCB, pR, iJk0, iJkL );
				//	Just send joint values.
				MoveOnPath_j ( pCB, pR, iJk0, iJkL );
				
				//	It might be important to let diagnostics flush so that
				//	the cmd record does not get to the next mov command before
				//	the viewport(s) finishes their updates from this move.
				//
			//	if ( bDiagsOn )
			//		Sleep ( 500 );

			}	//	if ( pPath )

		}	//	for ( ITb[] )


		if ( ENo && ((ENo & LAR_NOT_AN_ENO) == 0) )  throw ENo;


		//	2010-Aug-12		Unbuffer.
		//
		pR = Get();

		memcpy ( pR, &RBuf, sizeof(RBuf) );

		pR = 0;		Put();			//	Changed.  Unlock.
	}
	catch ( int ENoC )
	{
		if ( pR )
		{
			if ( pR != &RBuf )  PutC();
			else
			{
				pR = Get();

				memcpy ( pR, &ROrg, sizeof(RBuf) );

				pR = 0;		Put();
			}
		}


		if ( (iJk0 >= 0) && (iJkL >= 0) )
		{
			ResetJoints ( pCB, &ROrg, iJk0, iJkL - iJk0 + 1, JCur, TRUE );
		}

	//	if ( bVPsSyncd )
	//	{
	//	//	pCB->iP  = iP_NULL;		pCB->Cmd = PECB_RELEASE_VPS;
	//	//
	//	//	//	2011-Jan-17
	//		//
	//		pCB->iP = iP_NULL;		pCB->Cmd   = PECB_SYNC_VPS2;
	//								pCB->Flags = PECBF_SYNCH_PES_UNREGISTER;
	//
	//		if ( (ENo = pCB->CallBack ( pCB )) != 0 )  DelE ( ENo );
	//
	//		bVPsSyncd = false;
	//	}
	//
	//	//	2011-Jan-18
		//
		if ( pCBD )  { delete pCBD;		pCBD = 0; }

		FVs.bMovingArm = FALSE;

		ENo = T ( ENoC, sW );
	}

//	delete [] pWEA;
//
//	2011-Jan-18

	if (    ((ENo == 0) || (ENo & LAR_NOT_AN_ENO))
		 && 
			(iJk0 >= 0) && (iJkL >= 0) )
	{
		for ( iJ = 0; iJ < iJkL - iJk0 + 1; iJ++ )
		{
			FVs.JStart[iJ] = JCur[iJ];
		}
	}

	return ENo;

#undef	MAX_ITb

}	//	CDGenRobot01::MovRbtTo()


/*--------------------  CDGenRobot01::SetPaulJoint()  -----------------------*/
/*
*/
int		CDGenRobot01::SetPaulJoint ( PECB *				pCB,
									 int				iJ, 
									 SRGenRobot01_vXX * pR )
{
	int ENo = 0;	const char * sW = F("CDGenRobot01::SetPaulJoint()");

	WorldEntPaul *	pWEP = 0;	int iWEP;

	RbtMovCBData * pCBD = CreateCBD ( pCB, 0, 1 );

	pWEP = pCBD->pWEPV->pEnts;      iWEP = 0;

	pWEP[iWEP].Theta  = pR->Theta[iJ];
	pWEP[iWEP].Dist   = pR->Dist[iJ];
	pWEP[iWEP].Length = pR->Length[iJ];
	pWEP[iWEP].Alpha  = pR->Alpha[iJ];

	pWEP[iWEP].EId = pR->LinkId[iJ];

	iWEP++;

	pCBD->pWEPV->nEnts = iWEP;

	pCB->iP = iP_NULL;		pCB->Cmd = PECB_SYNC_VPS3;
							pCB->p   = pCBD;

	ENo = pCB->CallBack ( pCB );

	delete pCBD;

	if ( ENo ) {
		return T ( ENo, sW ); }
		
	return 0;

}	//	CDGenRobot01::SetPaulJoint()


/*--------------------  CDGenRobot01::GetIKJoints()  ------------------------*/
/*
	Inverse Kinematic joints.
*/
void	CDGenRobot01::GetIKJoints ( SRGenRobot01_vXX * pR, int & iJk0, 
														   int & iJkL )
{
	const char * sW = F("CDGenRobot01::GetIKJoints()");

	int	nJk = 0;		//	Total number of inverse kinematically solvable
						//	joints.  Must be == 6.
	int iJ;

	for ( iJk0 = 0; iJk0 < pR->nJoints; iJk0++ )
	{
		if (    (pR->JType[iJk0] != jtUndefined)
			 && 
				((pR->JType[iJk0] & jtNoInvKin) == 0) )  break;
	}

//	if ( iJk0 + 7 > pR->nJoints )					2020
	if ( iJk0 + pR->nJoints > pR->nJoints )
	{
		iJk0 = -1;

		throw M ( AE_RSP_INVALID_JOINT_TYPE, sW, 
				  "Joint type is not set." );
	}

	for ( iJ = iJk0; iJ < pR->nJoints; iJ++ )
	{
		if (    (pR->JType[iJ] != jtUndefined)
			 && 
				((pR->JType[iJ] & jtNoInvKin) == 0) )  
		{
			iJkL = iJ;		nJk++;
		}
	}

	if ( nJk != 6 )
	{
		throw M ( AE_RSP_INVALID_KINEMATICS, sW,
				  "There must be 6 kinematically solvable joints." );
	}

}	//	CDGenRobot01::GetIKJoints()


/*-------------------  CDGenRobot01::ResetJoints()  ------------------------*/
/*
	2002-May-28

	Lesser version of ResetJoints().

	No notifications are sent.

	Originally for setting back to the step when sub-stepping in MoveTo().
*/
void	CDGenRobot01::ResetJoints ( PECB *				pCB,
									SRGenRobot01_vXX *	pR,
									int iJ1, int nJ, double J[],
									BOOL				bNotify )
{
	const char * sW = F("CDGenRobot01::ResetJoints()");

	try
	{
		//	Set the links.
		//
		for ( int iJ = iJ1; iJ < iJ1 + nJ; iJ++ )
		{
			if ( pR->JType[iJ] & jtRotating )  
			{
				pR->Theta[iJ] = J[iJ - iJ1];

				SetPaulJoint ( pCB, iJ, pR );

				continue;
			}

			if ( pR->JType[iJ] & jtTranslating )  
			{
				pR->Dist[iJ] = J[iJ - iJ1];

				SetPaulJoint ( pCB, iJ, pR );

				continue;
			}

			throw M ( AE_RSP_INVALID_JOINT_TYPE, sW, 
					  "J%d (joint [%d]) type is not set.", iJ1 + iJ + 1, iJ );
		}
	}
	catch ( int ENoC )
	{
		DelE ( T ( ENoC, sW, "Failed reseting joints.  Deleting error." ) );
	}

	if ( bNotify )
	{
		pCB->Flags |= PECBF_NOTIFICATION_POSTED;
	}

}	//	CDGenRobot01::ResetJoints()


/*--------------------  CDGenRobot01::DeltaAngle()  ------------------------*/
/*
	Note that the difference in angles of -175 and +175 is 10.

	A1 and A2 are in radians in the range +/- PI.

	Return is in radians and is also in the range of +/- PI.
*/
double	CDGenRobot01::DeltaAngle ( double A1, double A2 )
{
	//	Convert both to a range of 360 (2PI).
	//
	if ( A1 < 0 )  A1 += 2 * PI;

	if ( A2 < 0 )  A2 += 2 * PI;

	double	D = A1 - A2;

	if ( D >  PI )  D -= 2 * PI;

	if ( D < -PI )  D += 2 * PI;

	return D;

}	//	CDGenRobot01::DeltaAngle()


/*-------------------  CDGenRobot01::AbsDeltaAngle()  ----------------------*/
/*
	Return is in radians and always positive.
*/
double	CDGenRobot01::AbsDeltaAngle ( double A1, double A2 )
{
	return fabs ( DeltaAngle ( A1, A2 ) );

}	//	CDGenRobot01::AbsDeltaAngle()


/*--------------------  CDGenRobot01::GetMovePath()  -----------------------*/
/*
*/
int		CDGenRobot01::GetMovePath ( PECB *				pCB, 
									SRGenRobot01_vXX *	pR, 
									int					iJk0, 
									int					iJkL,
									CM4 &		Tb,
									MovToRtn &	Rtn,
									int &		nStartOvers,
									bool &		bStartOver,
									int &		iJointLimit )  
{
	int ENo = 0;	//	const char * sW = F("CDGenRobot01::GetMovePath()");

	while ( FVs.iStep < FVs.nSteps )
	{
		iJointLimit = -1;

		ENo = FVs.Step ( pCB, *this, pR, Rtn, iJointLimit );

		if ( Rtn != mtrOK )
		{
			ResetJoints ( pCB, pR, iJk0, iJkL - iJk0 + 1, pPath->JCur,     
														  FALSE );
			return 0;
		}

		if ( ENo == LAR_IN_PROGRESS )
		{
			ENo = 0;

			LARInProgress ( pCB, pR, iJk0, iJkL, Tb, nStartOvers,
													 bStartOver );

			if ( bStartOver )  return 0;

			continue;
		}

		if ( ENo )  break;
	}

	if ( ENo == 0 )
	{
		ENo = FVs.Finish ( pCB, *this, pR, Rtn, iJointLimit );

		if ( Rtn != mtrOK )
		{
			ResetJoints ( pCB, pR, iJk0, iJkL - iJk0 + 1, pPath->JCur,     
														  FALSE );
			return 0;
		}

		while ( ENo == LAR_IN_PROGRESS )
		{
			ENo = 0;	
			
			if ( pPath == 0 )  InProgress ( pCB );

			if ( FVs.nSS == 0 )
			{
				if ( (ENo = FIP_MoveTo_UpdateSetup ( pR )) != 0 )  
				{
					throw ENo;
				}
			}

			if ( FVs.iStep < FVs.nSteps )
			{
				ENo = FVs.Step ( pCB, *this, pR, Rtn, iJointLimit );
			}
			else
				ENo = FVs.Finish ( pCB, *this, pR, Rtn, iJointLimit );

			if ( Rtn != mtrOK )
			{
				ResetJoints ( pCB, pR, iJk0, iJkL - iJk0 + 1, pPath->JCur,     
															  FALSE );
				return 0;
			}
		}
	}

	return ENo;

}	//	CDGenRobot01::GetMovePath()


/*--------------------  CDGenRobot01::ZeroEverything()  --------------------*/
/*
*/
void	CDGenRobot01::ZeroEverything()
{
	const char * sW = "CDGenRobot01::ZeroEverything()";

	pRD = 0;

	pPath = 0;

//	bDiagsOn = TRUE;		DL = D4;			StepCnt = 0;
	bDiagsOn = FALSE;		DL = D4;			StepCnt = 0;

	printf ( "%s:  bDiagsOn %d  DL %d\n", sW, bDiagsOn, DL );

	IKFunc = ::InvKin;

	bTryingSingularity = false;

 	nResidErrors = 0;

	nRecalls = 0;

	bSingularity = false;

	for ( int i = 0; i < GR01_MAX_JOINTS; i++ ) {
		Odr[i] = 0; }

}	//	CDGenRobot01::ZeroEverything()


/*----------------------  CDGenRobot01::CleanUp()  -------------------------*/
/*
*/
int		CDGenRobot01::CleanUp ( const char * sW )
{
//	int ENo = 0;

	if ( pRD ) {
		delete pRD; }

	if ( pPath ) {
		delete pPath; }
	
	ZeroEverything();

	return 0;

}	//	CDGenRobot01::CleanUp()


/*-------------------------  CDGenRobot01::Get()  --------------------------*/
/*
*/
SRGenRobot01_vXX *	CDGenRobot01::Get()
{
	if ( ! pRD ) {
		pRD = new SRGenRobot01_vXX(); 
		memset ( pRD, 0, sizeof(SRGenRobot01_vXX) ); 
	
		int	i;

		//	Initialize this record.
		//
		pRD->BaseId = 0;

		for ( i = 0; i < GR01_MAX_JOINTS; i++ )  
		{
			pRD->LinkId[i] = 0;		pRD->LinkA0[i].Reset();
		}

		pRD->Finger1Id = 0;		pRD->Finger1A0.Reset();
		pRD->Finger2Id = 0;		pRD->Finger2A0.Reset();
		pRD->GripOrgId = 0;		pRD->GripOrgA0.Reset();
		pRD->GrspEntId = 0;		pRD->GrspEntA0.Reset();

	//	pRD->nJoints = 7;
		pRD->nJoints = 0;		//	2020	Set in ConfigureLink().

		//	Paul parameters.
		//
		for ( i = 0; i < GR01_MAX_JOINTS; i++ )
		{
			pRD->JType[i]  = jtUndefined;

			pRD->Theta[i]  = 0;
			pRD->Length[i] = 0;
			pRD->Dist[i]   = 0;
			pRD->Alpha[i]  = 0;
		}

		pRD->Reach = 0;				//	Max distance from base to last link.

		//	Gripper
		//
		pRD->Gripper = 0;

		pRD->GAx = pRD->GAy = pRD->GAz = 0;

		//	Joint limits.
		//
		for ( i = 0; i < GR01_MAX_JOINTS; i++ ) 
		{
			pRD->JointLimit[i][0] = (-360.0 * DEG2PI);
			pRD->JointLimit[i][1] = ( 360.0 * DEG2PI);
		}

		pRD->GripperLimit[0] =  0.00;
		pRD->GripperLimit[1] =  4.00;


		pRD->K = kSymKin;			//	Kinematics function.

		FVs.Kinematics = &CDGenRobot01::KPaul03_RPPaul;

		memset ( pRD->IKModName, 0, sizeof(pRD->IKModName) );
	}

	return pRD;

}	//	CDGenRobot01::Get()


/*-------------------------  CDGenRobot01::Put()  --------------------------*/
/*
*/
void	CDGenRobot01::Put()
{

}	//	CDGenRobot01::Put()


/*-------------------------  CDGenRobot01::PutC()  --------------------------*/
/*
*/
void	CDGenRobot01::PutC()
{

}	//	CDGenRobot01::PutC()


/*---------------------  CDGenRobot01::GetJoints2()  ------------------------*/
/*
*/
void	CDGenRobot01::GetJoints2 ( SRGenRobot01_vXX * pR, 
								   int iJ1, int nJ, double J[] )
{
	const char * sW = F("CDGenRobot01::GetJoints2()");

	for ( int iJ = iJ1; iJ < iJ1 + nJ; iJ++ )
	{
		if ( pR->JType[iJ] & jtRotating )  J[iJ - iJ1] = pR->Theta[iJ];
		else
		if ( pR->JType[iJ] & jtTranslating )  J[iJ - iJ1] = pR->Dist[iJ];
		else
			throw M ( AE_RSP_INVALID_JOINT_TYPE, sW, 
					  "J%d (joint [%d]) type is not set.", iJ1 + iJ + 1, iJ );
	}

}	//	CDGenRobot01::GetJoints2()


RbtMovCBData *  CDGenRobot01::CreateCBD ( PECB * pCB, int MaxWEA, int MaxWEP ) 
{
	RbtMovCBData * pCBD = new RbtMovCBData();

	pCBD->PE_Id = pCB->PE_Id;

	if ( MaxWEA > 0 ) {
		pCBD->pWEAV = new WEAVector();   
		
		pCBD->pWEAV->pEnts = new WorldEntA [MaxWEA];
		pCBD->pWEAV->nEnts = 0;
	}

	if ( MaxWEP > 0 ) {
		pCBD->pWEPV = new WEPVector();   
		
		pCBD->pWEPV->pEnts = new WorldEntPaul [MaxWEP];
		pCBD->pWEPV->nEnts = 0;
	}
	
	return pCBD;

}   //  CDGenRobot01::CreateCBD()


RbtMovCBData_j *  CDGenRobot01::CreateCBD_j ( PECB * pCB, int MaxWEJ )
{
	RbtMovCBData_j * pCBD = new RbtMovCBData_j();

	pCBD->PE_Id = pCB->PE_Id;

	if ( MaxWEJ > 0 ) {
		pCBD->pWEJ = new WEJVector();   
		
		pCBD->pWEJ->pEnts = new WorldEntJoint [MaxWEJ];
		pCBD->pWEJ->nEnts = 0;
	}
	
	return pCBD;

}   //  CDGenRobot01::CreateCBD_j()


/*--------------------  CDGenRobot01::LARInProgress()  -----------------------*/
/*
*/
void	CDGenRobot01::LARInProgress ( PECB *				pCB, 
									  SRGenRobot01_vXX *	pR, 
									  int					iJk0, 
									  int					iJkL,
									  CM4 &			Tb,
									  int &			nStartOvers,
									  bool &		bStartOver    )  
{
	int ENo = 0;	const char * sW = F("CDGenRobot01::LARInProgress()");

	if ( pPath == 0 )  InProgress ( pCB );

	if ( FVs.nSS == 0 )
	{
		if ( (ENo = FIP_MoveTo_UpdateSetup ( pR )) != 0 )  
		{
			throw ENo;
		}
	}
	else
	{
		//	2009-Jul-14		Sanity check.
		//
		//	Moving farther from the goal?
		//
	//	if ( pPath && (FVs.iSS > 16) )		//	many substeps 
		if ( pPath && (FVs.iSS >  7) )		//	many substeps 
		{
		//	CM4 cb = pPath->nB * pPath->Es;
		//	//
		//	//	2011-Oct-01		Trying grasp coords as target.
			//
			//	See corresponding changes of this date in other places.
			//
			CM4	G;			//	Grasp coords WRT last link.

			{	DiA0 & A0 = pR->GripOrgA0;

				G.SetRT ( A0.X, A0.Y, A0.Z, A0.AX, A0.AY, A0.AZ );
			}

			CM4 cb = pPath->nB * (pPath->Es * G);

			dVector	v ( cb.GetX() - Tb.GetX(), 
						cb.GetY() - Tb.GetY(), 
						cb.GetZ() - Tb.GetZ() );

			double	d = v.Mag() - pPath->DistT0;

			if ( d > 5 )
			{
				nStartOvers++;		if ( nStartOvers >= 3 )
				{
					throw E ( sW, "Could not find solution." );
				}

				//	Start over.  Try a different Odr.
				//
				if ( Odr[0] == 0xa )  Odr[0] = 0xb;
				else
				if ( Odr[0] == 0xb )  Odr[0] = 0xa;
				else
					Odr[0] = 0xa;

				ResetJoints ( pCB, pR, iJk0, 
									   iJkL - iJk0 + 1, 
									   pPath->JCur, 
							  FALSE );

				bStartOver = true;
			}
		}
	}

}	//	CDGenRobot01::LARInProgress()


/*-----------------------  CDGenRobot01::MoveOnPath()  -----------------------*/
/*
*/
void	CDGenRobot01::MoveOnPath ( PECB *				pCB, 
								   SRGenRobot01_vXX *	pR, 
								   int					iJk0, 
								   int					iJkL )
{
#define	MAX_WEA		32
#define	MAX_WEP		32

	int ENo = 0;	const char * sW = F("CDGenRobot01::MoveOnPath()");

	int	iJ, iS;		BOOL bRotate[7];

	WorldEntA *		pWEA = 0;	int iWEA;
	WorldEntPaul *	pWEP = 0;	int iWEP;

	CM4				CamEntS, CamEntA;		bool bGraspingCamera = false;	

	for ( iJ = iJk0; iJ <= iJkL; iJ++ )
	{
		bRotate[iJ-iJk0] = (((int)pR->JType[iJ] & (int)jtRotating) != 0);
	}


	for ( iS = 0; iS < pPath->nS; iS++ )
	{
		MvSln * pS = &pPath->pS[iS];

	//	2011-Jan-14, 18
	//
	RbtMovCBData * pCBD = CreateCBD ( pCB, MAX_WEA, MAX_WEP );

	pWEA = pCBD->pWEAV->pEnts;      iWEA = 0;
	pWEP = pCBD->pWEPV->pEnts;		iWEP = 0;

	bGraspingCamera =	 (	 (pR->GrspEntId > 0) 
						  || (pR->GripOrgId > 0))
					  && (pR->GrspFlags & GRGF_CAMERA);

	if ( bGraspingCamera )  
	{
	//	CamEntS = pW->GetS ( pR->BaseId );
	//
	//	CamEntA = pW->GetA ( pR->GrspEntId );
		throw E ( sW, "Not ready for camera" );
	}


	for ( iJ = iJk0; iJ <= iJkL; iJ++, iWEA++, iWEP++ )
	{
		if ( bRotate[iJ - iJk0] )  pR->Theta[iJ] = pS->J[iJ - iJk0];
		else
			pR->Dist[iJ] = pS->J[iJ-iJk0];

		pWEA[iWEA].A.SetPaulA ( pR->Theta[iJ], 
								pR->Dist[iJ],
								pR->Length[iJ],
								pR->Alpha[iJ] );
		pWEP[iWEP].Theta  = pR->Theta[iJ];
		pWEP[iWEP].Dist	  = pR->Dist[iJ];
		pWEP[iWEP].Length = pR->Length[iJ];
		pWEP[iWEP].Alpha  = pR->Alpha[iJ];

		if ( bGraspingCamera )  CamEntS = CamEntS * pWEA[iWEA].A;

		pWEA[iWEA].EId = pR->LinkId[iJ];
		pWEP[iWEP].EId = pR->LinkId[iJ];
	}

{	DiA0 * pA;		double G = pR->Gripper * DEG2PI;

	pA = &pR->GripOrgA0;	pWEA[iWEA].A.SetRT ( pA->X,  pA->Y,  pA->Z, 
												 pA->AX, pA->AY, pA->AZ );

	if ( bGraspingCamera )  CamEntS = CamEntS * pWEA[iWEA].A;

	pWEA[iWEA++].EId = pR->GripOrgId;


//	pA = &pR->Finger1A0;	pWEA[iWEA].A.SetRT ( pA->X,  pA->Y,  -pR->Gripper, 
//												 pA->AX, pA->AY,  pA->AZ );
	pA = &pR->Finger1A0;	pWEA[iWEA].A.SetTR ( pA->X,  pA->Y,   pA->Z,
													-G,  pA->AY,  pA->AZ );
	pWEA[iWEA++].EId = pR->Finger1Id;


//	pA = &pR->Finger2A0;	pWEA[iWEA].A.SetRT ( pA->X,  pA->Y,   pR->Gripper, 
//												 pA->AX, pA->AY,  pA->AZ );
	pA = &pR->Finger2A0;	pWEA[iWEA].A.SetTR ( pA->X,  pA->Y,   pA->Z,
													 G,  pA->AY,  pA->AZ );
	pWEA[iWEA++].EId = pR->Finger2Id;
}

	//	2011-Jan-18		Viewport sets the entities.  See callback below.
	//
	pCBD->pWEAV->nEnts = iWEA;
	pCBD->pWEPV->nEnts = iWEP;


	//	2009-Jul-10		If grasped entity is a camera ...
	//
	if ( bGraspingCamera )
	{
		//	Camera coordinate frame defines the viewpoint.
		//
		//	2011-Feb-21
		//
	//	pCBD->pAFR = new VP_AFR;
	//
	//	int CamEntId = pR->GrspEntId;
	//
	//	if ( pR->GrspEntId )
	//		CamEntS  = CamEntS * CamEntA;
	//	else
	//		CamEntId = pR->GripOrgId;
	//
	//	SetCameraUpdate ( CamEntId, CamEntS, *pCBD->pAFR );
		throw E ( sW, "Implement camera update stuff." );
	}

		//	2011-Jan-18		Trying a FIFO.
		//
		pCB->iP = iP_NULL;		pCB->Cmd = PECB_SYNC_VPS3;
								pCB->p   = pCBD;

		ENo = pCB->CallBack ( pCB );
		
		delete pCBD;

		if ( ENo )  throw ENo;

	}	//	for ( iS; ... )


#undef	MAX_WEA

}	//	CDGenRobot01::MoveOnPath()


/*----------------------  CDGenRobot01::MoveOnPath_j()  ----------------------*/
/*
*/
void	CDGenRobot01::MoveOnPath_j ( PECB *				pCB, 
									 SRGenRobot01_vXX *	pR, 
								     int				iJk0, 
								     int				iJkL )
{
#define	MAX_WEJ		32

	int ENo = 0;

	int	iJ, iS;		BOOL bRotate[7];

	WorldEntJoint *	pWEJ = 0;	int iWEJ;

	for ( iJ = iJk0; iJ <= iJkL; iJ++ )
	{
		bRotate[iJ-iJk0] = (((int)pR->JType[iJ] & (int)jtRotating) != 0);
	}


	for ( iS = 0; iS < pPath->nS; iS++ )
	{
		MvSln * pS = &pPath->pS[iS];

	//	2011-Jan-14, 18
	//
	RbtMovCBData_j * pCBD = CreateCBD_j ( pCB, MAX_WEJ );

	pWEJ = pCBD->pWEJ->pEnts;		iWEJ = 0;

	for ( iJ = iJk0; iJ <= iJkL; iJ++, iWEJ++ )
	{
		if ( bRotate[iJ - iJk0] ) {
			pWEJ[iWEJ].J = pR->Theta[iJ] = pS->J[iJ - iJk0]; }
		else {
			pWEJ[iWEJ].J = pR->Dist[iJ] = pS->J[iJ-iJk0]; }

		pWEJ[iWEJ].EId = pR->LinkId[iJ];
	}

	pCBD->pWEJ->nEnts = iWEJ;


		//	2011-Jan-18		Trying a FIFO.
		//
		pCB->iP = iP_NULL;		pCB->Cmd = PECB_JOINTS;
								pCB->p   = pCBD;

		ENo = pCB->CallBack ( pCB );
		
		delete pCBD;

		if ( ENo )  throw ENo;

	}	//	for ( iS; ... )


#undef	MAX_WEJ

}	//	CDGenRobot01::MoveOnPath_j()


CM4		CDGenRobot01::GetLastLinkS ( SRGenRobot01_vXX * pR )
{
	int		iJk0 = -1;		//	First joint (of the arrary of joints) to be 
							//	inverse kinematically solved.
	int		iJkL = -1;		//	Last of such joints.

	int		iJ;

	GetIKJoints ( pR, iJk0, iJkL );

	CM4	Ps;

	for ( iJ = iJk0; iJ <= iJkL; iJ++ )
	{
		CM4	A;	A.SetPaulA ( pR->Theta[iJ],
							 pR->Dist[iJ],
							 pR->Length[iJ],
							 pR->Alpha[iJ] );

		if ( iJ == iJk0 ) {
			if ( pPath ) {
				Ps = pPath->B * A; }
			else {
				Ps = A; }
			continue; }

		Ps = Ps * A;
	}

	return Ps;

}	//	CDGenRobot01::GetLastLinkS()


//	GenRobot01.cpp
