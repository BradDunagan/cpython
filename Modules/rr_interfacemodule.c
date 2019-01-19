
/*	Use this file as a template to start implementing a module that
	also declares object types. All occurrences of 'Xxo' should be changed
	to something reasonable for your objects. After that, all other
	occurrences of 'xx' should be changed to something reasonable for your
	module. If your module is named foo your sourcefile should be named
	foomodule.c.

	You will probably want to delete all references to 'attr' and add
	your own types of attributes instead.  Maybe you want to name your
	local variables other than 'self'.  If your object type is needed in
	other files, you'll have to create a file "foobarobject.h"; see
	floatobject.h for an example. */

//	The actual collections of things, such as UDUIs, is gotten, at certain
//	times, from the app.
//
//	The app provides some call(back)s for getting things.
//
//		getUIs()				->	a list of UI names
//		getControl()
//		getVPs()				->	a list of viewports
//		setLookAt()
//		getObject()
//		obj.pose.x = 3.0;
//		obj.setPose()
//
//	So this is a Robot Records record's interface with the app.
//
//	Maybe this module should be called -
//
//		rr_app
//		rrapp


#include "Python.h"
#include "structmember.h"

//	UDUI panes.
//
//		Two level dictionary - frame and pane.
//
//		Keys?
//			Names:		Possibly not unique. Enforce unique frame and pane
//						names?
//			ID:			Unique, but how to know?
//			What will be easy?
//	->		Use IDs for keys. And functions to find frames or panes by name, 
//			or other criteria.
//
//		{ <frameId>: { <paneId>: { ... },
//					   <paneId>: { ... },
//					   ... },
//		  <frameId>: { <paneId>: { ... },
//					   <paneId>: { ... },
//					   ... } }

//	Control  <--->  Variable
//
//		control:	a label, for example
//
//		variable:	x,y,z - an array, for example
//
//		How to update the control quickly when the variable changes?
//
//		Variable must have an attribute.  Looked for by byte code when the 
//		variable value is set. Call made to NoStack which calls back the app 
//		to update the control. Thats variable --> control.
//
//		control --> variable might/will be simply some CPython calls.
//
//		Such variables must be record (global) variables.
//
//		A property of the control will indiate the record ID an variable name.

//	Back to UDUI panes. How to, for example, change x,y (location) of a control
//	and see the result in the pane?  That is, change the control's value in
//	the dictionary representing the pane ...
//
//	->	The set...() function in rr_interface ?
//
//	The STORE_NAME bytecode is -
//
//		if (PyDict_CheckExact(ns))
//			err = PyDict_SetItem(ns, name, v);
//		else
//			err = PyObject_SetItem(ns, name, v);
//
//	Do this: Implement a class, name it, maybe, xy, and use it to represent the 
//	positions of controls.  Or implement a class, control, with attributes such 
//	as x, y, w, h, name, etc..

//	Is it worth it? Tracking record variables, detecting changes, callbacks and 
//	calls to app to, for example, set properties of UDUI controls.
//
//	->	Its not just fancy UDUI control property setting. Its command records
//		in general. Command record values must be synced when the same record
//		is instantiated in multiple processes. After all, a record is a record
//		and it must/should have the same values in multiple processes.
//
//	It is truely a bag of worms.
//
//	Good grief. Crap.

//	If there are multiple CPython threads - can a record be instantiated and
//	shared amongst threads?  That is, when a thread instantiates a record and 
//	its variable members - ints, lists, dicts, etc. - will/can other threads
//	that load/instantiate that same record affect those same member instances
//	in memory such that they are shared between the threads?

//	It seems that everthing about multiple instantiations of the same record
//	is bad. That is, the idea of a single, shared memory between multiple 
//	threads. But -
//
//		-	CPython already, by default, takes care of read/write issues with
//			the GIL.
//
//	If, then, the GIL could be "disabled" on non-record variables - function
//	locals. That might be something ...
//
//	It seems that CPthon should do that anyway because local variables are 
//	accessable by other functions. That is, they are in their own dict of 
//	the function frame. But -
//
//		-	It may be then that internal dict code, for example, that is not 
//			thread safe.
//
//		-	If so then can the dict code, for example, be made thread safe?
//
//	Also - what about -
//
//		-	Garbage collection and memory allocation/deallocation in general?
//
//	Will it be worth the trouble?  Worth the high probability that such
//	"enhancements" will not be successful?
//
//	Many attempts have been made to enhance multithreadability by mucking with 
//	things associated with/by the GIL.

