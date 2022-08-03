#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <core/constant.h>


void *SetValidationError() {
    PyErr_SetString(PyExc_TypeError, "<WyeSerializers>: Validation error");
    return NULL;
}

void *SetAttributeError() {
    PyErr_SetString(PyExc_AttributeError, "<WyeSerializers>: Missing required parameters");
    return NULL;
}


static PyObject *method_build_json(PyObject *self, PyObject *args) {
    PyObject *obj = PyDict_New();
    PyObject *json, *rules = NULL;

    if (!PyArg_ParseTuple(args, "OO", &json, &rules)) {
        return NULL;
    }

    PyObject *params_rules = PyDict_Keys(rules);
    for (int index_param_rule = 0; index_param_rule < PyList_Size(params_rules); index_param_rule++) {
        PyObject *param_title = PyList_GetItem(params_rules, index_param_rule);
        PyObject *param_rule = PyDict_GetItem(rules, param_title);

        PyObject *type = PyDict_GetItemString(param_rule, TYPE_FIELD_KEY);
        PyObject *json_field = PyDict_GetItem(json, param_title);
        if (!json_field) {
            return SetAttributeError();
        }
        if (!PyObject_IsInstance(json_field, type)) {
            return SetValidationError();
        }

        PyObject *alias = PyDict_GetItemString(param_rule, ALIAS_FIELD_KEY);

        PyDict_SetItem(obj, alias, json_field);
    }

    return obj;
}


static PyMethodDef WyeSerializersMethods[] = {
    {
        "build_json",
        method_build_json,
        METH_VARARGS,
        "Collects json according to the rules"
    },
    { NULL, NULL, 0, NULL }
};


static struct PyModuleDef wye_serializers_modules = {
    PyModuleDef_HEAD_INIT,
    "wye_serializers",
    "Python serializer interface for Wye",
    -1,
    WyeSerializersMethods
};


PyMODINIT_FUNC PyInit_wye_serializers(void) {
    PyObject *module = PyModule_Create(&wye_serializers_modules);
    return module;
}
