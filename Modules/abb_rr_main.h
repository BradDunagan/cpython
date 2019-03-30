
#include    "abb_rr_libarg.h"

struct	P6;

//extern "C"
//{
//	Identifies the robot - by name - which other robot calls are applied to.
//
//	Prompts for entities that represent the robot base, links, fingers and
//	grip origin.
//
//	DLL Args -
//
//		char *			Robot name.
//
/*
PORT Ret *	IdRbt ( PECB * pCB, CData * pObj, int *, LibArg * );
*/
int		RR_Test ( const char * s, int i );

int		IdRbt ( PECB * pCB, const char * Name );

int		ConfigureLink ( PECB *		pCB, 
						int			iJ,
						int			EId,
						int 		bRot,
						int			bKin,
						double		Theta,
						double		Dist,
						double		Length,
						double		Alpha );

int		MovTo ( PECB * pCB, P6 * 	pB6, 	//	Base WRT system
					//	    P6 * 	pL6, 	//	Last link WRT system
						    P6 * 	pPs ); 	//	Last link target WRT system
//}


