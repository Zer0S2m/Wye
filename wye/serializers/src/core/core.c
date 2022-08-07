#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <core/constant.h>


int SetValidationError() {
    PyErr_SetString(PyExc_TypeError, "<WyeSerializers>: Validation error");
    return 0;
}

int SetAttributeError() {
    PyErr_SetString(PyExc_AttributeError, "<WyeSerializers>: Missing required parameters");
    return 0;
}


void *SetDefaultValue(PyObject *obj, PyObject *rules, PyObject *param_title) {
    PyObject *default_value = PyDict_GetItemString(rules, DEFAULT_FIELD_KEY);
    if (default_value != Py_None) {
        PyDict_SetItem(obj, param_title, default_value);
    }
}


int CheckField(PyObject *json_field, PyObject *type, PyObject *is_required) {
    if (!json_field && PyObject_IsTrue(is_required)) {
        return SetAttributeError();
    }
    if (!json_field) {
        return 1;
    }
    if (!PyObject_IsInstance(json_field, type)) {
        return SetValidationError();
    }
    return 1;
}


int BuildJson(
    PyObject *obj,
    PyObject *json,
    PyObject *params_rules,
    PyObject *rules,
    int index_param_rule
) {
    PyObject *param_title = PyList_GetItem(params_rules, index_param_rule);
    PyObject *param_rule = PyDict_GetItem(rules, param_title);
    PyObject *json_field = PyDict_GetItem(json, param_title);

    PyObject *type = PyDict_GetItemString(param_rule, TYPE_FIELD_KEY);
    PyObject *is_required = PyDict_GetItemString(param_rule, REQUIRED_FIELD_KEY);
    if (!CheckField(json_field, type, is_required))
        return NULL;

    PyObject *alias = PyDict_GetItemString(param_rule, ALIAS_FIELD_KEY);

    if (!json_field) {
        SetDefaultValue(obj, param_rule, alias);
        return 1;
    }

    PyDict_SetItem(obj, alias, json_field);

    return 1;
}


static PyObject *method_build_json(PyObject *self, PyObject *args) {
    PyObject *json, *rules;

    if (!PyArg_ParseTuple(args, "OO", &json, &rules)) {
        return NULL;
    }

    PyObject *params_rules = PyDict_Keys(rules);

    if (PyList_Check(json)) {
        int length_list_json = PyList_Size(json);
        PyObject *list_json = PyList_New(0);

        for (int in_json = 0; in_json < length_list_json; in_json++) {
            PyObject *ready_json = PyDict_New();
            PyObject *raw_json = PyList_GetItem(json, in_json);

            for (int index_param_rule = 0; index_param_rule < PyList_Size(params_rules); index_param_rule++) {
                if (!BuildJson(ready_json, raw_json, params_rules, rules, index_param_rule)) {
                    return NULL;
                }
            }

            PyList_Append(list_json, ready_json);
        }

        return list_json;
    }

    PyObject *obj = PyDict_New();
    for (int index_param_rule = 0; index_param_rule < PyList_Size(params_rules); index_param_rule++) {
        if (!BuildJson(obj, json, params_rules, rules, index_param_rule)) {
            return NULL;
        }
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
