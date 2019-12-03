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

PyAPI_FUNC(int)	_BradDs_SetCreateRecordCB ( BradDs_CreateRecordCB );


//	pe()		e.g., cmd: 'new-record', ...
//
typedef PyObject * (* BradDs_PECB) ( PyObject * script );

PyAPI_FUNC(int)	_BradDs_SetPECB ( BradDs_PECB );


//	ui()
//
typedef PyObject * (* BradDs_UICB) ( PyObject * script );

PyAPI_FUNC(int)	_BradDs_SetUICB ( BradDs_UICB );


//	vp()		i.e., viewport
//
typedef PyObject * (* BradDs_VPCB) ( PyObject * script );

PyAPI_FUNC(int)	_BradDs_SetVPCB ( BradDs_VPCB );


//	world()
//
typedef PyObject * (* BradDs_WorldCB) ( PyObject * script );

PyAPI_FUNC(int)	_BradDs_SetWorldCB ( BradDs_WorldCB );


//	robot()
//
typedef PyObject * (* BradDs_RobotCB) ( PyObject * script );

PyAPI_FUNC(int)	_BradDs_SetRobotCB ( BradDs_RobotCB ) ;


#ifdef __cplusplus
}
#endif
#endif /* !Py_BLTINMODULE_H */
