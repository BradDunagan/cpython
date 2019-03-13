/*
		 1         2         3         4         5         6         7         8
12345678901234567890123456789012345678901234567890123456789012345678901234567890

	File:	RSPCallback.h

*/

#ifndef		__RSPCALLBACK_H
#define		__RSPCALLBACK_H



/*------------------------------- Dependencies  ------------------------------*/

//	End Dependencies


#define PORT

/*
class	CProcEle3;
*/
#ifdef	WIN32
class	CSysEle2;
class	CObjERCtrl;
#else
#define	CSysEle2	void
#define	CObjERCtrl	void
#endif


//	2008-Jan-23		PE callback.

struct PORT	PECBParams
{
	int	i0, i1, i2, i3, i4, i5, i6, i7, i8;

	void *	p0, * p1, * p2, * p3;

};	//	PECBParams

//	Call CProcEle3::RmvOVDs()
//
//		void *		CDVals *.
//
//	Return ENo.
//
#define	PECBF_RmvOVDs				0x00000001

//	Get CProcEle3::bRunning
//
//	Return bRunning.
//
#define	PECBF_bRunning				0x00000002

//	Get CProcEle3::bLoadRngSrc
//
//	Return bLoadRngSrc.
//
#define	PECBF_bLoadRngSrc			0x00000003

//	Is command record view frame (OVF) open?
//
//	Return BOOL.
//
#define	PECBF_bOVFOpen				0x00000004

//	Call CProcEle3::GetTextLineColumn()
//
//		void *		PECBParams - 
//
//						i0		In.   iChar offset in buffer.
//						i1		Out.  iLine
//						i2		Out.  iCol
//
//	Return ENo.
//
#define	PECBF_GetTextLineColumn		0x00000005

//	Call CProcEle3::AddSub()
//
//		void *		PECBParams - 
//
//						i0		Sub Id.
//						i1		Sub iStart.
//
//	Returns ENo.
//
#define	PECBF_AddSub				0x00000006

//	Call CProcEle3::AddParPE()
//
//		void *		PECBParams -
//
//						i0			  CDNtrp *	pParent
//						i1		      int		SubPtr 
//						i2			  TknType	SubTT
//						i3		const DID2 &	GlobalValsId
//						i4		const DID2 &	ValsId
//						i5			  int		ProgPtr
//						i6			  LID		SubCP_Id
//						i7			  int &		SubPE_Id
//
//	Returns ENo.
//
#define	PECBF_AddParPE				0x00000007

//	Call CProcEle3::SendBkPtsToSub()
//
//		void *		PECBParams -
//
//						i0			  int		SubPE_Id
//						i1		      LID		ParBlkCPId 
//						i2			  LID		SubCPId
//
//	Returns ENo.
//
#define	PECBF_SendBkPtsToSub		0x00000008

//	Call CProcEle3::ShouldOnlyStepSub0()
//
//	Returns BOOL.
//
#define	PECBF_ShouldOnlyStepSub0	0x00000009

//	Call CProcEle3::Get_nOtherSubs()
//
//	Returns uint.
//
#define	PECBF_Get_nOtherSubs		0x0000000A

//	Call CProcEle3::DoOtherSubs()
//
//		void *		PECBParams -
//
//						i0			  BOOL		
//						i1		      CCodePieces *
//
//	Returns status (rspERROR, etc).
//
#define	PECBF_DoOtherSubs			0x0000000B

//	Call CProcEle3::IsSub0Finished()
//
//	Returns BOOL.
//
#define	PECBF_IsSub0Finished		0x0000000C

//	Call CProcEle3::IsSub0StepEnabled()
//
//	Returns BOOL.
//
#define	PECBF_IsSub0StepEnabled		0x0000000D

//	Call CProcEle3::SetSubFlags()
//
//		void *		PECBParams -
//
//						i0			  int	SubId 
//						i1		      uint	WhichFlags
//						i2		      uint	Flags
//
//	Returns ENo.
//
#define	PECBF_SetSubFlags			0x0000000E

//	Call CProcEle3::RemoveSub0()
//
//	Returns void.
//
#define	PECBF_RemoveSub0			0x0000000F

//	Call CProcEle3::CallBack()
//
//		void *		PECB *
//
//	Returns ENo.
//
#define	PECBF_CallBack				0x00000010

//	Get CProcEle3::Id
//
//	Returns PE's element Id.
//
#define	PECBF_Id					0x00000011

//	Call CProcEle3::SendErrorToParent().
//
//	Send error message to parent PE.
//
//	Returns ENo.
//
#define	PECBF_SubErrStr				0x00000012


//	Call CProcEle::IsAltEventFired().
//
//	Returns ENo.
//
#define	PECBF_IsAltEventFired		0x0000001F


//	Call CProcEle::ResetAltEvent().
//
//	Returns ENo.
//
#define	PECBF_ResetAltEvent			0x00000020


//	Get PE's change notification flags.
//
//	Returns PE's change notification flags.
//
#define	PECBF_ChgFlags				0x00000022


//	Returns the command record view frame (OVF) pointer.
//
//	May be 0.
//
#define	PECBF_pOVF					0x00000023


//	2013-Jul-02		Save a (root) cmd rec's UDUI.
//
//		void *		The root cmd rec odef.
//
//	Returns ENo.
//
#define	PECBF_SaveUDUI				0x00000024


//	2013-Jul-02		Load a (root) cmd rec's UDUI.
//
//	Returns ENo.
//
#define	PECBF_LoadUDUI				0x00000025



/*
typedef	int	(* PECallback) ( CProcEle3 * pPE, int Func, void * );
*/
typedef	int	(* PECallback) ( CSysEle2 *  pPE, int Func, void * );



//	2008-Jan-23		Command record error view (OERV) callback.

//	Call CObjERCtrl::Print().
//
//		void *		printf() like format string.
//
//	Return void.
//
#define	OERVCB_Print				0x00000001


typedef	int	(* OERVCallback) ( 		 CObjERCtrl *	pOERV, 
							   		 int 			Func,  
							   const char * 		fmt, ... );

//	2011-Aug-30
//
//	Error/Status callback that routes to both the OERV (if the UI is open)
//	and/or the remote status buffer (when executing a command from a remote
//	PE.
//
//		void *		printf() like format string.
//
//	Returns ENo.
/*
typedef	int	(* ERCallback) ( CProcEle3 * pPE, void *, ... );
*/
typedef	int	(* ERCallback) ( CSysEle2 *  pPE, void *, ... );




#endif

//	RSPCallback.h
