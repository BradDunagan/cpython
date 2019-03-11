/*
		 1         2         3         4         5         6         7		   8
12345678901234567890123456789012345678901234567890123456789012345678901234567890

	File:	RGenRobot01.cpp

	To update SRGenRobot01_vXX.

	The latest (current) SRGenRobot01_vXX is defined in RGenRobot01.h.

	Previous SRGenRobot01_vXX are only defined here because they are used
	only here.  That is, if CDGenRobot01::Get() detects a version that is
	not the latest it calls a function here - CDGenRobot01::UpdateSR() - to 
	update it so that no other part of GenRobot01 will see the old stuff.
*/

/*------------------------------ Dependencies  ------------------------------*/

#include	"abb_rr_genrobot01.h"

#include	"abb_rr_rgenrobot01.h"

#include	"abb_rr_diags.h"

#include	"abb_rr_error.h"

//	End Dependencies


//	A macro for specifying filename and function name in diagnostics calls.
//
#define	F(f)	"(" __FILE__ ")" "  " f


struct	SRGenRobot01_v01
{
	//	Entities are of the world.  They are not records.
	//
	LID		BaseId;

	LID		LinkId [GR01_MAX_JOINTS];
	DiA0	LinkA0 [GR01_MAX_JOINTS];

	LID		Finger1Id;
	DiA0	Finger1A0;

	LID		Finger2Id;
	DiA0	Finger2A0;

	LID		GripOrgId;
	DiA0	GripOrgA0;

	//	Entity that is grasped.  When this Id is not 0 then it moves
	//	when the robot moves.  I.e., its position WRT the GripOrg remains
	//	the same.
	//
	LID		GrspEntId;
	DiA0	GrspEntA0;

	int		nJoints;

//	uint	nEJ;
//
//	uint	iEJ [GR01_MAX_EJ];

	//	Joint rotates, translates, and/or is not a variable in the inverse
	//	kinematic solution.
	//
	JointType	JType [GR01_MAX_JOINTS];	

	//	R. P. Paul parameters of joints.
	//
	double	Theta [GR01_MAX_JOINTS];		//	Rotating joint variable.

	double	Dist [GR01_MAX_JOINTS];			//	Translating joint variable.

	double	Length [GR01_MAX_JOINTS];

	double	Alpha [GR01_MAX_JOINTS];


	double	Reach;		//	Max distance from base to last link.


	//	Gripper
	//
	double	Gripper;				//	Gripper opening.

	double	GAx, GAy, GAz;			//	Gripper rotations WRT base.


	//	Joint position limits.
	//
	double	JointLimit [GR01_MAX_JOINTS][2];

	double	GripperLimit [2];


	//	Inverse kinematics.
	//
	Kinematics	K;					//	Specifies which kinematics function
									//	to use.
	char	IKModName [64];			//	If K is kSymKin then this specifies
									//	library module file name.

};	//	SRGenRobot01_v01;