//	The GIL, it seems, protects  * everything * (that is why it is Global) 
//	when an interpreter runs.
//
//	->	But why can't -
//
//		-	A chunck of memory be allocated for each thread so that a dict, 
//			for example, can be operated on in a thread-private chunk?
//
//	What happens when, for example, -
//
//		-	A new dict is created?
//
//		-	How is the memory allocated?
//
//	How hard would it be to reimpliment it?
//
//	->	Remember this -
//
//		-	The point is to run the interpreter free of having to worry
//			about the GIL.
//
//	->	So the question is -
//
//		-	Why can't the GIL be gotten only when -
//			-	allocating memory?
//			-	calling a native module?
//			-	etc.?
//
//	There are reasons. Otherwise it would not be an issue.

//	gdb -tui
//	https://stackoverflow.com/questions/39771932/gdb-tui-output-does-not-line-up
//
//	Show output in another terminal -
//
//		To get the tty name just type tty in that terminal.
//
//		For example, in ~/dev/myEmbdB/NoStack/, to debug NoS more nicely -
//	
//			$ gdb -tui -ex "tty /dev/pts/7" NoS

//	Objects/dictobject.c -
//
//		lines 529, 532
//		PyObject_MALLOC
//
//	Objects/obmalloc.c -
//		lines 2199
//
//	Python/pystate.c -
//		lines 1015			PyGILState_Check()

//	See, in the interpreter loop, -
//
//			if (_Py_atomic_load_relaxed(
//					&_PyRuntime.ceval.gil_drop_request))
//			{
//				...
//
//	What makes a GIL drop request? -
//
//		-	Any thread that wants to run anything in CPython.
//
//		-	"anything" includes -
//
//			-	Especially the interpreter.
//
//			-	Any memory allocation.
//
//			-	Etc..
//
//			-	See below.
//
//	Introduction to the GIL -
//
//		In eval_gil.h - see -
//
//			Notes about the implementation:
//
//			...
//
//	In ceval.c -
//
//	#define SET_GIL_DROP_REQUEST() 
//
//	Which is used/called by/in (ceval_gil.h) -
//
//	static void take_gil(PyThreadState *tstate)
//	{
//		...
//
//	Which is called by -
//
//		-	Fuctions in ceval.c.

//	What initially sets/establishes the GIL? -
//
//		#0  take_gil () 				at Python/ceval_gil.h:225
//		#1  PyEval_InitThreads () 		at Python/ceval.c:161
//		#2  _Py_InitializeCore_impl () 	at Python/pylifecycle.c:731
//		#3  _Py_InitializeCore () 		at Python/pylifecycle.c:859
//		#4  _Py_InitializeFromConfig () at Python/pylifecycle.c:1012
//		#5  Py_InitializeEx () 			at Python/pylifecycle.c:1049
//		#6  CPythonInit () 				at /home/brad/dev/rr-python/utils/utils.c:477
//		#7  main () 					at /home/brad/dev/rr-python/NoStack/NoStack.c:1752

//	What requests the GIL? 
//
//	The request is made by calling -
//
//		drop_gil()
//
//	Which is called by -
//
//		PyEval_ReleaseLock()
//		PyEval_ReleaseThread()
//		PyEval_SaveThread()
//		PyEval_RestoreThread()		//	when finalizing (terminating an interpreter?)
//		_PyEval_EvalFrameDefault()	//	when yielding to another thread
//
//	PyEval_ReleaseLock() is called by -
//		PyThreadState_DeleteCurrent()		pystate.c
//	Which is called by -
//		_threadmodule.c				//	presumably when a Python thread is ending
//		PyGILState_Release()		//	callec by alot of stuff
//
//	PyEval_ReleaseThread() is called by -
//		_testembed.c
//		thats it?
//
//	PyEval_RestoreThread() is called by
//		
//
//	PyEval_SaveThread()	is sometimes (mostly times) called via the macro -
//		Py_BEGIN_ALLOW_THREADS	
//	and that - by far - mostly by modules.

