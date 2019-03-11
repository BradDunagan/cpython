/*
		 1         2         3         4         5         6         7		   8
12345678901234567890123456789012345678901234567890123456789012345678901234567890

	File:	RGenRobot01.h
	
	Record data structure.
*/

#ifndef		__RGENROBOT01_H
#define		__RGENROBOT01_H


#include	"abb_rr_genrobot01_defs.h"


struct	DiA0		//	Default or Initial link position WRT to its parent.
{
	void	Reset() { TO = toRT;	X = Y = Z = AX = AY = AZ = 0; }

	TransformOrder	TO;
	
	double	X;
	double	Y;
	double	Z;		//	Z is replaced with translating joint variable.
	double	AX;		//	Angles are stored in radians.
	double	AY;
	double	AZ;		//	AZ is replaced with rotating joint variable.

};	//	DiA0


enum	JointType
{
	jtUndefined			= 0x00000000,

	jtNoInvKin			= 0x00001000,	//	Joint is const in inv. kinematics.

	jtRotating			= 0x00000001,
	jtRotatingNIK		= 0x00001001,	//	No inv. kinematics.

	jtTranslating		= 0x00000002,
	jtTranslatingNIK	= 0x00001002,	//	No inv. kinematics.

};	//	JointType


//	General Robot Grasp Flags
//
//	Indicates that what is grasped is a camera.  So, when the robot moves
//	it must do a DCHG_VIEWPOINT notification specifying the viewpoint in
//	the extra data of the Notify() call.
//
#define	GRGF_CAMERA				0x00000001


//	Current version of the record.
//
struct	SRGenRobot01_v02
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

	uint	GrspFlags;						//	See GRGF_ definitions.

	int		nJoints;


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

	//	2010-Aug-12
	//
	//	Don't want to keep the record locked during a move.
	//
	//	But we don't want other contexts messing with the record during a 
	//	move.
	//
	//	So we buffer this structure and set this flag.  Onther contexts
	//	must check this flag and if set they must unlock the record and
	//	wait and/or do a callback on the app.
	//
	//	If the wait is too long the callback should probably be a
	//	PECB_CMD_STATUS and there might aught to be a way to cancel the
	//	action.
	//
	BOOL	bBusy;		char BusyMsg [80];


};	//	SRGenRobot01_v02;


#endif

//	RGenRobot01.h

