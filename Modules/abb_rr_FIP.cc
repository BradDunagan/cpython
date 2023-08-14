/*
		 1         2         3         4         5         6         7		   8
12345678901234567890123456789012345678901234567890123456789012345678901234567890

	File:	FIP.cpp		Function In Progress

*/

/*------------------------------ Dependencies  ------------------------------*/

#include	"abb_rr_genrobot01.h"

#include	"abb_rr_FIP.h"

#include	"abb_rr_world.h"

#include	"abb_rr_m4.h"

#include	"abb_rr_libarg.h"

#include	"abb_rr_diags.h"

#include	"abb_rr_error.h"

#include	"abb_rr_rgenrobot01.h"

//	End Dependencies


//	A macro for specifying filename and function name in diagnostics calls.
//
#define	F(f)	"(" __FILE__ ")" "  " f


#define	RPPAUL_MAX_SUBSTEPS		10

#define	RPPAUL_MAX_FINISHES		10


/*----------------------  CDGenRobot01::Get_Cb_Tb()  -------------------------*/
/*
	Ps		Target Position WRT system coordinates.

			This is the target position of the gripper coordinates.  That is,
			the "working coordinate frame" off the gripper/fingers.

	Ls		Position of last link WRT system.

	Cb		Current position of the last link WRT robot's base.

	Tb		Target position of last link WRT robot's base.

	2011-Oct-01

	Tb		Target position of gripping coords WRT robot's base.
*/
void	CDGenRobot01::Get_Cb_Tb ( const CM4 &				Ps,
								  const CM4 &				Ls,
										SRGenRobot01_vXX *	pR,
										CM4 & Cb, CM4 & Tb )
{
	const char * sW = F("CDGenRobot01::Get_Cb_Tb()");

	if ( ! pPath ) {
		DelE ( E ( sW, "Expect pPath.  Deleting error, returning." ) );
		return; }

	CM4	G;		//	Gripping coords WRT robot's link 6 coords.

	//	G is oriented relative to link 6 such that when G is square with
	//	the system origin then the gripper will be pointing down with
	//	the opening in the system's Z axis.
	//
	Print (  3, sW, "GripOrgId: %d\n", pR->GripOrgId );

	if ( pR->GripOrgId )
	{
		DiA0 & A0 = pR->GripOrgA0;	//	2009-Jan-28

		G.SetRT ( A0.X, A0.Y, A0.Z, A0.AX, A0.AY, A0.AZ );
	}
	else
		G.SetXYZRPY_TPR ( 0, 0, 0, 0, 0, 0 );

	/*	2011-Oct-01		Doing target position of gripping coords WRT base.

		Below.

	CM4	nG;	nG = ~G;				//	The position of link 6 WRT to G.

	CM4	B;	B  = pW->GetS ( pR->BaseId );	//	Pose of robot base.


	//	Target = ~B * Ps * G;
	//
	CM4	nB;	nB = ~B;

	CM4	T1;	T1 = nB * Ps;

	Tb = T1 * nG;		//	Target WRT base.


	//	Current position of last link.
	//
	Cb = nB * Ls;		//  Current position WRT base.
	*/
	//	2011-Oct-01		Doing target position of gripping coords WRT base.
	//
//	CM4	B;	B  = pW->GetS ( pR->BaseId );	//	Pose of robot base.
//
//	CM4	nB;	nB = ~B;
	CM4 nB; nB = pPath->nB;

	Tb = nB * Ps;

	Cb = nB * (Ls * G);

	PrintJandGb ( sW, "", pR );

}	//	CDGenRobot01::Get_Cb_Tb()