//	On memory allocation -
//
//	See, in obmalloc.c -
//
//		/* An object allocator for Python.
//		   ...

//	On garbage collection -
//
//	Seems to be implemented in a module -
//
//		Modules/gcmodule.c
//
//		/* This is the main function.  Read this to understand how the
//		* collection process works. */
//		static Py_ssize_t
//		collect(int generation, Py_ssize_t *n_collected, Py_ssize_t *n_uncollectable,
//				int nofail)
//
//	Also see, in objimp.h -
//
// 		* Garbage Collection Support
//
//	->	The question is this: What memory (blocks) does the GC work on?
//
//	->	For the "enhancement" to work -
//
//		-	There should be an area (name?) of memory for each thread's
//			private objects to be allocated from.
//
//		-	Maybe call it "private" memory.
//
//		-	Each thread with private allocations should have a GC that operates
//			on only the blocks allocated out of that memory.
//
//	->	Problem -
//
//		-	What happens when a thread-private object references an outside
//			object?
//
//			-	Can it be detected?
//
//			-	Is it an error?

PyObject * 			jsonMod;

static PyObject *	UIsObject;

static PyObject	* 	loadsFn;


//	A callback function is established by the app.
//
static PyTypeObject RRIFCallback_Type;

typedef struct {
	PyObject_HEAD
	PyCFunction		callback;
}	RRIFCallback;

static void
RRIFCallback_dealloc ( RRIFCallback * self )
{
//	printf ( "RRIFCallback_dealloc() self 0x%08X\n", (unsigned int)self );
	PyObject_Del ( self );
}

static PyTypeObject RRIFCallback_Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	.tp_name 		= "RRIFCallback",
	.tp_basicsize 	= sizeof(RRIFCallback),
	.tp_dealloc 	= (destructor)RRIFCallback_dealloc,
	.tp_flags 		= Py_TPFLAGS_DEFAULT,
};

static RRIFCallback *	rrCallback;


/* Function of two integers returning integer */

PyDoc_STRVAR(rr_interface_foo_doc,
"foo(i,j)\n\
\n\
Return the sum of i and j.");

static PyObject *
rr_interface_foo(PyObject *self, PyObject *args)
{
	long i, j;
	long res;
	if (!PyArg_ParseTuple(args, "ll:foo", &i, &j))
		return NULL;
	res = i+j; /* XXX Do something here */
	return PyLong_FromLong(res);
}


/* ---------- */

PyDoc_STRVAR(rr_interface_add_ui_doc,
"addUI ( frameId, paneId, jsonUI )\n\
\n\
Add the UI to the UI dictionary.");

static PyObject * 	rr_interface_add_ui ( PyObject * self, PyObject * args )
{
	//	Expected arguments -
	//		int		frameId
	//		int		paneId	
	//		string	JSON representation of the UI
	int	frameId, paneId;		const char * jsonUI;

		//	https://docs.python.org/3/c-api/arg.html?highlight=pyarg_parsetuple#parsing-arguments
	if ( ! PyArg_ParseTuple ( args, "lls:addUI", &frameId, &paneId, &jsonUI ) )
		return NULL;

//	printf ( "rr_interface_add_ui(): frameId %d  paneId %d  jsonUI %s\n", 
//			 frameId, paneId, jsonUI );


	PyTupleObject * targs = (PyTupleObject *)args;
	PyDictObject *	duis = (PyDictObject *)UIsObject;
	PyObject *		key_frameId = PyTuple_GetItem ( targs, 0 );
	PyObject *		key_paneId	= PyTuple_GetItem ( targs, 1 );
	PyObject *		s_jsonUI 	= PyTuple_GetItem ( targs, 2 );

	PyDictObject *	dframes	= NULL;

	//	Find the frame dictionary in the UIs dictionary. If not there then
	//	insert one.
	if ( ! PyDict_Contains ( duis, key_frameId ) ) {
		dframes = (PyDictObject *)PyDict_New();
		//	Add it to the UIs.
		PyDict_SetItem ( duis, key_frameId, dframes );
	} else {
		dframes = (PyDictObject *)PyDict_GetItem ( duis, key_frameId ); }
	
	//	Make a dictionary item out of the jsonUI.
	PyObject* pPosArgs = PyTuple_New( 1 ) ;
	Py_INCREF(s_jsonUI);
	PyTuple_SetItem ( pPosArgs, 0, s_jsonUI );
	PyObject *	di  = PyObject_Call ( loadsFn , pPosArgs , NULL ) ;

	//	Set the pane item to the UI.
	PyDict_SetItem ( dframes, key_paneId, di );
	


	Py_INCREF(Py_None);
	return Py_None;
}	//	rr_interface_add_ui()


