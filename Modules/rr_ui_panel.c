
#define	PY_SSIZE_CLEAN

#include	<Python.h>

#include	<frameobject.h>			//	For BRADDS_F_FLAGS_UI_CALL.

#include	"rr_ui_canvas.h"

#include	"rr_ui_panel.h"


extern UICBToAppFunc	UIAppCB;

extern int				UICBInProgress;

extern PyObject *		UIError;


void
Panel_dealloc ( Panel * self )
{
	Py_XDECREF(self->pane);
	Py_XDECREF(self->panel);
	Py_XDECREF(self->eleId);
	Py_TYPE(self)->tp_free ( (PyObject *) self );
}

PyObject *
Panel_new ( PyTypeObject * type, PyObject * args, PyObject * kwds )
{
	Panel *	self;
	self = (Panel *)type->tp_alloc ( type, 0 );
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
	self->eleId = NULL;
	return (PyObject *)self;

}	//	Panel_new()

int
Panel_init ( Panel * self, PyObject * args, PyObject * kwds )
{
	if ( UICBInProgress ) {
		UICBInProgress = 0;
		PyErr_SetString ( UIError, "Callback in progress." );
		return -1; }

//	static char *	kwlist[] = { "frameId", "paneId", "panelName", 
	static char *	kwlist[] = { "pane", "panel", 
								 NULL };
	PyObject * pane = NULL, * panel = NULL, * tmp;

	if ( ! PyArg_ParseTupleAndKeywords ( args, kwds, "UU", kwlist,
										 &pane, &panel) ) {
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


//	//	Callback to the app to get the context handle.
	//	Callback to the app to get the panel "pointer", or, maybe, element Id? 
	//	Necessary?  For sanity checks during panel calls? Helpful?
	//
	if ( UIAppCB == NULL ) {
		PyErr_SetString ( UIError, "App rr_ui callback is not set." );
		return -1; }
	UICBInProgress = 1;
	PyObject * uSelf = PyUnicode_FromFormat ( "%llu", 
											  (unsigned long long)self );
	PyObject * o;
//	o = UIAppCB ( BRADDS_F_FLAGS_UI_CALL,
//	//	Do not specify BRADDS_F_FLAGS_UI_CALL because we don't need/want to 
	//	execute the statement again.
	//	But we do need to have the app provide a canvas context.  How?
	//		-	Finish the statement, but, in the response to the app some
	//			command or status to get and provide the canvas context before 
	//			executing the next statement.
	o = UIAppCB ( 0,					//	don't execute the statement again
			   "[ { \"cmd\": \"get-panel-ele-id\", "
				  " \"panel-object\": \"%s\", "
				  " \"pane\": \"%s\", "
				  " \"panel\": \"%s\" } ]", PyUnicode_DATA ( uSelf ),
										  PyUnicode_DATA ( self->pane),
										  PyUnicode_DATA ( self->panel ) );
	Py_DECREF(uSelf);

	if ( o == Py_None ) {
		Py_DECREF(o); }

	return 0;

}	//	Panel_init()


PyObject *	
setPanelEleId ( PyObject * self, PyObject * args ) {

	const char * sW = "setPanelEleId()";

	const char * sPnl;	const char * sEleId;

	if ( ! UICBInProgress ) {
		UIErr ( "%s: expeted UICBInProgress", sW );
		return NULL; }

	UICBInProgress = 0;

	if ( ! PyArg_ParseTuple ( args, "ss:setPanelEleId", &sPnl, &sEleId ) )
		return NULL;

	Panel * pnl = (Panel *)atoll ( sPnl );

	PyObject * tmp = pnl->eleId;
	pnl->eleId = PyUnicode_FromString ( sEleId );
	Py_XDECREF(tmp); 

    Py_INCREF(Py_None);
    return Py_None;

}	//	setPanelEleId()


PyObject *	
Panel_size ( Panel * self, PyObject * args ) 
{
	if ( UICBInProgress ) {
		UICBInProgress = 0;

		//	Part 2.  Get the app response from the callback.
		//
		PyObject * o = UIAppCB ( 0, NULL );

		PyObject * r = AppCallPart2 ( "size", o ); 

		Py_DECREF(o);	o = NULL;

		return r; }

	//	Part 1.
	//
	int w, h;

	if ( ! PyArg_ParseTuple ( args, "ii:size", &w, &h) ) {
		return NULL; }

	PyObject * uSelf = PyUnicode_FromFormat ( "%llu", 
											  (unsigned long long)self );
	PyObject * o;
	o = UIAppCB ( BRADDS_F_FLAGS_UI_CALL,
			   "[ { \"cmd\": \"panel-size\", "
				  " \"panel-object\": \"%s\", "
				  " \"pane\": \"%s\", "
				  " \"panel\": \"%s\", "
				  " \"w\": %d, \"h\": %d  } ]", 
				PyUnicode_DATA ( uSelf ),
				PyUnicode_DATA ( self->pane ),
				PyUnicode_DATA ( self->panel ),
				w, h );

	UICBInProgress = 1;

	Py_DECREF(uSelf);

	return o;

}	//	Panel_size()

PyObject *	
Panel_load ( Panel * self, PyObject * args, PyObject * kwds ) 
{
	if ( UICBInProgress ) {
		UICBInProgress = 0;

		//	Part 2.  Get the app response from the callback.
		//
		PyObject * o = UIAppCB ( 0, NULL );

		PyObject * r = AppCallPart2 ( "load", o ); 

		Py_DECREF(o);	o = NULL;

		return r; }

	//	Part 1.
	//
//	const char * prompt;
//
//	if ( ! PyArg_ParseTuple ( args, "s:prompt", &prompt ) ) {
//		return NULL; }
	static char *	kwlist[] = { "prompt", NULL };

	PyObject * prompt = NULL;

	if ( ! PyArg_ParseTupleAndKeywords ( args, kwds, "U", kwlist, &prompt ) ) {
		return NULL; }

	PyObject * uSelf = PyUnicode_FromFormat ( "%llu", 
											  (unsigned long long)self );
	PyObject * o;
	o = UIAppCB ( BRADDS_F_FLAGS_UI_CALL,
			   "[ { \"cmd\": \"panel-load\", "
				  " \"panel-object\": \"%s\", "
				  " \"pane\": \"%s\", "
				  " \"panel\": \"%s\", "
				  " \"prompt\": \"%s\"  } ]", 
				PyUnicode_DATA ( uSelf ),
				PyUnicode_DATA ( self->pane ),
				PyUnicode_DATA ( self->panel ),
				PyUnicode_DATA ( prompt ) );

	UICBInProgress = 1;

	Py_DECREF(uSelf);

	return o;

}	//	Panel_size()