/*------------------  CDGenRobot01::FIP_MoveTo_Setup()  --------------------*/
/*
	2002-Jul-15

	Ps		Position WRT system coordinates.
*/
int		CDGenRobot01::FIP_MoveTo_Setup (	   int iJ1, int nJ, bool & b180Adj,
										 const CM4 & Cb,
										 const CM4 & Tb,
											   SRGenRobot01_vXX *	pR )
{
//	int ENo = 0;	const char * sW = F("CDGenRobot01::FIP_MoveTo_Setup()");
					const char * sW = F("_Setup()");


	double	TPx, TPy, TPz, TAx, TAy, TAz;

	b180Adj = false;

	//	Tb.GetXYZRPY_Hosed ( TPx, TPy, TPz, TAx, TAy, TAz );
	//
	//	2003-Dec-17		Fixing the TRP mess...
	//
	//	For this, TPR should work. (?)
	//
	Tb.GetXYZRPY_TPR ( TPx, TPy, TPz, TAx, TAy, TAz );

	//	Anything close -180 degress ...  make +180 so, for example, if
	//	CAx is -180 and TAx is 90 the difference will be 90 (after changing
	//	CAx to +180) instead of 270.
	//
	if ( fabs ( fabs ( TAx ) - PI ) < 0.00000001 )  TAx = PI;
	if ( fabs ( fabs ( TAy ) - PI ) < 0.00000001 )  TAy = PI;
	if ( fabs ( fabs ( TAz ) - PI ) < 0.00000001 )  TAz = PI;

	Print (  3, sW, "Tb: %7.2f %7.2f %7.2f  %7.2f %7.2f %7.2f",
					TPx, TPy, TPz, TAx * PI2DEG,
								   TAy * PI2DEG,
								   TAz * PI2DEG );


	//	Break up the move into steps.
	//
	CM4	nCb = ~Cb;

	CM4	Tc  = nCb * Tb;

	double	CPx, CPy, CPz, CAx, CAy, CAz;
	double	DPx, DPy, DPz, DAx, DAy, DAz;

	//	Cb.GetXYZRPY_Hosed ( CPx, CPy, CPz, CAx, CAy, CAz );
	//
	//	2003-Dec-17		Fixing the TRP mess...
	//
	//	For this, TPR should work. (?)
	//
	Cb.GetXYZRPY_TPR ( CPx, CPy, CPz, CAx, CAy, CAz );

	Print (  3, sW, "Cb: %7.2f %7.2f %7.2f  %7.2f %7.2f %7.2f",
					CPx, CPy, CPz, CAx * PI2DEG,
								   CAy * PI2DEG,
								   CAz * PI2DEG );

	//	Anything close -180 degress ...  make +180 so, for example, if
	//	CAx is -180 and TAx is 90 the difference will be 90 (after changing
	//	CAx to +180) instead of 270.
	//
	if ( fabs ( fabs ( CAx ) - PI ) < 0.00000001 )  CAx = PI;
	if ( fabs ( fabs ( CAy ) - PI ) < 0.00000001 )  CAy = PI;
	if ( fabs ( fabs ( CAz ) - PI ) < 0.00000001 )  CAz = PI;

	Print (  3, sW, "Cb: fixed -\n" );
	Print (  3, sW, "Cb: %7.2f %7.2f %7.2f  %7.2f %7.2f %7.2f",
					CPx, CPy, CPz, CAx * PI2DEG,
								   CAy * PI2DEG,
								   CAz * PI2DEG );

	//	Tc.GetXYZRPY_Hosed ( DPx, DPy, DPz, DAx, DAy, DAz );
	//
	//	2003-Dec-17		Fixing the TRP mess...
	//
	//	For this, TPR should work. (?)
	//
	Tc.GetXYZRPY_TPR ( DPx, DPy, DPz, DAx, DAy, DAz );

	if ( fabs ( fabs ( DAx ) - PI ) < 0.00000001 )  DAx = PI;
	if ( fabs ( fabs ( DAy ) - PI ) < 0.00000001 )  DAy = PI;
	if ( fabs ( fabs ( DAz ) - PI ) < 0.00000001 )  DAz = PI;

	Print (  3, sW, "Tc: %7.2f %7.2f %7.2f  %7.2f %7.2f %7.2f",
					DPx, DPy, DPz, DAx * PI2DEG,
								   DAy * PI2DEG,
								   DAz * PI2DEG );

	//	2009-Jan-30		If DAx and DAz are +/- 180 ...
	//
	//	Its like changing DAy by 180.
	//
//	if (    (fabs ( fabs ( DAx ) - PI ) < 0.00000001)
//		 && (fabs ( fabs ( DAz ) - PI ) < 0.00000001) )
	if (    (fabs ( fabs ( DAx ) - PI ) < 0.00001)
		 && (fabs ( fabs ( DAz ) - PI ) < 0.00001) )
	{
		DAx = DAz = 0;

		if ( DAy > 0 )  DAy = -(DAy - PI);
		else
			DAy = -(DAy + PI);

		Tc.SetXYZRPY_TPR ( DPx, DPy, DPz, DAx, DAy, DAz );

		Print (  3, sW, "Tc: %7.2f %7.2f %7.2f  %7.2f %7.2f %7.2f",
						DPx, DPy, DPz, DAx * PI2DEG,
									   DAy * PI2DEG,
									   DAz * PI2DEG );
		
		b180Adj = true;		//	2009-Dec-25
	}

	//	Check
	//
	CM4	T2 = Cb * Tc;		//	Should be same as Tb.

	T2.GetXYZRPY_TPR ( TPx, TPy, TPz, TAx, TAy, TAz );

	if ( fabs ( fabs ( TAx ) - PI ) < 0.00000001 )  TAx = PI;
	if ( fabs ( fabs ( TAy ) - PI ) < 0.00000001 )  TAy = PI;
	if ( fabs ( fabs ( TAz ) - PI ) < 0.00000001 )  TAz = PI;

	Print (  3, sW, "Tb: %7.2f %7.2f %7.2f  %7.2f %7.2f %7.2f",
					TPx, TPy, TPz, TAx * PI2DEG,
								   TAy * PI2DEG,
								   TAz * PI2DEG );

	
	
	
	double	dP = sqrt ( (DPx * DPx) + (DPy * DPy) + (DPz * DPz) );

	double	dS = dP / 1.0;

	if ( (fabs(DAx) / (2.5 * DEG2PI)) > dS )  dS = fabs(DAx) / (2.5 * DEG2PI);

	if ( (fabs(DAy) / (2.5 * DEG2PI)) > dS )  dS = fabs(DAy) / (2.5 * DEG2PI);

	if ( (fabs(DAz) / (2.5 * DEG2PI)) > dS )  dS = fabs(DAz) / (2.5 * DEG2PI);


	Print (  3, sW, "dS (nSteps): %7.2f", dS );


	FVs.iJ1	   = iJ1;
	FVs.nJ     = nJ;

	FVs.Cb     = Cb;
	FVs.Tb     = Tb;

	FVs.DPx    = DPx;
	FVs.DPy    = DPy;
	FVs.DPz    = DPz;
	FVs.DAx    = DAx;
	FVs.DAy    = DAy;
	FVs.DAz    = DAz;

	FVs.dPx    = DPx / dS;
	FVs.dPy    = DPy / dS;
	FVs.dPz    = DPz / dS;
	FVs.dAx    = DAx / dS;
	FVs.dAy    = DAy / dS;
	FVs.dAz    = DAz / dS;

	FVs.nSteps = (int)dS;
	FVs.iStep  = 0;

	FVs.dTPx   = 0;
	FVs.dTPy   = 0;
	FVs.dTPz   = 0;
	FVs.dTAx   = 0;
	FVs.dTAy   = 0;
	FVs.dTAz   = 0;

	FVs.nSS    = 0;

	FVs.bMovingArm = TRUE;

	FVs.nFinishes = 0;

	return 0;

}	//	CDGenRobot01::FIP_MoveTo_Setup()


