
/*
	See -
		https://docs.python.org/3.7/extending/
			https://docs.python.org/3.7/extending/extending.html
			https://docs.python.org/3.7/extending/newtypes_tutorial.html
			https://docs.python.org/3.7/extending/newtypes.html
			https://docs.python.org/3.7/extending/building.html

		https://docs.python.org/3.7/c-api/module.html

Notes on -

	https://docs.python.org/3.7/extending/newtypes_tutorial.html

	Use, at least, a C99 compiler. For easier struct initialization.

		"Note We recommend using C99-style designated initializers as above, to 
		avoid listing all the PyTypeObject fields that you don’t care about and 
		also to avoid caring about the fields’ declaration order."

	Create Custom instances by calling the Custome class -

		>>> import custom
		>>> mycustom = custom.Custom()

	Or, to create a canvas context -

		>>>	import ui
		>>> ctx = ui.CanvasContext()

	"Note While this documentation showcases the standard distutils module for 
	building C extensions, it is recommended in real-world use cases to use the 
	newer and better-maintained setuptools library. Documentation on how to do 
	this is out of scope for this document and can be found in the Python 
	Packaging User’s Guide."

	To add parameters to the ui.CanvasContext() call (when creating a 
	CanvasContext object) see -

		https://docs.python.org/3.7/extending/newtypes_tutorial.html#adding-data-and-methods-to-the-basic-example

	The parameters being the frameId, paneId (or names), and the name of the 
	canvas (e.g., "cvsA"). Then the CanvasContext() (constructor?) can go to 
	app to get the context handle.

Notes on building (i.e., compiling, etc.) -

	https://docs.python.org/3.7/c-api/module.html#initializing-modules
*/

#define	PY_SSIZE_CLEAN

#include	<Python.h>

#include	<frameobject.h>			//	For BRADDS_F_FLAGS_UI_CALL.

#include	"rr_ui.h"


//	Going by -
//
//		https://docs.python.org/3.7/extending/newtypes_tutorial.html

static PyObject *		UIError = NULL;

static UICBToAppFunc	UICB = NULL;

static int				UICBInProgress = 0;

/*
static PyObject *		UICBResult = NULL;
*/

//	My custom object.
//
typedef struct {
	PyObject_HEAD

//	int			frameId;
//	int			paneId;
	PyObject *	pane;			//	e.g., "Controls"
	PyObject *	panel;			//	e.g., "pnlMain"
	PyObject *	canvas;			//	e.g., "cvsA"
	int			hContext;		//	i.e., context handle

}	CanvasContext;

static void
CanvasContext_dealloc ( CanvasContext * self )
{
	Py_XDECREF(self->pane);
	Py_XDECREF(self->panel);
	Py_XDECREF(self->canvas);
	Py_TYPE(self)->tp_free ( (PyObject *) self );
}

static PyObject *
CanvasContext_new ( PyTypeObject * type, PyObject * args, PyObject * kwds )
{
	CanvasContext *	self;
	self = (CanvasContext *)type->tp_alloc ( type, 0 );
	if ( self == NULL ) {
		return NULL; }

//	self->frameId  = 0;
//	self->paneId   = 0;
	self->pane = PyUnicode_FromString ( "" );
	if ( self->pane == NULL ) {
		Py_DECREF(self);
		return NULL; }
	self->panel = PyUnicode_FromString ( "" );
	if ( self->panel == NULL ) {
		Py_DECREF(self);
		return NULL; }
	self->canvas = PyUnicode_FromString ( "" );
	if (self->canvas == NULL) {
		Py_DECREF(self);
		return NULL; }
	self->hContext = 0;
	return (PyObject *)self;

}	//	CanvasContext_new()

