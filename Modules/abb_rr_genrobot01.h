/*
		 1         2         3         4         5         6         7		   8
12345678901234567890123456789012345678901234567890123456789012345678901234567890

	File:	GenRobot01.h

	2000-May-25

	Something for implementing a generalized kinematics for robots.  See
	"A Complete Generalized Solution to the Inverse Kinematics of Robots"
	by Andrew A. Goldenberg, B.Benhabib and Robert G. Fenton, IEEE Journal
	of Robotics and Automation Vol. RA-I, No. 1, March 1985.

	The ideas in that paper were implemented in sim3c.

	A reimplementation is attempted here.
*/

#ifndef		__GENROBOT01_H
#define		__GENROBOT01_H

/*------------------------------ Dependencies  -----------------------------*/

#include	"abb_rr_libarg.h"

#include	"abb_rr_m4.h"

#include	"abb_rr_genrobot01_defs.h"

#include	"abb_rr_FIP.h"

#include	"abb_rr_rgenrobot01defs.h"

//	End Dependencies


#define PORT


class	CDGenRobot01;

struct	PECB;

struct	VP_AFR;

struct	RbtMovCBData;

struct	RbtMovCBData_j;


//	Bits set in the order word.
//
#define	oSA		0x0000000A		//	Solution A.
#define	oSB		0x0000000B		//	Solution B.

#define	oLA		0x0000000A		//	Last pos, solution A was used.
#define	oLB     0x0000000B		//	          solution B was used.

#define	o1st	0x000000F0		//	Current pos, first  solution, 0, A or B.
#define	o2nd	0x00000F00		//				 second solution, 0, B or A.

#define	oH1A	0x000000A0		//	Current pos, solution 1 is A.
#define	oH1B	0x000000B0		//               solution 1 is B.

#define	oH2A	0x00000A00		//	Current pos, solution 2 is A.
#define	oH2B	0x00000B00		//               solution 2 is B.

#define	oSEQ	0x000F0000		//	Sequence.  When, relative to other joints,
								//	the joint was solved.

//	o	The order word.
//
//	i	The index of the solution: 0 - 1st, 1 - 2nd.
//
//	n	The number indicating sequence in which solution was found: 1 - 7
//
//	s	oSA or oSB
//
#define	O_SOLVED(o,i)	((o & (0xF0 << (i * 4))) != 0)

#define	O_AorB(o,i)		(((o & 0xFF0) >> (4 + (i * 4))) & 0xF)

#define O_GET_SEQ(o)	((o & oSEQ) >> 16)

#define O_SET_SEQ(o,n)	(o = (o & ~oSEQ) | (n << 16))

#define	O_GET_S1(o)		((o & o1st) >> 4)
#define	O_SET_S1(o,s)	(o = (o & ~o1st) | (s << 4))

#define	O_GET_S2(o)		((o & o2nd) >> 8)
#define	O_SET_S2(o,s)	(o = (o & ~o2nd) | (s << 8))

#define O_LA(o)         ((o & 0x0000000F) == oSA)
#define O_LB(o)         ((o & 0x0000000F) == oSB)



//	2009-Jul-10
//
//	MovRbtTo(), etc., use MvSln and MvPath to store the joint positions
//	(the solutions) as the arm moves instead of moving the arm to each
//	solution as it goes.
//
//	Then, after the entire path is solved, the MvSlns are iterated over
//	setting the real joints and calling back on the app to update the 
//	display in for each MvSln.
//
struct	MvSln
{
	double	J [7];

	double	ET;

	//	2011-May-08
	//
	//	Remember how each joint was solved so that if run in to a joint
	//	limit we might try solving the joint with a different solution.
	//
	int		Odr [7];
};

struct	MvPath
{
	MvPath ( int Cap ) 
	{ 
		this->Cap = Cap;	
		
		nS = 0;  pS = new MvSln[Cap];
	
		ResetOdr();
	}