/*----------------  CDGenRobot01::FIP_MoveTo_UpdateSetup()  ----------------*/
/*
	2008-Jan-30

	The point of this "setup again" is to update the step value so that
	we don't overshoot at the end.
*/
int		CDGenRobot01::FIP_MoveTo_UpdateSetup ( SRGenRobot01_vXX * pR )
{
//	int ENo = 0; const char * sW = F("CDGenRobot01::FIP_MoveTo_UpdateSetup()");
				 const char * sW = F("_Updt()");

	if ( ! pPath ) {
		return E ( sW, "Expect pPath" ); }

	//	Rexamine the difference between the current and target positions.
	//
	//	We don't need to calculate the target - use what was determined
	//	in the first setup.
	//
	CM4	B, nB, T1, Cb, nCb, Tc;		double DPx, DPy, DPz, DAx, DAy, DAz;

//	B  = pW->GetS ( pR->BaseId );
	B  = pPath->B;

	nB = ~B;

	//	Current position of last link.
	//
//	if ( pPath )  T1 = pPath->Es;
//	else
//		T1 = pW->GetS ( pR->LinkId[pR->nJoints - 1] );
	T1 = pPath->Es;

//	Cb  = nB * T1;			//  Current position of last link WRT base.
//	//
//	//	2011-Oct-01		Trying grasp coords as target.
	//
	//	See corresponding changes of this date in other places.
	//
	CM4	G;			//	Grasp coords WRT last link.

	{	DiA0 & A0 = pR->GripOrgA0;

		G.SetRT ( A0.X, A0.Y, A0.Z, A0.AX, A0.AY, A0.AZ );
	}
	T1  = T1 * G;
	Cb  = nB * T1;			//  Current position of grasp coords WRT base.
	
	nCb = ~Cb;

	Tc  = nCb * FVs.Tb;

	Tc.GetXYZRPY_TPR ( DPx, DPy, DPz, DAx, DAy, DAz );



	if ( fabs ( fabs ( DAx ) - PI ) < 0.00000001 )  DAx = PI;
	if ( fabs ( fabs ( DAy ) - PI ) < 0.00000001 )  DAy = PI;
	if ( fabs ( fabs ( DAz ) - PI ) < 0.00000001 )  DAz = PI;

	Print (  2, sW, "Tc: %7.2f %7.2f %7.2f  %7.2f %7.2f %7.2f",
					DPx, DPy, DPz, DAx * PI2DEG,
								   DAy * PI2DEG,
								   DAz * PI2DEG );

	//	2009-Jan-30		If DAx and DAz are +/- 180 ...
	//
	//	Its like changing DAy by 180.
	//
//	if (    (fabs ( fabs ( DAx ) - PI ) < 0.00000001)
//		 && (fabs ( fabs ( DAz ) - PI ) < 0.00000001) )
	if (    (fabs ( fabs ( DAx ) - PI ) < 0.00001)
		 && (fabs ( fabs ( DAz ) - PI ) < 0.00001) )
	{
		DAx = DAz = 0;

		if ( DAy > 0 )  DAy = -(DAy - PI);
		else
			DAy = -(DAy + PI);

		Tc.SetXYZRPY_TPR ( DPx, DPy, DPz, DAx, DAy, DAz );

		Print (  2, sW, "Tc: %7.2f %7.2f %7.2f  %7.2f %7.2f %7.2f",
						DPx, DPy, DPz, DAx * PI2DEG,
									   DAy * PI2DEG,
									   DAz * PI2DEG );
	}


	double	dP = sqrt ( (DPx * DPx) + (DPy * DPy) + (DPz * DPz) );

	double	dS = dP / 1.0;

	if ( (fabs(DAx) / (2.5 * DEG2PI)) > dS )  dS = fabs(DAx) / (2.5 * DEG2PI);
	if ( (fabs(DAy) / (2.5 * DEG2PI)) > dS )  dS = fabs(DAy) / (2.5 * DEG2PI);
	if ( (fabs(DAz) / (2.5 * DEG2PI)) > dS )  dS = fabs(DAz) / (2.5 * DEG2PI);


	Print (  2, sW, "dS (nSteps): %7.2f", dS );


	FVs.Cb     = Cb;

	FVs.DPx    = DPx;
	FVs.DPy    = DPy;
	FVs.DPz    = DPz;
	FVs.DAx    = DAx;
	FVs.DAy    = DAy;
	FVs.DAz    = DAz;

	FVs.dPx    = DPx / dS;
	FVs.dPy    = DPy / dS;
	FVs.dPz    = DPz / dS;
	FVs.dAx    = DAx / dS;
	FVs.dAy    = DAy / dS;
	FVs.dAz    = DAz / dS;

	FVs.nSteps = (int)dS;
	FVs.iStep  = 0;

	FVs.dTPx   = 0;
	FVs.dTPy   = 0;
	FVs.dTPz   = 0;
	FVs.dTAx   = 0;
	FVs.dTAy   = 0;
	FVs.dTAz   = 0;

	FVs.nSS    = 0;

	FVs.bMovingArm = TRUE;

	return 0;

}	//	CDGenRobot01::FIP_MoveTo_UpdateSetup()


