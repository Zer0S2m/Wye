#include <Python.h>
#include <core\constant.h>


static PyObject *method_build_json(PyObject *self, PyObject *args) {
    PyObject *obj = PyDict_New();
    char *value = NULL;
    const char *key;

    if(!PyArg_ParseTuple(args, "ss", &key, &value)) {
        return NULL;
    }

    PyObject *PyValue = PyUnicode_FromString(value);

    PyDict_SetItemString(obj, key, PyValue);

    return obj;
}


static PyMethodDef WyeSerializersMethods[] = {
    {
        "build_json",
        method_build_json,
        METH_VARARGS,
        "Python interface for fputs Wye"
    },
    {
        NULL,
        NULL,
        0,
        NULL
    }
};


static struct PyModuleDef wye_serializers_modules = {
    PyModuleDef_HEAD_INIT,
    "wye_serializers",
    "Python interface for the Wye",
    -1,
    WyeSerializersMethods
};


PyMODINIT_FUNC PyInit_wye_serializers(void) {
    PyObject *module = PyModule_Create(&wye_serializers_modules);
    return module;
}