/*-------------------  CDGenRobot01::InitNewRecord()  ------------------------*/
/*

void	CDGenRobot01::InitNewRecord ( BYTE * pRB, uint nRB, int Ver )
{
	int ENo = 0;	const char * sW = F("CDGenRobot01::InitNewRecord()");

	Status &= ~D_CONSTRUCTEDOK;

	memset ( pRB, 0, nRB );

	SRGenRobot01_vXX * pR = (SRGenRobot01_vXX *)pRB;	bool bError = false;

	uint	i;

	switch ( Ver )
	{
		case 2:
	
			pR->BaseId = 0;

			for ( i = 0; i < GR01_MAX_JOINTS; i++ )  
			{
				pR->LinkId[i] = 0;		pR->LinkA0[i].Reset();
			}

			pR->Finger1Id = 0;		pR->Finger1A0.Reset();
			pR->Finger2Id = 0;		pR->Finger2A0.Reset();
			pR->GripOrgId = 0;		pR->GripOrgA0.Reset();
			pR->GrspEntId = 0;		pR->GrspEntA0.Reset();

			pR->nJoints = 7;

		//	pR->nEJ = 0;
		//
		//	for ( i = 0; i < GR01_MAX_EJ; i++ )  pR->iEJ[i] = 0;

			//	Paul parameters.
			//
			for ( i = 0; i < GR01_MAX_JOINTS; i++ )
			{
				pR->JType[i]  = jtUndefined;

				pR->Theta[i]  = 0;
				pR->Length[i] = 0;
				pR->Dist[i]   = 0;
				pR->Alpha[i]  = 0;
			}

			pR->Reach = 0;				//	Max distance from base to last link.

			//	Gripper
			//
			pR->Gripper = 0;

			pR->GAx = pR->GAy = pR->GAz = 0;

			//	Joint limits.
			//
			for ( i = 0; i < GR01_MAX_JOINTS; i++ ) 
			{
				pR->JointLimit[i][0] = (-360.0 * DEG2PI);
				pR->JointLimit[i][1] = ( 360.0 * DEG2PI);
			}

			pR->GripperLimit[0] =  0.00;
			pR->GripperLimit[1] =  4.00;


			pR->K = kUndefined;			//	Kinematics function.

			memset ( pR->IKModName, 0, sizeof(pR->IKModName) );

			try 
			{
				GetWorld();
			}
			catch ( int ENoC )
			{
				ENo = ENoC;
			}

	//	NEFlags and nORB, oORB set when calling the CLibRec constructor.

			break;

		default:

			DelE ( E ( sW, "Unrecognized version (%d).  Deleting error.", 
						   Ver ) );
			bError = true;
	}

	if ( ENo )
	{	DelE ( T ( ENo, sW, "Deleting error." ) );
		bError = true; }

	if ( ! bError )
		Status |= D_CONSTRUCTEDOK;

}	//	CDGenRobot01::InitNewRecord()
*/

/*----------------------  CDGenRobot01::FixRecord()  -----------------------*/
/*

	2005-Dec-28		Keep around if something similar is needed in the future?

int		CDGenRobot01::FixRecord()
{
	int	ENo;	const char *	sW = F("CDGenRobot01::FixRecord()");

	//	First try updating this context.  Maybe another context has already
	//	fixed the record.
	//
	BOOL bUpdated = FALSE;

	if ( (ENo = UpdateContext ( bUpdated, FALSE )) != 0 )
	{
		return T ( ENo, sW, "Update context error." );
	}

	uint	nB;			BYTE *	pD = 0;

	if ( bUpdated )
	{
		try
		{
			pD = Get ( nB );

			pD = 0;		Put ( FALSE );

		}
		catch ( int ENoC )
		{
			if ( pD )  PutC();

			return T ( ENoC, sW, "Could not get/put after update." );
		}

		if ( nB >= SzSRGenRobot01() )  return 0;
	}

	//	Fix it.
	//
	try
	{
		//	First get the current DRM's values.
		//
		SRGenRobot01_v01	Buf;

		pD = Get ( nB );	
		
		SRGenRobot01_v01 *	pR1;
		
		pR1 = (SRGenRobot01_v01 *)(pD + DF_NUM_HDR_BYTES);

		memcpy ( &Buf, pR1, sizeof(SRGenRobot01_v01) );

		pD = 0;		Put ( FALSE );


		//	Duplicate the DRM.  Make it larger to accomodate the version
		//	header.
		//
		//	Copy the values into the new DRM.
		//
		DRMChgResn	DCR ( DCR_NONE );	//	Dup reason not needed.

		nB  = SzSRGenRobot01();

		pD  = DupGet ( DCR, nB ) + DF_NUM_HDR_BYTES;

		pR1 = (SRGenRobot01_v01 *)SetDataVersion ( pD, 1 );

		memcpy ( pR1, &Buf, sizeof(SRGenRobot01_v01) );


		//	Offer new DRM to other contexts.
		//
		uint *	pW0 = (uint *)pD;		DF_SET1(*pW0, DF_CHANGED);

		if ( (ENo = Offer ( pW0 )) != 0 )  throw ENo;


		pD = 0;		Put();				//	Changed.  Unlock.
	}
	catch ( int ENoC )
	{
		if ( pD )  PutC();

		return T ( ENoC, sW, "Returning error." );
	}

	return 0;

}	//	CDGenRobot01::FixRecord()
*/


//	RGenRobot01.cpp

