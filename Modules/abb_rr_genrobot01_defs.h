
#ifndef	__GENROBOT01_DEFS_H
#define	__GENROBOT01_DEFS_H

/*
//	Minimum number of joints.
//
#define	GR01_MIN_JOINTS			6

//	Maximum number of joints above 6.
//
#define	GR01_MAX_EJ				4

//	Maximum number of joints.
//
#define	GR01_MAX_JOINTS			(6 + GR01_MAX_EJ)
*/
//	Maximum number of joints.
//
//	Possible uses -
//
//		6 that can be solved with inverse kinematics
//
//		3 extra for translating, probably at the base
//
//		1 extra rotation, probably at the sholder
//
#define	GR01_MAX_JOINTS			10


//	2011-May-11
//
//	Values returned to the cmd record as a robot move result.
//
enum	MovToRtn
{
	//	Move performed ok.
	//
	mtrOK			= 0x00000000,

	//	A joint limit was encountered.  The bot will be reset back to its
	//	starting position.
	//
	mtrJointLimit	= 0x00000001,

	//	Failed to find an inverse kinematic solution for one of the points 
	//	along the path.
	//
	mtrInvKin		= 0x00000002

};	//	MovToRtn


enum	Kinematics
{
	kUndefined		= 0x00000000,

	kPaul01			= 0x00000001,

	kPaul02			= 0x00000002,

	kGen01			= 0x00000003,

	kSymKin			= 0x00000004,

};	//	Kinematics


#endif