static int
CanvasContext_init ( CanvasContext * self, PyObject * args, PyObject * kwds )
{
	if ( UICBInProgress ) {
		UICBInProgress = 0;
		if ( self->hContext > 0 ) {
			return 0; }
		PyErr_SetString ( UIError, "Callback in progress, but no canvas context." );
		return -1; }

//	static char *	kwlist[] = { "frameId", "paneId", "panelName", "canvasName", 
	static char *	kwlist[] = { "pane", "panel", "canvas", 
								 NULL };
	PyObject * pane = NULL, * panel = NULL, * canvas = NULL, * tmp;

	if ( ! PyArg_ParseTupleAndKeywords ( args, kwds, "UUU", kwlist,
										 &pane, &panel, &canvas) ) {
		return -1; }

	if ( pane ) {
		tmp = self->pane;
		Py_INCREF(pane);
		self->pane = pane;
		Py_DECREF(tmp); }

	if ( panel ) {
		tmp = self->panel;
		Py_INCREF(panel);
		self->panel = panel;
		Py_DECREF(tmp); }

	if ( canvas ) {
		tmp = self->canvas;
		Py_INCREF(canvas);
		self->canvas = canvas;
		Py_DECREF(tmp); }

	//	Callback to the app to get the context handle.
	//
	if ( UICB == NULL ) {
		PyErr_SetString ( UIError, "App rr_ui callback is not set." );
		return -1; }
	UICBInProgress = 1;
	self->hContext            = 0;
	PyObject * uSelf = PyUnicode_FromFormat ( "%llu", 
											  (unsigned long long)self );
	PyObject * o;
//	o = UICB ( BRADDS_F_FLAGS_UI_CALL,
//	//	Do not specify BRADDS_F_FLAGS_UI_CALL because we don't need/want to 
	//	execute the statement again.
	//	But we do need to have the app provide a canvas context.  How?
	//		-	Finish the statement, but, in the response to the app some
	//			command or status to get and provide the canvas context before 
	//			executing the next statement.
	o = UICB ( 0,						//	don't execute the statement again
			   "[ { \"cmd\": \"get-canvas-context\", "
				  " \"context-object\": \"%s\", "
				  " \"pane\": \"%s\", "
				  " \"panel\": \"%s\", "
				  " \"canvas\": \"%s\" } ]", PyUnicode_DATA ( uSelf ),
										  PyUnicode_DATA ( self->pane),
										  PyUnicode_DATA ( self->panel ),
										  PyUnicode_DATA ( self->canvas ) );
	Py_DECREF(uSelf);

	if ( o == Py_None ) {
		Py_DECREF(o); }

	return 0;

}	//	CanvasContext_init()


static void
Error ( const char * fmt, ... )
{
	char buf [128];		buf[sizeof(buf) - 1] = '\0';

	va_list	alist;	va_start ( alist, fmt );

	vsnprintf ( buf, sizeof(buf) - 1, fmt, alist );

	PyErr_SetString ( UIError, buf );

}	//	Error()

static PyObject *
CtxCallPart2 ( const char * fnc, PyObject * response )
{
	if ( response == NULL ) {
		Error ( "%s: no result", fnc );
		return NULL; }

	if ( ! PyList_Check ( response ) ) {
		Error ( "%s: result is not a list", fnc );
		return NULL; }

	int n = PySequence_Length ( response );

	if ( n != 1 ) {
		Error ( "%s: expected 1 dict in result list", fnc ); 
		return NULL; }

	PyObject * d = PySequence_GetItem ( response, 0 );

	if ( ! PyDict_Check ( d ) ) {
		Error ( "%s: expected dict item in result list", fnc );
		return NULL; }

	//	From whence status, result?
	//	-	UICB result, a PyObject *.
	//	-	Gotten from the second UICB(), just above.
	//	-	Which itself is gotten by/from utils.c:ParseValue().
	//	-	It is the response from the app. I.e., one of the dicts
	//		in the array of responses.
	//	-	So, here, we use a PyDict_... API call to get the status
	//		and result items.

	PyObject *	statusO = PyDict_GetItemString ( d, "status" );
	PyObject *	resultO = PyDict_GetItemString ( d, "result" );

	if ( PyUnicode_KIND ( statusO ) != PyUnicode_1BYTE_KIND ) {
		Error ( "%s: expected status in dict item", fnc );
		return NULL; }

	if ( ! resultO ) {
		Error ( "%s: expect result object in dict item" );
		return NULL; }

	const char * status = PyUnicode_DATA ( statusO );

	if ( strcmp ( status, "ok" )  == 0 ) {
		//	result may be any PyObject *.
		Py_INCREF(resultO);
		return resultO; }

	//	If status != "error" then result should always be a string (i.e., 
	//	an error message).

//	PyErr_SetObject ( UIError, result );
	Error ( "%s: %s", fnc,  status );

	return NULL; 

}	//	CtxCallPart2()

