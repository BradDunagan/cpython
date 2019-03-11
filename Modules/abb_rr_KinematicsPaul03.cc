/*
		 1         2         3         4         5         6         7		   8
12345678901234567890123456789012345678901234567890123456789012345678901234567890

	File:	KinematicsPaul03.cpp

*/

/*------------------------------ Dependencies  ------------------------------*/

#include	<memory.h>

#include	"abb_rr_genrobot01.h"

#include	"abb_rr_m4.h"

#include	"abb_rr_libarg.h"

#include	"abb_rr_diags.h"

#include	"abb_rr_error.h"

#include	"abb_rr_rgenrobot01.h"

//	End Dependencies


//	A macro for specifying filename and function name in diagnostics calls.
//
#define	F(f)	"(" __FILE__ ")" "  " f





/*-----------------------  CDGenRobot01::TryBoth()  --------------------------*/
/*
*/
bool	CDGenRobot01::TryBoth ( CM4 & T, CM4 & G, CM4 A[], CM4 & B, 
								BOOL   bRotate[],
								double JNewR[][2], int Odr[], int Jn,
				  InvKin IKFnc, double JCurR[],    double MaxDelta,
								double J_BAF[] )	//	Best And Final
{
	const char * sW = "both";	//	F("CDGenRobot01::TryBoth()");

	int	i;

	for ( i = 0; i < 2; i++ )	//	solutions a and b
	{
		if ( ! O_SOLVED(Odr[Jn-1], i) )  continue;

		//	If this is the second solution and it is the same as the first...
		//
		if (    (i > 0) && O_SOLVED(Odr[Jn-1],0)
			 &&
				(fabs ( JNewR[Jn-1][0] - JNewR[Jn-1][i] ) < 0.000001) )  
		{
			continue;
		}

		CM4	J;	if ( bRotate[Jn-1] )  J.RotateZ ( JNewR[Jn-1][i] );

	//	J_BAF[Jn-1] = JNewR[Jn-1][i];

		{	double Jv = JNewR[Jn-1][i];		//	Print joint value.

		//	if ( fabs ( Jv - JCurR[Jn-1] ) > PI )
			if ( fabs ( Jv               ) > PI )
			{
				if ( Jv > 0 )  Jv -= 2 * PI;
				else
					Jv += 2 * PI;
			}

			Print ( 1, sW, "J[%d]  %7.2f", Jn - 1, Jv * PI2DEG );
		}

		CM4 Lb = B * (J * A[Jn - 1]);

		if ( Jn < 7 )
		{
			if ( TryBoth ( T, G, A, Lb, bRotate,
										JNewR,  Odr, Jn + 1, IKFnc, JCurR,
															 MaxDelta, 
										J_BAF ) )
			{
				if ( Odr[Jn-1] & 0xF )  Odr[Jn-1] &= 0xF;
				else
					Odr[Jn-1] = O_AorB(Odr[Jn-1],i);

				J_BAF[Jn-1] = JNewR[Jn-1][i];

				return true;
			}

			continue;
		}

	//	CM4 R = ~Lb * T;		double DPx, DPy, DPz, DAx, DAy, DAz;
	//	//
	//	//	2011-Oct-01		Check grasp-coords as target.
		//
		CM4 Gb = Lb * G;
		CM4 Tb = T  * G;
		PrintXYZ ( sW, "Gb", Gb );
		PrintXYZ ( sW, "Tb", Tb );
		CM4 R = ~Gb * Tb;	double DPx, DPy, DPz, DAx, DAy, DAz;

		R.GetXYZRPY_TPR ( DPx, DPy, DPz, DAx, DAy, DAz );

		if ( fabs ( fabs ( DAx ) - PI ) < 0.00000001 )  DAx = PI;
		if ( fabs ( fabs ( DAy ) - PI ) < 0.00000001 )  DAy = PI;
		if ( fabs ( fabs ( DAz ) - PI ) < 0.00000001 )  DAz = PI;

		if (    (fabs ( DPx ) < 0.0001)
			 && (fabs ( DPy ) < 0.0001)
			 && (fabs ( DPz ) < 0.0001)
			 && (fabs ( DAx ) < 0.0001)
			 && (fabs ( DAy ) < 0.0001)
			 && (fabs ( DAz ) < 0.0001) )
		//	 && (fabs ( DAx ) < 0.0010)
		//	 && (fabs ( DAy ) < 0.0010)
		//	 && (fabs ( DAz ) < 0.0010) )
		{
			Odr[Jn-1] = O_AorB(Odr[Jn-1],i);

			J_BAF[Jn-1] = JNewR[Jn-1][i];

			return true;
		}

		nResidErrors++;

		Print (  1, sW, "R:  %7.2f %7.2f %7.2f  %7.2f %7.2f %7.2f",
						DPx, DPy, DPz, DAx * PI2DEG,
									   DAy * PI2DEG,
									   DAz * PI2DEG );

	//	char	Buf [256];		int iB = sprintf ( Buf, "Jc: " );

	//	for ( iJ = 0; iJ < 7; iJ++ )
	//	{
	//		iB += sprintf ( &Buf[iB], "%7.2f ", JCur[iJ] * PI2DEG );
	//	}

	//	Print ( DA, sW, Buf );

	//	iB = sprintf ( Buf, "Jn: " );

	//	for ( iJ = 0; iJ < 7; iJ++ )
	//	{
	//		iB += sprintf ( &Buf[iB], "%7.2f ", JNewR[iJ][i] * PI2DEG );
	//	}

	//	Print ( DA, sW, Buf );
	}

	//	InvKin uses the first solution (of possibly two) of a joint to
	//	calculate the solutions of other joints.
	//	
	//	Use the other solution.
	//
	while ( IKFnc && (Jn < 7) && O_SOLVED(Odr[Jn-1],0) 
							  && O_SOLVED(Odr[Jn-1],1)  )
	{
		if ( fabs ( JNewR[Jn-1][0] - JNewR[Jn-1][1] ) < 0.000001 )  break;

		double	JNewRb[7][2];	memcpy ( JNewRb, JNewR, sizeof(JNewRb) );

		//	Joints may not be solved in 1 2 3 ... order.  6 may be solved
		//	before 5, for example.  If Jn is 6, in that case, then we must
		//	use the other value for 6 and set 5 as unsolved.
		//
		int	iJ[7], ij, j;		//	Joint indices in the oder they were solved.

		for ( i = 0; i < 7; i++ )
		{
			if ( ! (O_SOLVED(Odr[i],0) || O_SOLVED(Odr[i],1)) )  break;

			ij = O_GET_SEQ(Odr[i]) - 1;

			iJ[i] = ij;

			if ( ij >= Jn - 1 )  break;
		}

		iJ[i++] = ij;

		for ( ; i < 7; i++ )	//	Find the indices of the remaining joints.
		{
			for ( ij = 0; ij < 7; ij++ )
			{
				for ( j = 0; (j < i) && (iJ[j] != ij); j++ );

				if ( j == i )  { iJ[i] = ij;  break; }
			}
		}

		double JCurR2[7];    int Odr2[7];		//	Set the lower joints'
												//	current value to solutions 
		for ( i = 0; i < 7; i++ )				//	that were found for them.
		{
			ij = iJ[i];		_ASSERTE ( ij >= 0 );

			if ( ij + 1 == Jn )  break;

			JCurR2[ij] = O_SOLVED(Odr[ij],0) ? JNewR[ij][0] : JNewR[ij][1];
			
			Odr2[ij] = Odr[ij];		//	oSEQ not 0 indicates joint is solved.
		}

		//	For this joint, set the current value to the other solution.
		//
		Odr2[ij] = 0;

		JCurR2[ij] = JNewR[ij][1];  O_SET_SEQ(Odr2[ij], O_GET_SEQ(Odr[ij]));
									O_SET_S1(Odr2[ij], O_GET_S2(Odr[ij]));
		//							O_SET_S2(Odr2[ij], O_GET_S1(Odr[ij]));
		//	2009-Mar-22		
		//	
		O_SET_S2(Odr2[ij], 0);			//	Don't keep the first solution.

		JNewR[ij][0] = JNewR[ij][1];	//	Set new first to that of second.


		for ( i++; i < 7; i++ )		//	Solve the remaining joints.
		{
			ij = iJ[i];

			JCurR2[ij] = JCurR[ij];		Odr2[ij] = 0;
		}

		Print (  1, sW, "J:  %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f",
						JCurR2[0] * PI2DEG, JCurR2[1] * PI2DEG, 
						JCurR2[2] * PI2DEG, JCurR2[3] * PI2DEG, 
						JCurR2[4] * PI2DEG, JCurR2[5] * PI2DEG, 
						JCurR2[6] * PI2DEG );

		IKFnc ( T.GetM(), JCurR2, JNewR, MaxDelta, Odr2 );


		PrintJNewR ( sW, JNewR, Odr2 );


		nRecalls++;

		CM4	Lb;

	//	if ( TryBoth ( T, A, Lb, bRotate, JNewR, Odr2, 1, 0, 0, 0, J_BAF ) )  
	//	//
		//	2009-Mar-22
		//
		//	It may be that a later joint will now have multiple solutions.
		//
		for ( i = 0; i < 7; i++ )		//	Get a solution on all joints?
		{
			if ( ! (O_SOLVED(Odr2[i],0) || O_SOLVED(Odr2[i],1)) )  break;
		}

		if ( i == 7 )
		{
			if ( TryBoth ( T, G, A, Lb, bRotate, JNewR, Odr2, 1, IKFnc, 
																 JCurR2, 
																 MaxDelta, 
																 J_BAF ) )  
			{
				memcpy ( Odr, Odr2, sizeof(Odr2) );

				return true;
			}
		}

		//	Restore things.
		//
		memcpy ( JNewR, JNewRb, sizeof(JNewRb) );

	//	memcpy ( Odr, Odr2, sizeof(Odr2) );

		return false;
	}

	//	Singularity?
	//
	//	It may be that  * no *  soulutions were found for a joint because the 
	//	resulting change in joint position was greater than MaxDelta.
	//
	//	Try setting MaxDelta to 2PI.
	//
	//	If this is necessary in application the real robot might simply stop 
	//	all other joints and make the large changes in the ones that need it.  
	//	Hopefully the large changes will be at the end of the arm.  Otherwise
	//	the arm may need to run some extra routine in order to avoid collisions
	//	while the large move is done.
	//
	while ( IKFnc && (Jn < 7) && ! (   O_SOLVED(Odr[Jn-1],0) 
									|| O_SOLVED(Odr[Jn-1],1)) )
	{
		if ( bTryingSingularity )  break;

		bTryingSingularity = true;

		double	JNewRb[7][2];	memcpy ( JNewRb, JNewR, sizeof(JNewRb) );

		int		Odrb[7];		memcpy ( Odrb, Odr, sizeof(Odrb) );

		//	Joints may not be solved in 1 2 3 ... order.  6 may be solved
		//	before 5, for example.  If Jn is 6, in that case, then we must
		//	use the other value for 6 and set 5 as unsolved.
		//
		int	iJ[7], ij, j;		//	Joint indices in the oder they were solved.

		for ( i = j = 0; i < 7; i++ )
		{
		//	if ( ! (   O_SOLVED(Odr[Jn-1],0) 
		//			|| O_SOLVED(Odr[Jn-1],1)) )  continue;
			if ( ! (   O_SOLVED(Odr[i   ],0) 
					|| O_SOLVED(Odr[i   ],1)) )  continue;

			iJ[j++] = i;
		}

		int nJS = j;			//	Number of solved joints.

		for ( ; j < 7; j++ )	//	Find the indices of the remaining joints.
		{
			for ( ij = 0; ij < 7; ij++ )
			{
				for ( i = 0; (i < j) && (iJ[i] != ij); i++ );

				if ( i == j )  { iJ[j] = ij;  break; }
			}
		}

		double JCurR2[7];    int Odr2[7];		//	Set the lower joints'
												//	current value to solutions 
		for ( i = 0; i < nJS; i++ )				//	that were found for them.
		{
			ij = iJ[i];

		//	JCurR2[ij] = O_SOLVED(Odr[Jn-1],0) ? JNewR[ij][0] : JNewR[ij][1];
			JCurR2[ij] = O_SOLVED(Odr[ij  ],0) ? JNewR[ij][0] : JNewR[ij][1];
			
			Odr2[ij] = Odr[ij];		//	oSEQ not 0 indicates joint is solved.
		}

		//	Attempt to solve the remaining joints again with no limit 
		//	(2 * PI) on the joint change.
		//
		for ( ; i < 7; i++ )
		{
			ij = iJ[i];

			JCurR2[ij] = JCurR[ij];		Odr2[ij] = 0;
		}

		IKFnc ( T.GetM(), JCurR2, JNewR, 2 * PI, Odr2 );

		CM4	Lb;		bool bR;

		bR = TryBoth ( T, G, A, Lb, bRotate,
									JNewR, 
			//	2011-Oct-02			Odr2, 1, IKFnc, JCurR,  2 * PI, J_BAF );
									Odr2, 1, IKFnc, JCurR2, 2 * PI, J_BAF );

		nRecalls++;

		bTryingSingularity = false;

		if ( bR )  
		{
		//	bSingularity = true;
		//
		//	return true;
		//	//
			bSingularity = true;

			memcpy ( Odr, Odr2, sizeof(Odr2) );		//	Remember order for next
													//	move.
			return true;
		}

		//	Restore things.
		//
		memcpy ( JNewR, JNewRb, sizeof(JNewRb) );

		memcpy ( Odr, Odrb, sizeof(Odrb) );

		return false;
	}

	return false;

}	//	CDGenRobot01::TryBoth()


