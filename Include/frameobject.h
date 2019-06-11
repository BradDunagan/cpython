
/* Frame object interface */

#ifndef Py_LIMITED_API
#ifndef Py_FRAMEOBJECT_H
#define Py_FRAMEOBJECT_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int b_type;                 /* what kind of block this is */
	int b_handler;              /* where to jump to find handler */
	int b_level;                /* value stack level to pop to */
} PyTryBlock;

#define	BRADDS_F_FLAGS_NO_STACK			0x00000001
#define BRADDS_F_FLAGS_FIRST_INSTR		0x00000002
#define BRADDS_F_FLAGS_CALL_FUNCTION	0x00000004
#define	BRADDS_F_FLAGS_RETURN			0x00000008
#define	BRADDS_F_FLAGS_NEW_FRAME		0x00000010
/* The idea of this "native call" flag is that it is set when a call is 
   made that does not create a  new frame.  When it is not set upon a return 
   then we know we need to clean things up and prepare to switch back to 
   the calling frame. */
#define BRADDS_F_FLAGS_NATIVE_CALL		0x00000020
#define BRADDS_F_FLAGS_JUMP_OFF_FOR		0x00000040
#define BRADDS_F_FLAGS_CREATE_RECORD	0x00000080
#define BRADDS_F_FLAGS_UI_CALL			0x00000100
#define BRADDS_F_FLAGS_VP_CALL			0x00000200
#define	BRADDS_F_FLAGS_WORLD_CALL		0x00000400

typedef struct _frame {
	PyObject_VAR_HEAD
	struct _frame *f_back;      /* previous frame, or NULL */
	PyCodeObject *f_code;       /* code segment */
	PyObject *f_builtins;       /* builtin symbol table (PyDictObject) */
	PyObject *f_globals;        /* global symbol table (PyDictObject) */
	PyObject *f_locals;         /* local symbol table (any mapping) */
	PyObject **f_valuestack;    /* points after the last local */
	/* Next free slot in f_valuestack.  Frame creation sets to f_valuestack.
	   Frame evaluation usually NULLs it, but a frame that yields sets it
	   to the current stack top. */
	PyObject **f_stacktop;
	PyObject *f_trace;          /* Trace function */
	char f_trace_lines;         /* Emit per-line trace events? */
	char f_trace_opcodes;       /* Emit per-opcode trace events? */

	/* Borrowed reference to a generator, or NULL */
	PyObject *f_gen;

	int f_lasti;                /* Last instruction if called */
	/* Call PyFrame_GetLineNumber() instead of reading this field
	   directly.  As of 2.3 f_lineno is only valid when tracing is
	   active (i.e. when f_trace is set).  At other times we use
	   PyCode_Addr2Line to calculate the line from the current
	   bytecode index. */
	int f_lineno;               /* Current line number */
	int f_iblock;               /* index in f_blockstack */
	char f_executing;           /* whether the frame is still executing */
	PyTryBlock f_blockstack[CO_MAXBLOCKS]; /* for try and loop blocks */
	
	int bradds_f_flags;			/*	See BRADDS_F_FLAGS_ defines */
	const _Py_CODEUNIT * bradds_f_next_instr;
	int					 bradds_f_next_instri;
	int bradds_f_oparg;			/*	Used after return from another frame */
//	PyObject **bradds_f_stack_pointer;	/* To maintain during a call */
//	PyObject * bradds_async_arg;

	PyObject *f_localsplus[1];  /* locals+stack, dynamically sized */
} PyFrameObject;


/* Standard object interface */

PyAPI_DATA(PyTypeObject) PyFrame_Type;

#define PyFrame_Check(op) (Py_TYPE(op) == &PyFrame_Type)

PyAPI_FUNC(PyFrameObject *) PyFrame_New(PyThreadState *, PyCodeObject *,
										PyObject *, PyObject *);

/* only internal use */
PyFrameObject* _PyFrame_New_NoTrack(PyThreadState *, PyCodeObject *,
									PyObject *, PyObject *);


/* The rest of the interface is specific for frame objects */

/* Block management functions */

PyAPI_FUNC(void) PyFrame_BlockSetup(PyFrameObject *, int, int, int);
PyAPI_FUNC(PyTryBlock *) PyFrame_BlockPop(PyFrameObject *);

/* Extend the value stack */

PyAPI_FUNC(PyObject **) PyFrame_ExtendStack(PyFrameObject *, int, int);

/* Conversions between "fast locals" and locals in dictionary */

PyAPI_FUNC(void) PyFrame_LocalsToFast(PyFrameObject *, int);

PyAPI_FUNC(int) PyFrame_FastToLocalsWithError(PyFrameObject *f);
PyAPI_FUNC(void) PyFrame_FastToLocals(PyFrameObject *);

PyAPI_FUNC(int) PyFrame_ClearFreeList(void);

PyAPI_FUNC(void) _PyFrame_DebugMallocStats(FILE *out);

/* Return the line of code the frame is currently executing. */
PyAPI_FUNC(int) PyFrame_GetLineNumber(PyFrameObject *);

#ifdef __cplusplus
}
#endif
#endif /* !Py_FRAMEOBJECT_H */
#endif /* Py_LIMITED_API */
