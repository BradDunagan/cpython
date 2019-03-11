/*
		 1         2         3         4         5         6         7		   8
12345678901234567890123456789012345678901234567890123456789012345678901234567890

	File:	FIP.h		Function In Progress

*/

#ifndef		__FIP_H
#define		__FIP_H

/*------------------------------ Dependencies  -----------------------------*/

#include	"abb_rr_m4.h"

#include	"abb_rr_genrobot01_defs.h"

#include	"abb_rr_rgenrobot01defs.h"

//	End Dependencies

#define PORT


class	CDGenRobot01;

struct	SRGenRobot01_vXX;

class	CM4;



//	2002-Jul-15		Supports functions in progress (FIP).
//
//	That is, for example, during long moves a return is made to the
//	caller so it can do things (maybe appear to be multitasking) at
//	each step.  These hold the conditions of things between steps.
//
struct	FIP_Vals
{
	//	Arm movement.
	//
	//		iJointLimit		Normally -1.  Set to joint index on return if
	//						the joint limit is passed.
	//
	int		Step ( PECB *				pCB, 
				   CDGenRobot01 &		R, 
				   SRGenRobot01_vXX	*	pR,
				   MovToRtn &			Rtn,
				   int &				iJointLimit );

	int		Finish ( PECB *				pCB, 
					 CDGenRobot01 &		R, 
					 SRGenRobot01_vXX * pR,
					 MovToRtn &			Rtn,
					 int &				iJointLimit );

	int		(CDGenRobot01::*Kinematics) ( PECB *				pCB,
										  const CM4 &			Trg,
										  SRGenRobot01_vXX *	pR,
										  BOOL &				bDeltaChgEr,
										  double				SSF,
										  MovToRtn &			Rtn,
										  int &					iJointLimit );

	BOOL	bMovingArm;

//	double	CurTheta1,	//	For restoring to the begining position
//			CurTheta2,	//	if something does not work.
//			CurTheta3,
//			CurTheta4,
//			CurTheta5,
//			CurTheta6,
//			CurTheta7;
	double	JStart [GR01_MAX_JOINTS];	//	For restoring to the begining 
										//	position in case of error.

	int		iJ1;		//	The first inverse kinematically solvable joint.
	
	int		nJ;			//	There may be non-solvable joints between iJ1 and
						//	iJ1 + nJ (inclusive).

	CM4		Cb;			//	Current position WRT base.
	
	CM4		Tb;			//	Target position WRT base.

	int		nSteps, iStep, nFinishes;

	double	DPx,   DPy,   DPz,   DAx,   DAy,   DAz;

	double	dPx,   dPy,   dPz,   dAx,   dAy,   dAz;

	double	dTPx,  dTPy,  dTPz,  dTAx,  dTAy,  dTAz;

	double	dTPx0, dTPy0, dTPz0, dTAx0, dTAy0, dTAz0;

//	double	J1,    J2,    J3,    J4,    J5,    J6,    J7;
	double	JStepStart [GR01_MAX_JOINTS];

	int		nSS,   iSS;

	double	SSF,   nSSF;


	//	Gripper movement.
	//
	int		StepGripper ( PECB * pCB,
						  CDGenRobot01 & R, SRGenRobot01_vXX * pR );

	int		FinishGripper ( PECB * pCB,
							CDGenRobot01 & R, SRGenRobot01_vXX * pR );

	BOOL	bMovingGripper;

	double	Gripper;	//	Target gripper opening.

	double	dGripper;	//	Difference between target and original opening.

	int		nGripperSteps, iGripperStep;
};




#endif

//	FIP.h