static PyObject *
CanvasContext_fillStyle ( CanvasContext * self, PyObject * args )
{
	if ( UICBInProgress ) {
		UICBInProgress = 0;

	//	if ( UICBResult == NULL ) {
	//		PyErr_SetString ( UIError, "fillstyle: no result" );
	//		return NULL; }

		//	Part 2.  Get the app response from the callback.
		PyObject * o = UICB ( 0, NULL );

	//	if ( o == NULL ) {
	//		PyErr_SetString ( UIError, "fillstyle: no result" );
	//		return NULL; }
	//
	//	if ( ! PyList_Check ( o ) ) {
	//		PyErr_SetString ( UIError, "fillstyle: result is not a list" );
	//		return NULL; }
	//
	//	int n = PySequence_Length ( o );
	//
	//	if ( n != 1 ) {
	//		PyErr_SetString ( UIError, "fillstyle: expected 1 dict in result "
	//								   "list" );
	//		return NULL; }
	//
	//	PyObject * d = PySequence_GetItem ( o, 0 );
	//
	//	if ( ! PyDict_Check ( d ) ) {
	//		PyErr_SetString ( UIError, "fillstyle: expected dict item in result "
	//								   "list" );
	//		return NULL; }
	//
	//	//	From whence status, result?
	//	//	-	UICB result, a PyObject *.
	//	//	-	Gotten from the second UICB(), just above.
	//	//	-	Which itself is gotten by/from utils.c:ParseValue().
	//	//	-	It is the response from the app. I.e., one of the dicts
	//	//		in the array of responses.
	//	//	-	So, here, we use a PyDict_... API call to get the status
	//	//		and result items.
	//
	//	PyObject *	status = PyDict_GetItemString ( d, "status" );
	//	PyObject *	result = PyDict_GetItemString ( d, "result" );
	//
	//	if ( strcmp ( status, "ok" )  == 0 ) {
	//		//	result may be any PyObject *.
	//		Py_INCREF(result);
	//		Py_DECREF(o);	o = NULL;
	//		return result; }
	//
	//	//	If status != "error" then result should always be a string (i.e., 
	//	//	an error message).
	//
	//	PyErr_SetObject ( UIError, result );
	//	Py_DECREF(o);	o = NULL;
	//	return NULL; }
		PyObject * r = CtxCallPart2 ( "fillStyle", o ); 

		Py_DECREF(o);	o = NULL;

		return r; }

	//	Part 1.
	//
	//  On errors - see -
	//      https://docs.python.org/3/extending/extending.html#intermezzo-errors-and-exceptions
	//  The Python C API function will have probably set the exception. The
	//  client (us) should just return NULL and let what called us do what
	//  ever.
	//
	//	On parsing arguments -
	//		https://docs.python.org/3/c-api/arg.html?highlight=pyarg_parsetuple#parsing-arguments
	//		https://docs.python.org/3/c-api/arg.html?highlight=pyarg_parsetuple#c.PyArg_ParseTuple
	//
	const char * style;

	if ( ! PyArg_ParseTuple ( args, "s:fillStyle", &style ) ) {
		return NULL; }

	PyObject * uSelf = PyUnicode_FromFormat ( "%llu", 
											  (unsigned long long)self );
	PyObject * o;
	o = UICB ( BRADDS_F_FLAGS_UI_CALL,
			   "[ { \"cmd\": \"canvas-set-fillstyle\", "
				  " \"context-object\": \"%s\", "
				  " \"pane\": \"%s\", "
				  " \"panel\": \"%s\", "
				  " \"canvas\": \"%s\", "
				  " \"context-handle\": %d, "
				  " \"fillstyle\": \"%s\"  } ]", PyUnicode_DATA ( uSelf ),
										  PyUnicode_DATA ( self->pane ),
										  PyUnicode_DATA ( self->panel ),
										  PyUnicode_DATA ( self->canvas ),
										  self->hContext,
										  style );
	Py_DECREF(uSelf);

	return o;

}	//	CanvasContext_fillStyle()

