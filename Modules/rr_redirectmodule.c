//	Copied from xxmodule.c

/* Use this file as a template to start implementing a module that
   also declares object types. All occurrences of 'Xxo' should be changed
   to something reasonable for your objects. After that, all other
   occurrences of 'xx' should be changed to something reasonable for your
   module. If your module is named foo your sourcefile should be named
   foomodule.c.

   You will probably want to delete all references to 'x_attr' and add
   your own types of attributes instead.  Maybe you want to name your
   local variables other than 'self'.  If your object type is needed in
   other files, you'll have to create a file "foobarobject.h"; see
   floatobject.h for an example. */

//	Xxo -> File
//
//	Objects are called files because we are redirecting stdout, stderr for
//	which we need an object for each, and stdout and stderr are called file
//	objects throughout CPython.
//
//	Please don't confuse what are called "file objects" here with those 
//	elsewhere.  Here file objects simply relay stdout, stderr characters
//	to callbacks specified by the RR parts that create these objects.
//
//	Xxo -> RRStdFileRedirect
//
//	There.  RR says the object is a Robot Records thing.  StdFileRedirect
//	says the object redirects stdout or stderr.

//	xx -> rr_redirect


/* RRStdFileRedirect objects */

#include "Python.h"

static PyObject *ErrorObject;

typedef struct {
	PyObject_HEAD

	PyCFunction		write;
	PyCFunction		flush;

}	RRWriteFlush;

typedef struct {
	PyObject_HEAD
	PyObject            *x_attr;        /* Attributes dictionary */

	PyCFunction		cbwrite;
	PyCFunction		cbflush;

} RRStdFileRedirectObject;

static PyTypeObject RRStdFileRedirect_Type;

#define RRStdFileRedirectObject_Check(v) (Py_TYPE(v) == &RRStdFileRedirect_Type)

static RRStdFileRedirectObject *
newRRStdFileRedirectObject ( PyObject * args )
{
//	void * cbwrite, * cbflush;

	//	Docs on format string -
	//	https://docs.python.org/3/c-api/arg.html?highlight=pyarg_parsetuple#parsing-arguments

//	if ( ! PyArg_ParseTuple ( args, "OO:new", &cbwrite, &cbflush ) )
	RRWriteFlush * WF;
	if ( ! PyArg_ParseTuple ( args, "O:new", &WF ) )
		return NULL;

//	printf ( "newRRStdFileRedirectObject():  write 0x%08X  flush 0x%08X\n",
//			 cbwrite, cbflush );

//	printf ( "newRRStdFileRedirectObject():  RRStdFileRedirect_Type.tp_basicsize %d  "
//			 "sizeof(RRStdFileRedirectObject) %d\n",  RRStdFileRedirect_Type.tp_basicsize,
//			 										  sizeof(RRStdFileRedirectObject) );

	RRStdFileRedirectObject *	self;
	self = PyObject_New ( RRStdFileRedirectObject, &RRStdFileRedirect_Type );
	if ( self == NULL )
		return NULL;

//	printf ( "newRRStdFileRedirectObject():  setting self ...\n" );

	self->x_attr = NULL;

//	self->cbwrite = (PyCFunction)cbwrite;
//	self->cbflush = (PyCFunction)cbflush;
	self->cbwrite = WF->write;
	self->cbflush = WF->flush;

//	Py_DECREF ( WF );		Done by the args tuple.

	return self;
}

/* RRStdFileRedirect methods */

static void
RRStdFileRedirect_dealloc(RRStdFileRedirectObject *self)
{
	Py_XDECREF(self->x_attr);
	PyObject_Del(self);
}