/*---------------------------  FIP_Vals::Step()  ---------------------------*/
/*
	2002-Jul-15
*/
int		FIP_Vals::Step ( PECB *				pCB, 
						 CDGenRobot01 &		R, 
						 SRGenRobot01_vXX *	pR,
						 MovToRtn &			Rtn,
						 int &				iJointLimit )
{
	int	ENo;	//	const char * sW = F("FIP_Vals::Step()");

	if ( R.StepCnt >= 0xfff )  R.StepCnt = 0;

	char	sW [80];	sprintf ( sW, "_Step(%03x)", ++R.StepCnt );

	_ASSERTE ( iStep < nSteps );

	if ( nSS == 0 )			//	Not currently in the middle of a step?
	{
		//	Starting a step.
		//
		//	If/when we need to do the step in smaller sub-steps save the
		//	joints at the begining of the step.
		//
		R.GetJoints2 ( pR, iJ1, nJ, JStepStart );

		iSS = 0;		nSS  = 1;		//	Sub-steps.

		SSF = 1.0;		nSSF = 0;		//	Sub-step factor.
		//
		//	Initially the sub-step size is the entire step.

	//	dTPx0 = dTPx;
	//	dTPy0 = dTPy;
	//	dTPz0 = dTPz;
	//	dTAx0 = dTAx;
	//	dTAy0 = dTAy;
	//	dTAz0 = dTAz;
	}

	dTPx0 = dTPx;		//	To reset the substep (not the entire step).
	dTPy0 = dTPy;
	dTPz0 = dTPz;
	dTAx0 = dTAx;
	dTAy0 = dTAy;
	dTAz0 = dTAz;


	dTPx += dPx * SSF;
	dTPy += dPy * SSF;
	dTPz += dPz * SSF;
	dTAx += dAx * SSF;
	dTAy += dAy * SSF;
	dTAz += dAz * SSF;

	R.Print (    1, sW, "dT: %7.2f %7.2f %7.2f  %7.2f %7.2f %7.2f",
						dTPx, dTPy, dTPz, dTAx * PI2DEG,
										  dTAy * PI2DEG,
										  dTAz * PI2DEG );

	CM4	dT ( dTPx, dTPy, dTPz, dTAx, dTAy, dTAz );

//	CM4	Target = Cb * dT;
//	//
//	//	2011-Oct-01		dT is that of grasp coords WRT base.
	//
	//	IK function expects Target to be last link WRT base.
	//
	CM4	G;			//	Grasp coords WRT last link.

	{	DiA0 & A0 = pR->GripOrgA0;

		G.SetRT ( A0.X, A0.Y, A0.Z, A0.AX, A0.AY, A0.AZ );
	}

	CM4 Target = (Cb * dT) * ~G;


	double	J0Jerky [GR01_MAX_JOINTS];

	R.GetJoints2 ( pR, iJ1, nJ, J0Jerky );


	BOOL	bDeltaChgEr;

//	if ( (ENo = R.RPPaul ( Target, pR, bDeltaChgEr )) != 0 )
//	//
	//	2009-Feb-13
	//
//	if ( (ENo = (R.*Kinematics) ( Target, pR, bDeltaChgEr )) != 0 )
	//
	//	2009-Mar-23		
	//
	//	Pass SSF so the routine might adjust the joints on large changes.
	//
	if ( (ENo = (R.*Kinematics) ( pCB, Target, pR, bDeltaChgEr, SSF,
												   Rtn,
												   iJointLimit       )) != 0 )
	{
		return T ( ENo, sW );
	}

	if ( Rtn != mtrOK )  return 0;

	if ( iJointLimit >= 0 )  return 0;

	if ( bDeltaChgEr )
	{
		if ( nSSF++ < RPPAUL_MAX_SUBSTEPS )
		{
			//	Restore the joints to that at the begining of the
			//	step.
			//
		//	R.ResetJoints2 ( pR, iJ1, nJ, JStepStart );
		//
		//	//	Start the step over.  Do it in smaller sub-steps.
		//	//
		//	SSF = SSF / 2.0;	iSS = 0;	nSS *= 2;
		//	//
			//	Do not start the entire step again.  Instead, just continue 
			//	in smaller substeps.
			//
			SSF = SSF / 2.0;				nSS = (nSS * 2) - iSS;

			dTPx = dTPx0;
			dTPy = dTPy0;
			dTPz = dTPz0;
			dTAx = dTAx0;
			dTAy = dTAy0;
			dTAz = dTAz0;

			R.Print (    1, sW, "Smaller steps.  (nSS %d)", nSS );

			ENo = LAR_IN_PROGRESS;
		}
		else
			return E ( sW, "Failed to find kinematic solution." );
	}
	else
	{//	double	j1, j2, j3, j4, j5, j6, j7;		//	Jerky?
	//
	//	R.GetJoints2 ( pR, j1, j2, j3, j4, j5, j6, j7 );
	//
	//	R.Print (    1, sW, "dj: %7.2f %7.2f %7.2f  %7.2f %7.2f %7.2f %7.2f",
	//						(j1 - j01) * PI2DEG, 
	//						(j2 - j02) * PI2DEG, 
	//						(j3 - j03) * PI2DEG, 
	//						(j4 - j04) * PI2DEG,
	//						(j5 - j05) * PI2DEG,
	//						(j6 - j06) * PI2DEG,
	//						(j7 - j07) * PI2DEG );
		double	JJerky [GR01_MAX_JOINTS];  char Buf [256];  int iJ, iB = 0;

		R.GetJoints2 ( pR, iJ1, nJ, JJerky );

		iB += sprintf ( Buf, "dj: " );

		for ( iJ = 0; iJ < nJ; iJ++ )  
		{
			double	D = JJerky[iJ] - J0Jerky[iJ];

			if ( pR->JType[iJ1 + iJ] & jtRotating )  D *= PI2DEG;

			iB += sprintf ( &Buf[iB], "%7.2f ", D );
		}

		R.Print (    1, sW, Buf );
	}

	if ( bDeltaChgEr )  return ENo;


	if ( ++iSS == nSS )
	{
		nSS = 0;		nSSF = 0;

		++iStep;
	}

	return LAR_IN_PROGRESS;

}	//	FIP_Vals::Step()

		
/*--------------------------  FIP_Vals::Finish()  --------------------------*/
/*
	2002-Jul-15
*/
int		FIP_Vals::Finish ( PECB *				pCB, 
						   CDGenRobot01 &		R, 
						   SRGenRobot01_vXX *	pR,
						   MovToRtn &			Rtn,
						   int &				iJointLimit )
{
	int	ENo;	//	const char * sW = F("FIP_Vals::Finish()");
				//	const char * sW = F("_Finish()");

	if ( R.StepCnt >= 0xfff )  R.StepCnt = 0;

	char	sW [80];	sprintf ( sW, "_Fnsh(%03x)", ++R.StepCnt );


	_ASSERTE ( iStep == nSteps );

	if ( nSS == 0 )			//	Not currently in the middle of a step?
	{
		//	Starting the final step.
		//
		//	If/when we need to do the step in smaller sub-steps save the
		//	joints at the begining of the step.
		//
		R.GetJoints2 ( pR, iJ1, nJ, JStepStart );

		iSS = 0;		nSS  = 1;		//	Sub-steps.

		SSF = 1.0;		nSSF = 0;		//	Sub-step factor.
		//
		//	Initially the sub-step size is the entire step.

	//	dTPx0 = dTPx;
	//	dTPy0 = dTPy;
	//	dTPz0 = dTPz;
	//	dTAx0 = dTAx;
	//	dTAy0 = dTAy;
	//	dTAz0 = dTAz;

		//	Try to finish in one substep.
		//
		dPx = DPx - dTPx;
		dPy = DPy - dTPy;
		dPz = DPz - dTPz;
		dAx = DAx - dTAx;
		dAy = DAy - dTAy;
		dAz = DAz - dTAz;
	}

	dTPx0 = dTPx;		//	To reset the substep (not the entire step).
	dTPy0 = dTPy;
	dTPz0 = dTPz;
	dTAx0 = dTAx;
	dTAy0 = dTAy;
	dTAz0 = dTAz;


	R.Print (    1, sW, "dT: %7.2f %7.2f %7.2f  %7.2f %7.2f %7.2f",
						dTPx, dTPy, dTPz, dTAx * PI2DEG,
										  dTAy * PI2DEG,
										  dTAz * PI2DEG );

//	dTPx += dPx * SSF;	if ( dPx > 0.0 ) { if ( dTPx > DPx )  dTPx = DPx; }
//						else
//							if ( dTPx < DPx )  dTPx = DPx;
//
//	dTPy += dPy * SSF;	if ( dPy > 0.0 ) { if ( dTPy > DPy )  dTPy = DPy; }
//						else
//							if ( dTPy < DPy )  dTPy = DPy;
//
//	dTPz += dPz * SSF;	if ( dPz > 0.0 ) { if ( dTPz > DPz )  dTPz = DPz; }
//						else
//							if ( dTPz < DPz )  dTPz = DPz;
//
//	dTAx += dAx * SSF;	if ( dAx > 0.0 ) { if ( dTAx > DAx )  dTAx = DAx; }
//						else
//							if ( dTAx < DAx )  dTAx = DAx;
//
//	dTAy += dAy * SSF;	if ( dAy > 0.0 ) { if ( dTAy > DAy )  dTAy = DAy; }
//						else
//							if ( dTAy < DAy )  dTAy = DAy;
//
//	dTAz += dAz * SSF;	if ( dAz > 0.0 ) { if ( dTAz > DAz )  dTAz = DAz; }
//						else
//							if ( dTAz < DAz )  dTAz = DAz;
//	//
	//	Should not need to check the boundary since dPx was set on the first
	//	substep above.
	//
	dTPx += dPx * SSF;
	dTPy += dPy * SSF;
	dTPz += dPz * SSF;
	dTAx += dAx * SSF;
	dTAy += dAy * SSF;
	dTAz += dAz * SSF;


	R.Print (    1, sW, "dT: %7.2f %7.2f %7.2f  %7.2f %7.2f %7.2f",
						dTPx, dTPy, dTPz, dTAx * PI2DEG,
										  dTAy * PI2DEG,
										  dTAz * PI2DEG );

	CM4	dT ( dTPx, dTPy, dTPz, dTAx, dTAy, dTAz );

//	CM4	Target = Cb * dT;
//	//
//	//	2011-Oct-01		dT is that of grasp coords WRT base.
	//
	//	IK function expects Target to be last link WRT base.
	//
	CM4	G;			//	Grasp coords WRT last link.

	{	DiA0 & A0 = pR->GripOrgA0;

		G.SetRT ( A0.X, A0.Y, A0.Z, A0.AX, A0.AY, A0.AZ );
	}

	CM4 Target = (Cb * dT) * ~G;



	BOOL	bDeltaChgEr;

//	if ( (ENo = R.RPPaul ( Target, pR, bDeltaChgEr )) != 0 )
//	//
	//	2009-Feb-13
	//
//	if ( (ENo = (R.*Kinematics) ( Target, pR, bDeltaChgEr )) != 0 )
	//
	//	2009-Mar-23		
	//
	//	Pass SSF so the routine might adjust the joints on large changes.
	//
	if ( (ENo = (R.*Kinematics) ( pCB, Target, pR, bDeltaChgEr, SSF,
												   Rtn,
												   iJointLimit )) != 0 )
	{
		return T ( ENo, sW );
	}

	if ( Rtn != mtrOK )  return 0;

	if ( iJointLimit >= 0 )  return 0;

	if ( bDeltaChgEr )
	{
		if ( nSSF++ < RPPAUL_MAX_SUBSTEPS )
		{
			//	Restore the joints to that at the begining of the
			//	step.
			//
		//	R.ResetJoints2 ( pR, iJ1, nJ, JStepStart );
		//
		//	//	Start the step over.  Do it in smaller sub-steps.
		//	//
		//	SSF = SSF / 2.0;	iSS = 0;	nSS *= 2;
		//	//
			//	Do not start the entire step again.  Instead, just continue 
			//	in smaller substeps.
			//
			SSF = SSF / 2.0;				nSS = (nSS * 2) - iSS;

			dTPx = dTPx0;
			dTPy = dTPy0;
			dTPz = dTPz0;
			dTAx = dTAx0;
			dTAy = dTAy0;
			dTAz = dTAz0;

			R.Print (    1, sW, "Smaller steps.  (nSS %d)", nSS );

			return LAR_IN_PROGRESS;
		}

		return E ( sW, "Failed to find kinematic solution." );
	}

	if ( ! R.pPath ) {
		return E ( sW, "Expect R.pPath" ); }

	//	Made it to the target?  Examine the difference between the current
	//	and target positions.
	//
	CM4	B, nB, T1, Cb2, nCb, Tc;	double DPx2, DPy2, DPz2, DAx2, DAy2, DAz2;

//	B   = R.pW->GetS ( pR->BaseId );
	B   = R.pPath->B;

	nB  = ~B;

	//	Current position of last link.
	//
//	if ( R.pPath )  T1 = R.pPath->Es;
//	else
//	    T1 = R.pW->GetS ( pR->LinkId[pR->nJoints - 1] );
	T1 = R.pPath->Es;

//	Cb2 = nB * T1;			//  Current position of last link WRT base.
//	//
//	//	2011-Oct-01		Trying grasp coords as target.
	//
	//	See corresponding changes of this date in other places.
	//
	T1  = T1 * G;
	Cb2 = nB * T1;			//  Current position of grasp coords WRT base.

	nCb = ~Cb2;

	Tc  = nCb * Tb;

	Tc.GetXYZRPY_TPR ( DPx2, DPy2, DPz2, DAx2, DAy2, DAz2 );


	double	dP = sqrt ( (DPx2 * DPx2) + (DPy2 * DPy2) + (DPz2 * DPz2) );

	if (    (fabs ( dP )            < 0.0001)
		 && (fabs ( DAx2 * PI2DEG ) < 0.001 )
		 && (fabs ( DAy2 * PI2DEG ) < 0.001 )
		 && (fabs ( DAz2 * PI2DEG ) < 0.001 ) )
	{
		bMovingArm = FALSE;

		//	2008-Sep-24		Show finished position.
		//
	//	CM4	G;	if ( (ENo = R.GetGripSM ( G )) != 0 )  return T ( ENo, sW );
	//
	//	double GPx, GPy, GPz, GAx, GAy, GAz;
	//
	//	G.GetXYZRPY_TPR ( GPx, GPy, GPz, GAx, GAy, GAz );
	//
	//	::Print (  1, sW, " G: %7.2f %7.2f %7.2f  %7.2f %7.2f %7.2f",
	//						GPx, GPy, GPz, GAx * PI2DEG,
	//									   GAy * PI2DEG,
	//									   GAz * PI2DEG );
	//	later

		return 0;
	}

	//	Next substep.
	//
	R.Print (    1, sW, " D: %7.2f %7.2f %7.2f  %7.2f %7.2f %7.2f",
						DPx2, DPy2, DPz2, DAx2 * PI2DEG,
										  DAy2 * PI2DEG,
										  DAz2 * PI2DEG );

	if ( ++iSS >= nSS )
	{
		nSS = 0;		nSSF = 0;

		if ( ++nFinishes >= RPPAUL_MAX_FINISHES )
		{
			return E ( sW, "(nSS %d)  Could not find kinematic solution.  "
						   "Returning error.", nSS );
		}
	}

	return LAR_IN_PROGRESS;

}	//	FIP_Vals::Finish()

		


//	FIP.cpp
