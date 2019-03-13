/*
		 1         2         3         4         5         6         7		   8
12345678901234567890123456789012345678901234567890123456789012345678901234567890

	File:	World.h

	2008-Dec-01
*/

#ifndef		__WORLD_H
#define		__WORLD_H

#include    "abb_rr_rs_typedefs.h"

#include    "abb_rr_m4.h"


//	2009-Jul-10		VP_AFR		ViewPoint-At-From-Roll
//
//	Viewpoint as specified with a point looking at and a point looking from.
//	Roll also.
//
//	Don't know where to put this.  Needs to be in a place commonly used
//	by the app and user libraries.
//
//	Right now GenRobot01 sends a DCHG_VIEWPOINT notification to the World
//	when a camera entity moves.  That is, when a camera moves the viewpoint
//	changes.
//
struct	VP_AFR
{
	LID		EId;		//	Camera entity Id.

	uint	Spare;

	double	AtX,  AtY,  AtZ;
	double	FmX,  FmY,  FmZ;

	double	Roll;		//	Degrees.
};


//	2011-Jan-14, 18
//
struct	WorldEntA
{
	LID		EId;

	CM4		A;

};	//	WorldEntA

struct PORT	WEAVector
{
	WEAVector() { nEnts = 0;	pEnts = 0; }

   ~WEAVector() { if ( pEnts )  delete [] pEnts; }

	int		nEnts;

	WorldEntA *	pEnts;

};	//	WEAVector

struct	WorldEntPaul
{
	LID		EId;

	double	Theta, Dist, Length, Alpha;

};	//	WorldEntPaul

struct PORT	WEPVector
{
	WEPVector() { nEnts = 0;	pEnts = 0; }

   ~WEPVector() { if ( pEnts )  delete [] pEnts; }

	int		nEnts;

	WorldEntPaul *	pEnts;

};	//	WEPVector

//	2011-Feb-21
//
struct PORT RbtMovCBData
{
	RbtMovCBData() { PE_Id = 0;	pWEAV = 0;	pWEPV = 0;  pAFR = 0; }

   ~RbtMovCBData() { 
	   if ( pWEAV )  delete pWEAV;
	   if ( pWEPV )  delete pWEPV;
	   if ( pAFR )  delete pAFR; 
	}

   int			PE_Id;

	WEAVector *	pWEAV;

	WEPVector * pWEPV;

	VP_AFR *	pAFR;

};	//	RbtMovCBData


#endif

//  world.h