PyDoc_STRVAR(rr_interface_set_callback_doc,
"setCallback ( callback )\n\
\n\
Establish application callback.");

static PyObject * 	rr_interface_set_callback ( PyObject * self, PyObject * args )
{
	//	Expected arguments -
	//		PyCFunction		callback
	//
	//	Docs on format string -
	//	https://docs.python.org/3/c-api/arg.html?highlight=pyarg_parsetuple#parsing-arguments

	RRIFCallback * CB;
	if ( ! PyArg_ParseTuple ( args, "O:setCallback", &CB ) )
		return NULL;

	rrCallback = PyObject_New ( RRIFCallback, &RRIFCallback_Type );

	if ( rrCallback == NULL )
		return NULL;

	rrCallback->callback = CB->callback;

	Py_INCREF(Py_None);
	return Py_None;
}	//	rr_interface_set_callback()


//	Note and Know -
//
//	Container objects (objects which reference other objects) must support
//	garbage collection!  Follow the rules!
//
//		->	https://docs.python.org/3/c-api/gcsupport.html
//
typedef struct {
    PyObject_HEAD
    PyObject            * attr;        /* Attributes dictionary */
} control_object;

static PyTypeObject control_type;

static PyObject *
control_new ( PyTypeObject * type, PyObject * args, PyObject * kwds )
{
    control_object *	control;

//	printf ( "control_new()\n" );

	//	Call back the app to get the frameId, paneId and controlId.
	//	No.  Instead: Each frame may have at most one PE. So the PE and its
	//	frameId are linked. Likewise, each frame may have at most one UDUI 
	//	pane.
	//	So the app will gather what frame and pane a callback is applied 
	//	to by association with the PE.
	//	Panel? - But what about the panel the control goes on? There will be 
	//	multiple panels when the UDUI has pages or tabs.
	//	The interface needs a way of specifying the UDUI panel a new control 
	//	is to be added to.

	//	App wide, its -
	//		frame
	//			pane
	//				panel

	//	Now its -
	//		import rr_interface as rr;
	//		...
	//		c = rr.control()

	//	The problem is knowing how to identify and specify the frame, pane and
	//	panel in a command record.  ID?  Name?
	//	Does it matter?  The ID or Name can be maintained in a record variable.
	//	If wrong the statement will simply error - and the user can determine 
	//	and apply the correction.

	//		f = rr.getFrame ( ID or Name );		//	exception if not found.

	//	Found from what?  UIsObject?  Each process (PE/cpython) maintains a
	//	* copy *?

	//	Multiple cpython threads? No. The GIL makes multiple threads execute 
	//	sequentially - no speed up.

	//	It does not appear that an address of a SharedArrayBuffer can be had.
	//	That is, no sharing of memory between workers can be done.

	//	Maybe this -
	//	Track what changes.

	//	The problem is this: Large amount of data - examples -
	//		command records
	//		world objects
	//		UDUIs
	//	Each CPython worker must have a copy of these. Good Grief.
	//
	//	Command Records -
	//		Can it be detected when a record variable is instantiated, value 
	//		changes? If yes then message that to other workers.
	//
	//	Bite the bullet. No shared memory. Duplicates of the same data in 
	//	multiple processes. Its the browser, protected memory. Deal with it.
	//
	//	Generalize to a record. That is, maintain UDUI data as a command record 
	//	in CPython/rr_interfacemodule.

	//	Command Records and Tracking Changes -
	//
	//	Set an attribute in CR (module).  See utils.c CreateRecord().  Run 
	//	NoStack RecDef1b() for an example of how CreateRecord() is called.
	//	Maybe not necessary. CreateRecord() already sets __file__ to 
	//	">rr-record<".
	//
	//	So, track changes of module variables and their values when the 
	//	module attribute __file__ is set to ">rr-record<".
	//
	//	Look at bytecode STORE_NAME. There -
	//		Check module __file__ attribute for ">rr-record<"
	//		If  false  then
	//			no worries, do nothing special, continue
	//		Call back specifying what has changed and the new value.
	//
	//	The callback (probably something in utils.c) will buffer all the 
	//	changes and provide-them-back/make-them-accessable to the JS app 
	//	on return from the JS app call (step, for example).
	//
	//	Also look at STORE_ATTR and STORE_SUBSCR.  There are probably others.
	//
	//	See/run RecDef2a() in NoStack.c.
	//
	//	When a variable value changes how is it determined that the variable
	//	is a CR member?
	//		-	When CR instance is created create an attribute in each of its
	//			members.  Maybe __cr__, set it's value to the ID of the module
	//			(use the builtin id() function or just the C pointer of the 
	//			module object).
	//		-	Do this in utils.c CreateRecord().
	//		-	The attribute will need to be set on  * all *  objects? That,
	//			is, for example, all items of a tuple?



	//	This -
	//		process-shared chunk(s) of memory - SharedArrayBuffer
	//	https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer
	//	https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Atomics
	//	https://github.com/tc39/ecmascript_sharedmem/blob/master/TUTORIAL.md
	//	Allocated in JS. 
	//	How is it interpreted on the C side?  I think its this -
	//		On the JS side - got to set/get bytes.  int, float, etc.
	//		Each value is a fixed size - so there locations in the buffer can 
	//		be tracked.
	//		Likewise on the C side.
	//	https://developer.mozilla.org/en-US/docs/Web/JavaScript/Typed_arrays





	//	Arguments -
	//		type [, x, y, w, h, name]
	const char * controlType = 0;
	int x = 10, y = 10, w = 34, h = 18;
	const char * controlName = 0;
	if ( ! PyArg_ParseTuple ( args, "s|lllls:control", &controlType,
													   &x, &y, &w, &h,
													   &controlName ) )
		return NULL;

	if (	(strcmp ( controlType, "panel" )  != 0)
		 &&	(strcmp ( controlType, "label" )  != 0)
		 &&	(strcmp ( controlType, "button" ) != 0)
		 &&	(strcmp ( controlType, "input" )  != 0)
		 &&	(strcmp ( controlType, "list" )   != 0) ) {
		PyErr_SetString ( PyExc_TypeError,
						  "unrecognized control type");
		return NULL; }

    control = (control_object *)type->tp_alloc ( type, 0 );

	int bOk = 0;
	while ( 1 ) {
		if ( control == NULL )
			break;

		PyObject * dict = control->attr = PyDict_New();
		if ( dict == NULL ) 
			break;

		if ( PyDict_SetItemString ( dict, "controlType", 
									PyUnicode_FromString ( controlType ) ) ) 
			break;
		if ( PyDict_SetItemString ( dict, "x", PyLong_FromLong ( x ) ) ) 
			break;
		if ( PyDict_SetItemString ( dict, "y", PyLong_FromLong ( y ) ) ) 
			break;
		if ( PyDict_SetItemString ( dict, "w", PyLong_FromLong ( w ) ) ) 
			break;
		if ( PyDict_SetItemString ( dict, "h", PyLong_FromLong ( h ) ) ) 
			break;
		if ( 	controlName
			 && PyDict_SetItemString ( dict, "controlName", 
			 						   PyUnicode_FromString ( controlName ) ) ) 
			break;
		bOk = 1;
		break;
	}	//	while ( 1 )

	if ( (! bOk) && control ) {
		Py_DECREF(control);
		return NULL; } 

    return (PyObject *)control;
}	//	control_new()

