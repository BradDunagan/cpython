/*
		 1         2         3         4         5         6         7         8
12345678901234567890123456789012345678901234567890123456789012345678901234567890

	File:	RSPCallback.h

*/

#ifndef		__PECB_H
#define		__PECB_H


#include    "abb_rr_basic_typedefs.h"

#include    "abb_rr_RSPCallback.h"

//	PECB - ProcEle CallBack
//
//	A function provided to DLLs that they can call back in order to send 
//	messages to other threads.
//
//	The message is a ChanAMsg.
//
//	The function is provided to the DLL via the PECB structure.  The user's
//	library function calls the callback function passing the PECB structure
//	back.  Before the call the user would set the structure parameters.
//
//	Returns ENo.
//

#ifdef  WIN32
class	CWnd;
#else
#define CWnd    void
#endif

#define	PECB_CMD_MASK		0x00FFFFFF

#define	PECB_CMD_FLAGS_MASK	0xFF000000

//	Send a message.
//
#define	PECB_CMD_SEND		0x00000001

#define	PECB_CMD_NO_RSP		0x01000000


//	Have the BPW post a notification.
//
//	When the library calls the memory of iP should be set as follows:
//
//		DIDhC	Id and hC of data that has changed or been added.
//
//		int		The change type.  Eg., DCHG_REC_ADD.
//
//	The memory (iP) passed in the call will be freed in its function.
//
//	When the call returns the library iP will not be set (ie., nothing is
//	returned to the library).
//
#define	PECB_CMD_GEN_UPDT	0x00000100


//	To display status messages during long operations.
//
//	The memory of iP will be a 0 terminated text string.
//
//	The memory (iP) passed in the call will be freed in its function.
//
//	No return (iP on return will not be set).
//
#define	PECB_CMD_STATUS		0x00000200


//	Library is doing a long operation.  App should do windows messages.
//
#define	PECB_IN_PROGRESS	0x00000400


//	To synchronize multiple PEs with the viewport(s).  
//
//	Before moving a world entity SyncVPs() is called.  When all the positions
//	of entities have been set ReleaseVPs() is called.  If multiple PEs (or
//	the library code called by a PE) sync a the same time then no VPs are
//	redrawn until  * all *  those PEs release.
//
#define	PECB_SYNC_VPS		0x00000800

#define	PECB_RELEASE_VPS	0x00001000

#define	PECB_SYNC_VPS2		0x00002000

#define	PECB_RELEASE_VPS2	0x00004000

#define	PECB_SYNC_VPS3		0x00008000


//	2013-Oct-15		Callbacks from built-in library functions.
//
//	The first 4 bytes (an int) indicates the funciton to be done during 
//	the callback.  See BIL_CB_FNC_ definitions in bilib.h.
//
#define	PECB_BIL_CALLBACK	0x00010000



//	PECB Flags
//
//	Library function set this flag when it calls Notify() on some data.
//
//	R & S looks for this flag when running (as opposed to stepping).  If
//	set then R & S will break out of its tight bytecode execution loop
//	and do the notifications.
//
#define	PECBF_NOTIFICATION_POSTED		0x00000001

//	2011-Jan-17
//
//	To synchronize the VP updates when multiple PEs are moving entities
//	at the same time.
//
#define	PECBF_SYNCH_PES_REGISTER		0x00000002

#define	PECBF_SYNCH_PES_UNREGISTER		0x00000004


typedef struct PECB         PECB;

typedef	int (* PECBFunc) ( PECB * pCB );

#ifdef	WIN32
struct PORT	PECB		//	This is whats passed to the library function.
#else
struct		PECB
#endif
{
	//	PECB ( int PE_Id, PECBFunc, void *, void *,     CWnd * );
		PECB ( int PE_Id, PECBFunc, void *, PECallback, CWnd * );

	//	PECB ( int PE_Id, PECBFunc, void *, void *,     CWnd *, uint ChgFlags );
		PECB ( int PE_Id, PECBFunc, void *, PECallback, CWnd *, uint ChgFlags );

		int			PE_Id;

		PECBFunc	CallBack;	//	User's library function calls this 
								//	function passing back this structure in
								//	the call.

		//	Before the user calls CallBack() it sets these parameters and
		//
		//	CallBack() will free memory of iP that was set on the call.
		//
		//	When CallBack() returns iP may be set to other memory containing
		//	results.  Otherwise iP will be 0 on return.
		//
		//	2003-Nov-08	
		//
		//	Now this call-back facility may be used for additional purposes.
		//
		//	See PECB_CMD_GEN_UPDT, for example.
		//
		int			Cmd;		//	Usually PECB_CMD_SEND.  May be some other
								//	command that does not involve sending a 
								//	message - iP would be used to pass 
								//	parameters and results.

		int			DstSE_Id;	//	Destination element Id.  If -1 then sent 
								//	to the BPW.

		uint		iP;			//	On call this is the MMS memory containing 
								//	the message sent to the destination.  On 
								//	return this will be that message sent back 
								//	from the destination.

		uint		Flags;		//	2009-Jul-13		See PECBF_ macros.

		uint		ChgFlags;	//	2009-Dec-21		See DCHGF_ macros.

		CWnd *		pWnd;		//	2011-Jan-03		Calling window.

		void *		p;			//	2011-Jan-17		General pointer.


	//	void * 		Get_pPE() { return pPE; }

	//	void *		Get_cbPE() { return cbPE; }

//	private:
//
//		friend int	cpLibCallbackFunc ( PECB * pPECB );
//
//		friend int	  LibCallbackFunc ( PECB * pPECB );

		void *		pPE;		//	User library should not muck with this.

	//	void *		cbPE;		//	User library should not muck with this.
		PECallback	cbPE;		//	User library should not muck with this.

};	//	PECB



#endif          //  __PECB_H