   ~MvPath() { if ( pS )  delete [] pS; }

	CM4		B;			//	Base WRT system.

	CM4		nB;			//	Inv of base WRT system.

	CM4		Es;			//	Current system position of last link.

//	dVector	Sb;			//	Starting X,Y,Z WRT base.

	double	DistT0;		//	Starting distance from target.

	double	JCur [GR01_MAX_JOINTS];		//	In case of a start over.

	int		Cap;		//	Max MvSlns.

	int		nS;			//	Number of MvSlns.

	MvSln *	pS;			//	Joint solution at each step.

	void	ResetOdr()
	{
		for ( int i = 0; i < Cap; i++ )
		for ( int j = 0; j <   7; j++ )  pS[i].Odr[j] = 0;
	}
};


typedef int (* InvKinFunc)( const double Tgt[], double JCur[], 
						 		  double JNew[][2], double MaxDelta,
								  int     Odr[] );

/*-------------------------  class  CDGenRobot01  --------------------------*/
/*
*/
class CDGenRobot01
{
	friend		struct	FIP_Vals;

	public:


		CDGenRobot01();




virtual ~CDGenRobot01();



		int		bDiagsOn;

		int		DL;			//	Diags Level.

		void		Print (		  int		Level, 
							const char *	sW, 
							const char *	Fmt, ... );

		void		PrintM ( const char *	sW,
							 const CM4 &	M,
							 const char *	Desc, ... );

		//	Identify a robot.
		//
		//	If RobotName is an empty string then all robot entities are
		//	discharged.
		//
		//	Otherwise...
		//
		//	The user will be prompted to specify the base and other robot
		//	links (entities) including fingers.  The user may cancel at
		//	any time.  If the user cancels the current robot remains intact.
		//
		//	The robot name becomes the name of this data record.
		//
		//	Returns ENo.
		//
		int		IdRbt ( const char * RobotName );


		//	2009-Feb-27		To set joint's Paul parameters.
		//
		//	Returns ENo.
		//
		int		ConfigureLink ( PECB * pCB, 
								int  iJ,   LID  EId,
								BOOL bRot, BOOL bKin, double Theta,
													  double Dist,
													  double Length,
													  double Alpha );


		//	Move the robot's gripper to a position WRT the system coords.
		//
		//	Returns ENo.
		//
		//	2002-Jul-15
		//
		//	MovRbtTo() and Continue() may now return a non-zero value 
		//	indicating Continue() must be called again to finish the move.  
		//	Caller should test the LAR_NOT_AN_ENO bit in the returned value.
		//	If not set then it is an ENo.
		//
		//	If these functions return LAR_IN_PROGRESS then caller must call
		//	Continue() again.
		//
		//	2011-May-11
		//
		//		Rtn		From the cmd rec's perspective, the function's return
		//				value.  0: all ok, 1: joint limit, and so on.
		//
		int		MovRbtTo ( PECB * 		pCB, 
						   P6 * 		pB6, 	//	Base WRT system
					//	   P6 * 		pL6, 	//	Last link WRT system
						   CM4 * 		pPs, 	//	Last link target WRT system
						   MovToRtn & 	Rtn );


		int		SetPaulJoint ( PECB *				pCB,
							   int					iJ, 
							   SRGenRobot01_vXX *	pR );


		void	GetIKJoints ( SRGenRobot01_vXX * pR, int & iJk0, 
													 int & iJkL );


		void	ResetJoints ( PECB *				pCB,
							  SRGenRobot01_vXX *	pR,
							  int iJ1, int nJ, double J[],
							  BOOL					bNotify );


		double	DeltaAngle ( double A1, double A2 );


		double	AbsDeltaAngle ( double A1, double A2 );


		int		GetMovePath ( PECB *				pCB, 
							  SRGenRobot01_vXX *	pR, 
							  int					iJk0, 
							  int					iJkL,
							  CM4 &			Tb,
							  MovToRtn &	Rtn,
							  int &			nStartOvers,
							  bool &		bStartOver,
							  int &			iJointLimit );