/*---------------------  CDGenRobot01::PrintJNewR()  ------------------------*/
/*
*/
void	CDGenRobot01::PrintJNewR ( const char *	sW, 
										 double JNewR[][2], 
										 int	Odr[] )
{
	char sBuf[256];		int iB, iJ;

	iB = sprintf ( sBuf, "N:  " );

	for ( iJ = 0; iJ < 7; iJ++ )
	{
		if ( O_SOLVED(Odr[iJ],0) )  
			iB += sprintf ( &sBuf[iB], "%7.2f ", JNewR[iJ][0] * PI2DEG );
		else
			iB += sprintf ( &sBuf[iB], "        " );
	}

	Print (  1, sW, sBuf );

	iB = sprintf ( sBuf, "N:  " );

	for ( iJ = 0; iJ < 7; iJ++ )
	{
		if ( O_SOLVED(Odr[iJ],1) )  
			iB += sprintf ( &sBuf[iB], "%7.2f ", JNewR[iJ][1] * PI2DEG );
		else
			iB += sprintf ( &sBuf[iB], "        " );
	}

	Print (  1, sW, sBuf );

}	//	CDGenRobot01::PrintJNewR()


/*---------------------  CDGenRobot01::PrintXYZ()  --------------------------*/
/*
*/
void	CDGenRobot01::PrintXYZ ( const char *	sW,
								 const char *	Desc,
								 const CM4 &	P )
{
	Print ( 1, sW, "%s XYZ %8.2f %8.2f %8.2f", Desc,
											   P.GetX(), P.GetY(), P.GetZ() );

}	//	CDGenRobot01::PrintXYZ()