static int
control_init ( control_object * control, PyObject * args, PyObject * kwdargs )
{
//	printf ( "control_init()\n" );
	
	return 0;
}	//	control_init()

static void
control_dealloc ( control_object * control )
{
//	printf ( "control_dealloc()\n" );
	
//	PyObject_GC_UnTrack ( control );

	if ( control->attr ) {
		Py_DECREF(control->attr); }

//	Py_TYPE(control)->tp_free ( control );
}

static PyObject *
control_getattro ( control_object * self, PyObject * name )
{
//	printf ( "control_getattr()\n" );
	
	if ( self->attr != NULL ) {
		PyObject * v = PyDict_GetItem ( self->attr, name );
		if ( v != NULL ) {
			Py_INCREF(v);
			return v; }
	}
	return PyObject_GenericGetAttr ( ( PyObject *)self, name );
}	//	control_getattro()

static int
control_setattr ( control_object * self, const char * name, PyObject * v )
{
//	printf ( "control_setattr()\n" );

	if ( self->attr == NULL ) {
		self->attr = PyDict_New();
		if ( self->attr == NULL )
			return -1;
	}
	if ( v == NULL ) {
		int rv = PyDict_DelItemString ( self->attr, name );
		if ( rv < 0 )
			PyErr_SetString ( PyExc_AttributeError,
							  "delete non-existing control attribute");
		return rv;
	}
	else {
		while ( rrCallback ) {
			//	Control must have (at least) these attributes set -
			//		controlType		str
			//		frameId			int
			//		paneId			int
			//		controlId		int
			//	If those are set then we send those and these in the call -
			//		name
			//		v
			PyObject *	controlType = PyDict_GetItemString ( self->attr, "controlType" );
			if ( ! controlType ) {
				break; }
			PyObject *	frameId = PyDict_GetItemString ( self->attr, "frameId" );
			if ( ! frameId ) {
				break; }
			PyObject *	paneId = PyDict_GetItemString ( self->attr, "paneId" );
			if ( ! paneId ) {
				break; }
			PyObject *	controlId = PyDict_GetItemString ( self->attr, "controlId" );
			if ( ! controlId ) {
				break; }

			//	Furthermore - the name of the attribute being set should not be any
			//	of those.
			if (    (strcmp ( name, "controlType" ) == 0)
				 || (strcmp ( name, "frameId" ) == 0)
				 || (strcmp ( name, "paneId" ) == 0)
				 || (strcmp ( name, "controlId" ) == 0) ) {
				break; }

			PyObject *	pPosArgs = PyTuple_New ( 6 ) ;
			PyTuple_SetItem ( pPosArgs, 0, controlType );
			PyTuple_SetItem ( pPosArgs, 1, frameId );
			PyTuple_SetItem ( pPosArgs, 2, paneId );
			PyTuple_SetItem ( pPosArgs, 3, controlId );
			PyTuple_SetItem ( pPosArgs, 4, PyUnicode_FromString ( name ) );	//	propertyName
			Py_INCREF(v);
			PyTuple_SetItem ( pPosArgs, 5, v );								//	propertyValue

			PyObject * rtn = rrCallback->callback ( (PyObject *)self, pPosArgs );
			Py_DECREF(pPosArgs);
			if ( rtn ) {
				Py_DECREF(rtn); }
			else {
				return -1; }
			break;
		}

		return PyDict_SetItemString ( self->attr, name, v);
	}
}	//	control_setattr()