static PyObject *
RRStdFileRedirect_demo(RRStdFileRedirectObject *self, PyObject *args)
{
	if (!PyArg_ParseTuple(args, ":demo"))
		return NULL;
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
RRStdFileRedirect_write ( RRStdFileRedirectObject * self, PyObject * args )
{
//	printf ( "RRStdFileRedirect_write(): cbwrite 0x%08X\n", self->cbwrite );

	return self->cbwrite ( (PyObject *)self, args );
}

static PyObject *
RRStdFileRedirect_flush ( RRStdFileRedirectObject * self, PyObject * args )
{
	return self->cbflush ( (PyObject *)self, args );
}

static PyMethodDef RRStdFileRedirect_methods[] = {
	{"demo",            (PyCFunction)RRStdFileRedirect_demo,  METH_VARARGS,
		PyDoc_STR("demo() -> None")},
	{"write",           (PyCFunction)RRStdFileRedirect_write,  METH_VARARGS, 
		PyDoc_STR("write() -> empty string")},
	{"flush",           (PyCFunction)RRStdFileRedirect_flush,  METH_VARARGS,
		PyDoc_STR("flush() -> empty string")},
	{NULL,              NULL}           /* sentinel */
};

static PyObject *
RRStdFileRedirect_getattro(RRStdFileRedirectObject *self, PyObject *name)
{
	if (self->x_attr != NULL) {
		PyObject *v = PyDict_GetItem(self->x_attr, name);
		if (v != NULL) {
			Py_INCREF(v);
			return v;
		}
	}
	return PyObject_GenericGetAttr((PyObject *)self, name);
}

static int
RRStdFileRedirect_setattr(RRStdFileRedirectObject *self, const char *name, PyObject *v)
{
	if (self->x_attr == NULL) {
		self->x_attr = PyDict_New();
		if (self->x_attr == NULL)
			return -1;
	}
	if (v == NULL) {
		int rv = PyDict_DelItemString(self->x_attr, name);
		if (rv < 0)
			PyErr_SetString(PyExc_AttributeError,
				"delete non-existing RRStdFileRedirect attribute");
		return rv;
	}
	else
		return PyDict_SetItemString(self->x_attr, name, v);
}

static PyTypeObject RRStdFileRedirect_Type = {
	/* The ob_type field must be initialized in the module init function
	 * to be portable to Windows without using C++. */
	PyVarObject_HEAD_INIT(NULL, 0)
	"rr_redirectmodule.RRStdFileRedirect",             /*tp_name*/
	sizeof(RRStdFileRedirectObject),          /*tp_basicsize*/
	0,                          /*tp_itemsize*/
	/* methods */
	(destructor)RRStdFileRedirect_dealloc,    /*tp_dealloc*/
	0,                          /*tp_print*/
	(getattrfunc)0,             /*tp_getattr*/
	(setattrfunc)RRStdFileRedirect_setattr,   /*tp_setattr*/
	0,                          /*tp_reserved*/
	0,                          /*tp_repr*/
	0,                          /*tp_as_number*/
	0,                          /*tp_as_sequence*/
	0,                          /*tp_as_mapping*/
	0,                          /*tp_hash*/
	0,                          /*tp_call*/
	0,                          /*tp_str*/
	(getattrofunc)RRStdFileRedirect_getattro, /*tp_getattro*/
	0,                          /*tp_setattro*/
	0,                          /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,         /*tp_flags*/
	0,                          /*tp_doc*/
	0,                          /*tp_traverse*/
	0,                          /*tp_clear*/
	0,                          /*tp_richcompare*/
	0,                          /*tp_weaklistoffset*/
	0,                          /*tp_iter*/
	0,                          /*tp_iternext*/
	RRStdFileRedirect_methods,                /*tp_methods*/
	0,                          /*tp_members*/
	0,                          /*tp_getset*/
	0,                          /*tp_base*/
	0,                          /*tp_dict*/
	0,                          /*tp_descr_get*/
	0,                          /*tp_descr_set*/
	0,                          /*tp_dictoffset*/
	0,                          /*tp_init*/
	0,                          /*tp_alloc*/
	0,                          /*tp_new*/
	0,                          /*tp_free*/
	0,                          /*tp_is_gc*/
};
/* --------------------------------------------------------------------- */

/* Function of two integers returning integer */

PyDoc_STRVAR(rr_redirect_foo_doc,
"foo(i,j)\n\
\n\
Return the sum of i and j.");

static PyObject *
rr_redirect_foo(PyObject *self, PyObject *args)
{
	long i, j;
	long res;
	if (!PyArg_ParseTuple(args, "ll:foo", &i, &j))
		return NULL;
	res = i+j; /* XXX Do something here */
	return PyLong_FromLong(res);
}


/* Function of no arguments returning new RRStdFileRedirect object */

static PyObject *
rr_redirect_new ( PyObject * self, PyObject * args )
{
//	printf ( "rr_redirect_new(): 0x%08X\n", rr_redirect_new );
	
	RRStdFileRedirectObject *rv;

	rv = newRRStdFileRedirectObject ( args );
	if ( rv == NULL )
		return NULL;

//	printf ( "rr_redirect_new(): returning 0x%08X\n", rv );

	return (PyObject *)rv;
}

/* Example with subtle bug from extensions manual ("Thin Ice"). */

static PyObject *
rr_redirect_bug(PyObject *self, PyObject *args)
{
	PyObject *list, *item;

	if (!PyArg_ParseTuple(args, "O:bug", &list))
		return NULL;

	item = PyList_GetItem(list, 0);
	/* Py_INCREF(item); */
	PyList_SetItem(list, 1, PyLong_FromLong(0L));
	PyObject_Print(item, stdout, 0);
	printf("\n");
	/* Py_DECREF(item); */

	Py_INCREF(Py_None);
	return Py_None;
}

/* Test bad format character */

static PyObject *
rr_redirect_roj(PyObject *self, PyObject *args)
{
	PyObject *a;
	long b;
	if (!PyArg_ParseTuple(args, "O#:roj", &a, &b))
		return NULL;
	Py_INCREF(Py_None);
	return Py_None;
}


/* ---------- */

static PyTypeObject Str_Type = {
	/* The ob_type field must be initialized in the module init function
	 * to be portable to Windows without using C++. */
	PyVarObject_HEAD_INIT(NULL, 0)
	"rr_redirectmodule.Str",             /*tp_name*/
	0,                          /*tp_basicsize*/
	0,                          /*tp_itemsize*/
	/* methods */
	0,                          /*tp_dealloc*/
	0,                          /*tp_print*/
	0,                          /*tp_getattr*/
	0,                          /*tp_setattr*/
	0,                          /*tp_reserved*/
	0,                          /*tp_repr*/
	0,                          /*tp_as_number*/
	0,                          /*tp_as_sequence*/
	0,                          /*tp_as_mapping*/
	0,                          /*tp_hash*/
	0,                          /*tp_call*/
	0,                          /*tp_str*/
	0,                          /*tp_getattro*/
	0,                          /*tp_setattro*/
	0,                          /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
	0,                          /*tp_doc*/
	0,                          /*tp_traverse*/
	0,                          /*tp_clear*/
	0,                          /*tp_richcompare*/
	0,                          /*tp_weaklistoffset*/
	0,                          /*tp_iter*/
	0,                          /*tp_iternext*/
	0,                          /*tp_methods*/
	0,                          /*tp_members*/
	0,                          /*tp_getset*/
	0, /* see PyInit_rr_redirect */      /*tp_base*/
	0,                          /*tp_dict*/
	0,                          /*tp_descr_get*/
	0,                          /*tp_descr_set*/
	0,                          /*tp_dictoffset*/
	0,                          /*tp_init*/
	0,                          /*tp_alloc*/
	0,                          /*tp_new*/
	0,                          /*tp_free*/
	0,                          /*tp_is_gc*/
};

/* ---------- */

static PyObject *
null_richcompare(PyObject *self, PyObject *other, int op)
{
	Py_INCREF(Py_NotImplemented);
	return Py_NotImplemented;
}

static PyTypeObject Null_Type = {
	/* The ob_type field must be initialized in the module init function
	 * to be portable to Windows without using C++. */
	PyVarObject_HEAD_INIT(NULL, 0)
	"rr_redirectmodule.Null",            /*tp_name*/
	0,                          /*tp_basicsize*/
	0,                          /*tp_itemsize*/
	/* methods */
	0,                          /*tp_dealloc*/
	0,                          /*tp_print*/
	0,                          /*tp_getattr*/
	0,                          /*tp_setattr*/
	0,                          /*tp_reserved*/
	0,                          /*tp_repr*/
	0,                          /*tp_as_number*/
	0,                          /*tp_as_sequence*/
	0,                          /*tp_as_mapping*/
	0,                          /*tp_hash*/
	0,                          /*tp_call*/
	0,                          /*tp_str*/
	0,                          /*tp_getattro*/
	0,                          /*tp_setattro*/
	0,                          /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
	0,                          /*tp_doc*/
	0,                          /*tp_traverse*/
	0,                          /*tp_clear*/
	null_richcompare,           /*tp_richcompare*/
	0,                          /*tp_weaklistoffset*/
	0,                          /*tp_iter*/
	0,                          /*tp_iternext*/
	0,                          /*tp_methods*/
	0,                          /*tp_members*/
	0,                          /*tp_getset*/
	0, /* see PyInit_rr_redirect */      /*tp_base*/
	0,                          /*tp_dict*/
	0,                          /*tp_descr_get*/
	0,                          /*tp_descr_set*/
	0,                          /*tp_dictoffset*/
	0,                          /*tp_init*/
	0,                          /*tp_alloc*/
	PyType_GenericNew,          /*tp_new*/
	0,                          /*tp_free*/
	0,                          /*tp_is_gc*/
};


/* ---------- */


/* List of functions defined in the module */

static PyMethodDef rr_redirect_methods[] = {
	{"roj",             rr_redirect_roj,         METH_VARARGS,
		PyDoc_STR("roj(a,b) -> None")},
	{"foo",             rr_redirect_foo,         METH_VARARGS,
		rr_redirect_foo_doc},
	{"new",             rr_redirect_new,         METH_VARARGS,
		PyDoc_STR("new(cbwrite, cbflush) -> new rr_redirect object")},
	{"bug",             rr_redirect_bug,         METH_VARARGS,
		PyDoc_STR("bug(o) -> None")},
	{NULL,              NULL}           /* sentinel */
};

PyDoc_STRVAR(module_doc,
"This is a template module just for instruction.");


static int
rr_redirect_exec(PyObject *m)
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
//	Null_Type.tp_base = &PyBaseObject_Type;
//	Str_Type.tp_base = &PyUnicode_Type;
//
//	bradds - exception at end of the program when Str_Type is garbage collected,
//	deallocated. I have noticed some things -
//	-	tp_dealloc is 0 for Str_Type in this file.
//	-	And, properly, I suppose, dp_flags does not have the Py_TPFLAGS_HEAPTYPE
//		bit set.	
//	-	PyType_Ready(&Str_Type) makes Str_Type.dp_dealloc inherit that of 
//		PyUnicode_Type but the Py_TPFLAGS_HEAPTYPE but remains unset in 
//		Str_Type.tp_dealloc.
//	Thus, I think, the exception.  cpython tries to dealloc Str_Type because
//	tp_dealloc is set but Py_TPFLAGS_HEAPTYPE is not set.
//	I don't see hw Str_Type or Null_Type are used here.  So, for now, just 
//	commenting them out.

	/* Finalize the type object including setting type of the new type
	 * object; doing it here is required for portability, too. */
	if (PyType_Ready(&RRStdFileRedirect_Type) < 0)
		goto fail;

	/* Add some symbolic constants to the module */
	if (ErrorObject == NULL) {
		ErrorObject = PyErr_NewException("rr_redirect.error", NULL, NULL);
		if (ErrorObject == NULL)
			goto fail;
	}
	Py_INCREF(ErrorObject);
	PyModule_AddObject(m, "error", ErrorObject);

	/* Add Str */
//	if (PyType_Ready(&Str_Type) < 0)
//		goto fail;
//	PyModule_AddObject(m, "Str", (PyObject *)&Str_Type);

	/* Add Null */
//	if (PyType_Ready(&Null_Type) < 0)
//		goto fail;
//	PyModule_AddObject(m, "Null", (PyObject *)&Null_Type);
	return 0;
 fail:
	Py_XDECREF(m);
	return -1;
}

static struct PyModuleDef_Slot rr_redirect_slots[] = {
	{Py_mod_exec, rr_redirect_exec},
	{0, NULL},
};

static struct PyModuleDef rr_redirectmodule = {
	PyModuleDef_HEAD_INIT,
	"rr_redirect",
	module_doc,
	0,
	rr_redirect_methods,
	rr_redirect_slots,
	NULL,
	NULL,
	NULL
};

/* Export function for the module (*must* be called PyInit_rr_redirect) */

PyMODINIT_FUNC
PyInit_rr_redirect(void)
{
	return PyModuleDef_Init(&rr_redirectmodule);
}
