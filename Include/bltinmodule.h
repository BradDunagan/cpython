#ifndef Py_BLTINMODULE_H
#define Py_BLTINMODULE_H
#ifdef __cplusplus
extern "C" {
#endif

PyAPI_DATA(PyTypeObject) PyFilter_Type;
PyAPI_DATA(PyTypeObject) PyMap_Type;
PyAPI_DATA(PyTypeObject) PyZip_Type;

typedef PyObject * (* BradDs_CreateRecordCB) ( PyObject * def_name,
											   PyObject * rec_name );

PyAPI_FUNC(int)	_BradDs_SetCreateRecordCB ( BradDs_CreateRecordCB ) ;


#ifdef __cplusplus
}
#endif
#endif /* !Py_BLTINMODULE_H */