static PyMemberDef control_members[] = {
    {"properties", T_OBJECT,
     offsetof(control_object, attr),         READONLY,
     PyDoc_STR("Control properties.")},
    {NULL}
};

static PyMethodDef control_methods[] = {
    {NULL,              NULL}   /* sentinel */
};

PyDoc_STRVAR(control_doc,
"control(type [, x, y, w, h, name]) --> control object\n\
\n\
A button, label, list, input (or, what else?).");

static PyTypeObject control_type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	.tp_name = "rr_interface.control",             /* tp_name */
	.tp_basicsize = sizeof(control_object),             /* tp_basicsize */
//	0,                                  /* tp_itemsize */
	/* methods */
	.tp_dealloc = (destructor)control_dealloc,        /* tp_dealloc */
//	0,                                  /* tp_print */
//	0,                                  /* tp_getattr */
    .tp_setattr = (setattrfunc)control_setattr, 		/* tp_setattr */
//	0,                                  /* tp_reserved */
//	0,			                        /* tp_repr */
//	0,									/* tp_as_number */
//	0,									/* tp_as_sequence */
//	0,                                  /* tp_as_mapping */
//	0,									/* tp_hash */
//	0,                                  /* tp_call */
//	0,                                  /* tp_str */
	.tp_getattro = (getattrofunc)control_getattro,		/* tp_getattro */