static PyObject *
CanvasContext_fillRect ( CanvasContext * self, PyObject * args )
{
	if ( UICBInProgress ) {
		UICBInProgress = 0;

		//	Part 2.  Get the app response from the callback.
		//
		PyObject * o = UICB ( 0, NULL );

		PyObject * r = CtxCallPart2 ( "fillRect", o ); 

		Py_DECREF(o);	o = NULL;

		return r; }

	//	Part 1.
	//

	int x, y, w, h;

	if ( ! PyArg_ParseTuple ( args, "iiii:fillRect", &x, &y, &w, &h) ) {
		return NULL; }

	PyObject * uSelf = PyUnicode_FromFormat ( "%llu", 
											  (unsigned long long)self );
	PyObject * o;
	o = UICB ( BRADDS_F_FLAGS_UI_CALL,
			   "[ { \"cmd\": \"canvas-fill-rect\", "
				  " \"context-object\": \"%s\", "
				  " \"pane\": \"%s\", "
				  " \"panel\": \"%s\", "
				  " \"canvas\": \"%s\", "
				  " \"context-handle\": %d, "
				  " \"x\": %d, \"y\": %d, \"w\": %d, \"h\": %d  } ]", 
				PyUnicode_DATA ( uSelf ),
				PyUnicode_DATA ( self->pane ),
				PyUnicode_DATA ( self->panel ),
				PyUnicode_DATA ( self->canvas ),
				self->hContext,
				x, y, w, h );

	Py_DECREF(uSelf);

	return o;

}	//	CanvasContext_fillRect()

static PyObject *
CanvasContext_translate ( CanvasContext * self, PyObject * args )
{
	Error ( "%s: implement this", "translate" );

	//	Part 2.


	//	Part 1.


}	//	CanvasContext_translate()

static PyObject *
CanvasContext_rotate ( CanvasContext * self, PyObject * args )
{
	Error ( "%s: implement this", "rotate" );

	//	Part 2.


	//	Part 1.


}	//	CanvasContext_rotate()

static PyObject *
CanvasContext_getImageData ( CanvasContext * self, PyObject * args )
{
	Error ( "%s: implement this", "getImageData" );

	//	Part 2.


	//	Part 1.


}	//	CanvasContext_getImageData()

static PyMethodDef CanvasContext_methods[] = {
    { "fillStyle",		(PyCFunction)CanvasContext_fillStyle,		METH_VARARGS,
      "Set the fill stiyle for a canvas." },
    { "fillRect",		(PyCFunction)CanvasContext_fillRect,		METH_VARARGS,
      "Fill a rectangle with the current fill style." },
    { "translate",		(PyCFunction)CanvasContext_translate,		METH_VARARGS,
      "Translate." },
    { "rotate",			(PyCFunction)CanvasContext_rotate,			METH_VARARGS,
      "Rotate." },
    { "getImageData",	(PyCFunction)CanvasContext_getImageData,	METH_VARARGS,
      "Get image data." },
    { NULL }  /* Sentinel */
};

static PyTypeObject CanvasContextType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	.tp_name = "rr_ui.CanvasContext",
	.tp_doc = "Canvas Context",
	.tp_basicsize = sizeof(CanvasContext),
	.tp_itemsize = 0,
	.tp_flags = Py_TPFLAGS_DEFAULT,
	.tp_new = CanvasContext_new,
	.tp_init = (initproc) CanvasContext_init,
	.tp_dealloc = (destructor) CanvasContext_dealloc,
//	.tp_members = Custom_members,
	.tp_methods = CanvasContext_methods,
