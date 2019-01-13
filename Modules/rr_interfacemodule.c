
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

//	The actual colletions of things, such as UDUIs, is gotten, at certain
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
//	Do this: Implement a class, name it, maybe, xy, and it to represent the 
//	positions of controls.  Or implement a class, control, with attributes such 
//	as x, y, w, h, name, etc..





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

    /* create control_object structure */
    control = (control_object *)type->tp_alloc ( type, 0 );
    if ( control == NULL )
        return NULL;

	control->attr = PyDict_New();
	if ( control->attr == NULL ) {
        Py_DECREF(control);
        return NULL; }

    return (PyObject *)control;
}

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

		return PyDict_SetItemString ( self->attr, name, v) ;
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