//	0,                                  /* tp_setattro */
//	0,                                  /* tp_as_buffer */
//	.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
	.tp_flags = Py_TPFLAGS_DEFAULT,
										/* tp_flags */
	.tp_doc = control_doc,				/* tp_doc */
//	0,									/* tp_traverse */
//	0,									/* tp_clear */
//	0,									/* tp_richcompare */
//	0,									/* tp_weaklistoffset*/
//	0,									/* tp_iter */
//	0,                                  /* tp_iternext */
	.tp_methods = control_methods,		/* tp_methods */
	.tp_members = control_members,		/* tp_members */
//	0,									/* tp_getset */
//	0,                                  /* tp_base */
//	0,                                  /* tp_dict */
//	0,                                  /* tp_descr_get */
//	0,                                  /* tp_descr_set */
//	0,                                  /* tp_dictoffset */
	.tp_init = (initproc)control_init,	/* tp_init */
	.tp_alloc = PyType_GenericAlloc,	/* tp_alloc */
	.tp_new = control_new,				/* tp_new */
	.tp_free = PyObject_GC_Del,			/* tp_free */
};



/* List of functions defined in the module */

static PyMethodDef rr_interface_methods[] = {

	{"foo",             rr_interface_foo,         	METH_VARARGS,
		rr_interface_foo_doc},

	{"setCallback",		rr_interface_set_callback,	METH_VARARGS,
		rr_interface_set_callback_doc},

	{"addUI",           rr_interface_add_ui,		METH_VARARGS,
		rr_interface_add_ui_doc},



	{NULL,              NULL}           /* sentinel */
};

PyDoc_STRVAR(module_doc,
"This is a template module just for instruction.");


static int
rr_interface_exec(PyObject *m)
{
	/* Slot initialization is subject to the rules of initializing globals.
	   C99 requires the initializers to be "address constants".  Function
	   designators like 'PyType_GenericNew', with implicit conversion to
	   a pointer, are valid C99 address constants.

	   However, the unary '&' operator applied to a non-static variable
	   like 'PyBaseObject_Type' is not required to produce an address
	   constant.  Compilers may support this (gcc does), MSVC does not.

	   Both compilers are strictly standard conforming in this particular
	   behavior.
	*/

	/* Finalize the type object including setting type of the new type
	 * object; doing it here is required for portability, too. */


	UIsObject = PyDict_New();
	if ( UIsObject == NULL ) {
		goto fail; }

	Py_INCREF(UIsObject);
	PyModule_AddObject ( m, "uis", UIsObject );


	jsonMod = PyImport_ImportModule ( "json" );
	if ( ! jsonMod ) {
		goto fail; }

	loadsFn = PyObject_GetAttrString ( jsonMod, "loads" ) ;
	if ( ! loadsFn ) {
		goto fail; }

	if ( PyType_Ready ( &control_type ) < 0 )
		goto fail;
	Py_INCREF ( &control_type );
	PyModule_AddObject ( m, "control", (PyObject *)&control_type );


	return 0;
 fail:
	Py_XDECREF(m);
	return -1;
}

static struct PyModuleDef_Slot rr_interface_slots[] = {
	{Py_mod_exec, rr_interface_exec},
	{0, NULL},
};

static struct PyModuleDef rr_interfacemodule = {
	PyModuleDef_HEAD_INIT,
	"rr_interface",
	module_doc,
	0,
	rr_interface_methods,
	rr_interface_slots,
	NULL,
	NULL,
	NULL
};

/* Export function for the module (*must* be called PyInit_rr_interface) */

PyMODINIT_FUNC
PyInit_rr_interface(void)
{
	return PyModuleDef_Init(&rr_interfacemodule);
}