//	.tp_getset  = Custom_getsetters,
};








PyDoc_STRVAR(setCallback_doc, "setCallback()\n\nSet the UI callback.");

static PyObject *	setCallback ( PyObject * self, PyObject * args ) {

	//	For PyArg_ParseTuple() see -
	//
	//		https://docs.python.org/3.7/extending/extending.html#extracting-parameters-in-extension-functions
	//
	//	and -
	//	
	//		https://docs.python.org/3.7/extending/extending.html#building-arbitrary-values
	//
	//	For PyArg_ParseTupleAndKeywords() see -
	//
	//		 https://docs.python.org/3.7/extending/extending.html#keyword-parameters-for-extension-functions

//	unsigned long long	cb;
//	if ( ! PyArg_ParseTuple ( args, "K:setCallback", &cb ) )
	const char *	cb;
	if ( ! PyArg_ParseTuple ( args, "s:setCallback", &cb ) )
		return NULL;

//	UICB = (UICBToAppFunc)cb;
	UICB = (UICBToAppFunc)atoll ( cb );

    Py_INCREF(Py_None);
    return Py_None;

}	//	setCallback()

PyDoc_STRVAR(setCanvasContextHandle_doc, 
		"setCanvasContextHandle()\n\nSet/keep the handle to a canvas context.");

static PyObject *	setCanvasContextHandle ( PyObject * self, PyObject * args ) {

	const char * sCC;	int handle;
	if ( ! PyArg_ParseTuple ( args, "si:setCanvasContextHandle", &sCC, &handle ) )
		return NULL;

	CanvasContext * cc = (CanvasContext *)atoll ( sCC );

	cc->hContext = handle;

    Py_INCREF(Py_None);
    return Py_None;

}	//	setCanvasContextHandle()


/*
PyDoc_STRVAR(setCallbackResult_doc, 
		"setCallbackResult()\n\nSet/keep the result of a callback.");

static PyObject *	setCallbackResult ( PyObject * self, PyObject * args ) {

	if ( ! PyArg_ParseTuple ( args, "O:setCallbackResult ", &UICBResult ) )
		return NULL;

    Py_INCREF(Py_None);
    return Py_None;

}	//	setCallbackResult ()
*/

static PyMethodDef	rr_ui_Methods[] = {
        
	{ "setCallback",			setCallback, 
								METH_VARARGS, setCallback_doc },
	{ "setCanvasContextHandle", setCanvasContextHandle, 
								METH_VARARGS, setCanvasContextHandle_doc },
//	{ "setCallbackResult", 		setCallbackResult, 
//								METH_VARARGS, setCallbackResult_doc },
	{ NULL, NULL, 0, NULL } 
};

static struct PyModuleDef       rr_ui_Module = {
	PyModuleDef_HEAD_INIT,
	"rr_ui",
	NULL,
	-1,
	rr_ui_Methods,
	NULL,
	NULL,
	NULL,
	NULL
};

//	Note -
//
//	Evedently, to be integrated in the static build of the executable ./python, 
//	this must be added to file Modules/Setup (see bottom of that file) -
//
//			rr_ui rr_ui.c
//
//	The (maybe) the same thing in Modules/Setup.dist.

PyMODINIT_FUNC	PyInit_rr_ui ( void ) {

	if ( PyType_Ready ( &CanvasContextType ) < 0 ) {
		return NULL; }

	PyObject *	m = PyModule_Create ( &rr_ui_Module );
	if ( m == NULL ) {
		return NULL; }

	if ( UIError == NULL ) {
		UIError = PyErr_NewException ( "rr_ui.error", NULL, NULL );
		if ( UIError == NULL ) {
			return NULL; } }
	Py_INCREF(UIError);
	PyModule_AddObject ( m, "error", UIError );





	Py_INCREF(&CanvasContextType);
	if (PyModule_AddObject ( m, "CanvasContext", 
							 (PyObject *)&CanvasContextType ) < 0 ) {
		Py_DECREF(&CanvasContextType);
		Py_DECREF(m);
		return NULL; }

	return m;
}