	protected:


	private:

		SRGenRobot01_vXX *  pRD;

		InvKinFunc	IKFunc;				//	Inverse kinematics function in
										//	the DLL built by the SymKin
		bool		bTryingSingularity;	//	routines.

		MvPath *	pPath;				//	2009-Jul-10

		FIP_Vals	FVs;				//	2002-Jul-15

		int			Odr [GR01_MAX_JOINTS];	//	Order in which joints are 
											//	solved.
		int			StepCnt;

		int			nResidErrors;		//	Number of times the residual test 
										//	failed.

		int			nRecalls;			//	Number of times InvKinFunc() is 
										//	called again to get an alternate 
										//	solution.

		bool		bSingularity;		//	Set true when the solution was 
										//	found by relaxing the joint change 
										//	limit.


		void		ZeroEverything();


		int			CleanUp ( const char * sW );

		SRGenRobot01_vXX *	Get();

		void 	Put();

		void	PutC();
	
		void	GetJoints2 ( SRGenRobot01_vXX * pR, 
							 int iJ1, int nJ, double J[] );

		RbtMovCBData *	CreateCBD ( PECB * pCB, int MaxWEA, int MaxWEP );


		RbtMovCBData_j *	CreateCBD_j ( PECB * pCB, int MaxWEJ );


		void	LARInProgress ( PECB *				pCB, 
								SRGenRobot01_vXX *	pR, 
								int					iJk0, 
								int					iJkL,
								CM4 &		Tb,
								int &		nStartOvers,
								bool &		bStartOver    );


		void	MoveOnPath ( PECB *				pCB, 
							 SRGenRobot01_vXX *	pR,  
							 int				iJk0, 
							 int				iJkL );


		void	MoveOnPath_j ( PECB *				pCB, 
							   SRGenRobot01_vXX *	pR,  
							   int					iJk0, 
							   int					iJkL );


		//	In FIP.cpp
		//
		void	Get_Cb_Tb ( const CM4 &					Ps, 
							const CM4 &					Ls,
								  SRGenRobot01_vXX *	pR,
								  CM4 & Cb, CM4 & Tb );

		int		FIP_MoveTo_Setup ( 		 int iJ1, int nJ, bool & b180Adj, 
								   const CM4 & Cb,
								   const CM4 & Tb,
										 SRGenRobot01_vXX * pR );

		int		FIP_MoveTo_UpdateSetup ( SRGenRobot01_vXX * pR );

		int		FIP_Gripper_Setup ( double				Gripper, 
									SRGenRobot01_vXX *	pR );

		//	In	KinematicsPaul03.cpp
		//
		bool	TryBoth ( CM4 & T, CM4 & G, CM4 A[], CM4 & B, 
						  BOOL   bRotate[],
						  double JNewR[][2], int  Odr[], int Jn,
						  InvKinFunc IKFnc,		 double JCurR[], double MaxDelta,
											 double J_BAF[] );


		void	PrintJNewR ( const char	*	sW, 
								   double	JNewR[][2], 
								   int		Odr[] );


		void	PrintXYZ ( const char *	sW,
						   const char *	Desc, 
						   const CM4 &	P );


		void	PrintJandGb ( const char *	sW,
							  const char *	Desc,
									SRGenRobot01_vXX *	pR );


		int		KPaul03_RPPaul (	   PECB *				pCB,
								 const CM4 &				Trg,
									   SRGenRobot01_vXX *	pR,
									   BOOL &				bDeltaChgEr,
									   double				SSF,
									   MovToRtn &			Rtn,
									   int &				iJointLimit );

		CM4		GetLastLinkS ( SRGenRobot01_vXX * pR );

};	//	class	CDGenRobot01




#endif

//	GenRobot01.h