/*---------------------  CDGenRobot01::PrintJandGb()  ------------------------*/
/*
	Show joint values and Grip WRT base.
*/
void	CDGenRobot01::PrintJandGb ( const char *	sW,
									const char *	Desc,
										  SRGenRobot01_vXX *	pR )
{
	double	JCur [GR01_MAX_JOINTS];
	int		iJk0 = -1;
	int		iJkL = -1;
	int		iJ;
	CM4		Gb;

	GetIKJoints ( pR, iJk0, iJkL );

	GetJoints2 ( pR, iJk0, iJkL - iJk0 + 1, JCur );

	for ( iJ = iJk0; iJ <= iJkL; iJ++ )
	{
		CM4	J;		J.RotateZ ( JCur[iJ] );
		CM4	A;		A.SetPaulA ( 0, pR->Dist[iJ], pR->Length[iJ], pR->Alpha[iJ] );

		Gb = Gb * (J * A);
	}

	CM4	G;			//	Grasp coords WRT last link.

	{	DiA0 & A0 = pR->GripOrgA0;

		G.SetRT ( A0.X, A0.Y, A0.Z, A0.AX, A0.AY, A0.AZ );
	}

	Gb = Gb * G;

	Print ( 1, sW, "%s   J: %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f",
					Desc,
					JCur[0] * PI2DEG, JCur[1] * PI2DEG, JCur[2] * PI2DEG, 
					JCur[3] * PI2DEG, JCur[4] * PI2DEG, JCur[5] * PI2DEG, 
					JCur[6] * PI2DEG );

	Print ( 1, sW, "%s XYZ: %7.2f %7.2f %7.2f", Desc,
												Gb.GetX(), Gb.GetY(), Gb.GetZ() );

}	//	CDGenRobot01::PrintJandGb()


/*-------------------  CDGenRobot01::KPaul03_RPPaul()  ----------------------*/
/*
	Closed kinematic solution (i.e., no iterations).

	Based on kinematics_2() in GO_____1.C of five\sim4c, HWN around 1987-Feb?

	2000-Jul-03

		Trg		Target WRT base.

	2009-Feb-28

	Uses solution automatically generated by SymKin.

	2009-Mar-04

	This version of this function 
	
		Uses a new interface to the inverse kinematics function which
		allows solutions to be eeked out in difficult situations.  That
		is, at degeneraces and singularities.
*/
int		CDGenRobot01::KPaul03_RPPaul (		 PECB *				pCB,
									   const CM4 &				Trg,
											 SRGenRobot01_vXX *	pR,
											 BOOL &				bDeltaChgEr,
											 double				SSF,
											 MovToRtn &			Rtn,
											 int &				iJointLimit )
{
//	int ENo = 0;	char sW [80];	sprintf ( sW, "_03Pl(%03x)", StepCnt );
					char sW [80];	sprintf ( sW, "_03Pl(%03x)", StepCnt );

//	int		PaulDL = 3;

	bDeltaChgEr = FALSE;		Rtn = mtrOK;	iJointLimit = -1;

//	if ( StepCnt == 0x2d )
//	{
//		Print ( DA, sW, "break" );
//	}
//
//	if ( StepCnt == 0x22 )
//	{
//		//	Note that the change is set in Es on StepCnt x26.
//
//		Print ( DA, sW, "break  look for Es.pY change" );
//	}
//
//	if ( StepCnt == 0x3a )
//	{
//		Print ( DA, sW, "break  Why isn't the erroneous solution caught?" );
//	}
//
//	if ( fabs ( pPath->Es.GetY() - 15.0 ) > 0.001 )
//	{
//		Print ( DA, sW, "oops" );
//	}

//	double	Px, Py, Pz, Pz2;
//
//	Px  = Trg.GetX();
//	Py  = Trg.GetY();
//	Pz  = Trg.GetZ();
//	Pz2 = Pz - d1;			//	Subtract offset of first link from base.
//
//	if ( sqrt ( (Px * Px) + (Py * Py) + (Pz2 * Pz2) ) > pR->Reach )
//	{
//		return E ( sW, "Target is out of reach." );
//	}

	//	Loaded and linked to the inverse kinematics function?
	//
//	if ( (IKFunc == 0) && ((ENo = LoadIKFunc ( pR )) != 0) )
//	{
//		return T ( ENo, sW );
//	}
	if ( IKFunc == 0 ) {
		return E ( sW, "IKFunc is not set." ); }

	double	JCur [GR01_MAX_JOINTS], JNew [GR01_MAX_JOINTS];

	int		iJk0 = -1;		//	First joint (of the array of joints) to be 
							//	inverse kinematically solved.
	int		iJkL = -1;		//	Last of such joints.

	int		iJ;

	bool	bSmallChg = false;

	GetIKJoints ( pR, iJk0, iJkL );

	GetJoints2 ( pR, iJk0, iJkL - iJk0 + 1, JCur );

	{	double TPx, TPy, TPz, TAx, TAy, TAz;		//	Target wrt base.

		Trg.GetXYZRPY_TPR ( TPx, TPy, TPz, TAx, TAy, TAz );

		if ( fabs ( fabs ( TAx ) - PI ) < 0.00000001 )  TAx = PI;
		if ( fabs ( fabs ( TAy ) - PI ) < 0.00000001 )  TAy = PI;
		if ( fabs ( fabs ( TAz ) - PI ) < 0.00000001 )  TAz = PI;

		Print (  3, sW, "T:  %7.2f %7.2f %7.2f  %7.2f %7.2f %7.2f",
						TPx, TPy, TPz, TAx * PI2DEG,
									   TAy * PI2DEG,
									   TAz * PI2DEG );
	}

	Print (  3, sW, "J:  %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f",
					JCur[0] * PI2DEG, JCur[1] * PI2DEG, JCur[2] * PI2DEG, 
					JCur[3] * PI2DEG, JCur[4] * PI2DEG, JCur[5] * PI2DEG, 
					JCur[6] * PI2DEG );

	CM4 T ( Trg );


	double MaxD = PI / 2;		//	Max allowed change in joint.  For now.

	double JCurR [7], JNewR [7][2];		int OdrBak [GR01_MAX_JOINTS];

	for ( iJ = 0; iJ < 7; iJ++ )  JCurR[iJ] = JCur[iJ];

	memcpy ( OdrBak, Odr, sizeof(OdrBak) );


	for ( iJ = 0; iJ < 7; iJ++ )		//	2011-Apr-08
	{
		MvSln & S = pPath->pS[pPath->nS];

		if ( S.Odr[iJ] )  
		{
			Odr[iJ] = (Odr[iJ] & 0xFFFFFFF0) | (S.Odr[iJ] & 0x0000000F);
		}

		//	2011-May-09		Temp. not trying both.  See below.
		//
//		Odr[iJ] &= 0x0000000F;
	}


	nResidErrors = 0;  nRecalls = 0;  bSingularity = false;

	bTryingSingularity = false;

	IKFunc ( T.GetM(), JCurR, JNewR, MaxD, Odr );


	PrintJNewR ( sW, JNewR, Odr );


	CM4	Lb;		CM4 A[7];	BOOL bRotate[7];	double J_BAF [7];

	for ( iJ = 0; iJ < 7; iJ++ )
	{
	//	bRotate[iJ] =	 (((int)pR->JType[iJ] & (int)jtRotating) != 0)
	//				  && (((int)pR->JType[iJ] & (int)jtNoInvKin) == 0);
		bRotate[iJ] =	 (((int)pR->JType[iJ]  & (int)jtRotating) != 0);

		A[iJ].SetPaulA ( 0, pR->Dist[iJ], pR->Length[iJ], pR->Alpha[iJ] );
	}

	//	2011-Oct-01		Trying grasp coords as target.
	//
	//	See corresponding changes of this date in other places.
	//
	CM4	G;			//	Grasp coords WRT last link.

	{	DiA0 & A0 = pR->GripOrgA0;

		G.SetRT ( A0.X, A0.Y, A0.Z, A0.AX, A0.AY, A0.AZ );

		PrintXYZ ( sW, "Gb", T * G );
	}

	if ( ! TryBoth ( T, G, A, Lb, bRotate, 
								  JNewR, Odr, 1, IKFunc, JCurR, MaxD, J_BAF ) )
	{
		memcpy ( Odr, OdrBak, sizeof(OdrBak) );

	//	return E ( sW, "Could not find solution." );
	//	//
		Rtn = mtrInvKin;

		return 0;
	}
//	//
//	//	2011-May-09		See what happens now (without both) ...
//	//
//	//	Also see   Odr[iJ] &= 0x0000000F;   (of this date) above.
//	//
//	for ( iJ = iJk0; iJ <= iJkL; iJ++ )  J_BAF[iJ] = JNewR[iJ][0];


	//	Adjust by 2PI if we can get the soulutions closer to the current
	//	joint values.
	//
	for ( iJ = iJk0; iJ <= iJkL; iJ++ )
	{
		JNew[iJ] = J_BAF[iJ];

		if ( fabs ( JNew[iJ] - JCur[iJ] ) > PI )
		{
			if ( JNew[iJ] > 0 )  JNew[iJ] -= 2 * PI;
			else
				JNew[iJ] += 2 * PI;
		}

		//	Check delta change.
		//
		if ( AbsDeltaAngle ( JNew[iJ], JCur[iJ] ) > ((PI * 3) / 180.0) )
		{
			bDeltaChgEr = TRUE;
		}
	}

//	if ( bDeltaChgEr && (SSF < (1.0 / 64.0)) )
	if ( bDeltaChgEr && (SSF < (1.0 / 16.0)) )
	{
		//	Try making small changes here.
		//
		for ( iJ = iJk0; iJ <= iJkL; iJ++ )
		{
			JNew[iJ] = JCur[iJ] + ((JNew[iJ] - JCur[iJ]) * SSF);
		}

		bSmallChg   = true;		//	2011-May-10

		bDeltaChgEr = FALSE;
	}

	if ( bDeltaChgEr )
	{
		return 0;
	}

	Print (  3, sW, "N:  %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f",
					JNew[0] * PI2DEG, JNew[1] * PI2DEG, JNew[2] * PI2DEG, 
					JNew[3] * PI2DEG, JNew[4] * PI2DEG, JNew[5] * PI2DEG, 
					JNew[6] * PI2DEG );


	//	2011-May-02		Joint limit violation?
	//
	for ( iJ = iJk0; iJ <= iJkL; iJ++ )
	{
		if ( pR->JointLimit[iJ][0] >= pR->JointLimit[iJ][1] )  continue;

		if (    (JNew[iJ - iJk0] < pR->JointLimit[iJ][0])
			 || (JNew[iJ - iJk0] > pR->JointLimit[iJ][1]) )
		{
		//	return M ( AE_RSP_INVALID_JOINT_VALUE, sW, 
		//			   "Joint [%d] limit.", iJ );
		//	//
			memcpy ( Odr, OdrBak, sizeof(OdrBak) );

			Rtn = mtrJointLimit;

			iJointLimit = iJ;

			return 0;
		}
	}


	//	Set the links.
	
	//	Doing a path?
	//
	if ( pPath )
	{
		if ( pPath->nS >= pPath->Cap )  return E ( sW, "Out of MvSlns." ); 

		MvSln * pS = &pPath->pS[pPath->nS++];

		if ( ! bSmallChg )
		{
		//	pPath->Es = pW->GetS ( pR->BaseId );
			pPath->Es = pPath->B;
		}

		for ( iJ = iJk0; iJ <= iJkL; iJ++ )
		{
			//	Remember the joint solution.
			//
			if ( bRotate[iJ - iJk0] )  
				pS->J[iJ - iJk0] = pR->Theta[iJ] = JNew[iJ - iJk0];
			else
				pS->J[iJ - iJk0] = pR->Dist[iJ]  = JNew[iJ - iJk0];

			pS->Odr[iJ] = Odr[iJ];		//	2011-Apr-08

			if ( ! bSmallChg )
			{
				//	Get the new position of the last link.
				//
				CM4	A;	A.SetPaulA ( pR->Theta[iJ], 
									 pR->Dist[iJ],
									 pR->Length[iJ],
									 pR->Alpha[iJ] );

				pPath->Es = pPath->Es * A;
			}
		}

		PrintJandGb ( sW, "path", pR );

		return 0;
	}

	//	Set the entity positions here.
	//
	for ( iJ = iJk0; iJ <= iJkL; iJ++ )
	{
		if ( bRotate[iJ - iJk0] )  pR->Theta[iJ] = JNew[iJ - iJk0];
		else
			pR->Dist[iJ] = JNew[iJ-iJk0];

		SetPaulJoint ( pCB, iJ, pR );
	}

//	{	CM4	A;	DiA0 * pA;	//	2009-Jan-28
//	
//		pA = &pR->GripOrgA0;	A.SetRT ( pA->X,  pA->Y,  pA->Z, 
//										  pA->AX, pA->AY, pA->AZ );
//
//		if ( (ENo = pW->SetA ( pR->GripOrgId, A )) != 0 )  return ENo;
//
//		pA = &pR->Finger1A0;	A.SetRT ( pA->X,  pA->Y,  -pR->Gripper, 
//										  pA->AX, pA->AY,  pA->AZ );
//
//		if ( (ENo = pW->SetA ( pR->Finger1Id, A )) != 0 )  return ENo;
//
//		pA = &pR->Finger2A0;	A.SetRT ( pA->X,  pA->Y,   pR->Gripper, 
//										  pA->AX, pA->AY,  pA->AZ );
//
//		if ( (ENo = pW->SetA ( pR->Finger2Id, A )) != 0 )  return ENo;
//	}
//	later
//
//	if ( (ENo  = pW->Notify ( DCHG_POS, pCB->ChgFlags, 0, 0, 0, FALSE, 0, 0 )) 
//			  != 0 )  return ENo;
//	
//	Notify ( DCHG_ROBOT, pCB->ChgFlags, 0, 0, 0, FALSE, 0, 0 );

	//	2002-Sep-03		Move the grasped entity.
	//
//	if ( (ENo = SetGrspEnt ( GEgo )) != 0 )		//	2002-Sep-03
//	{
//		return T ( ENo, sW, "Returning error." );
//	}
	//
	//	2009-Jan-19		Necessary?  If an entity is grasped then will it
	//					not move as a descendent?


	//	2009-Jul-10		If grasped entity is a camera ...
	//
//	if ( (pR->GrspEntId > 0) && (pR->GrspFlags & GRGF_CAMERA) )
//	{
//		//	Camera coordinate frame defines the viewpoint.
//
//		SendCameraUpdate ( pR->GrspEntId );
//	}
//	later

	return 0;

}	//	CDGenRobot01::KPaul03_RPPaul()


//	KinematicsPaul03.cpp
