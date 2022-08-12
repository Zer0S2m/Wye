#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <core/core.h>


int SetValidationError() {
    PyErr_SetString(PyExc_TypeError, "<WyeSerializers>: Validation error");
    return 0;
}

int SetAttributeError() {
    PyErr_SetString(PyExc_AttributeError, "<WyeSerializers>: Missing required parameters");
    return 0;
}

int SetValidationDefaultError() {
    PyErr_SetString(PyExc_AttributeError, "<WyeSerializers>: Validation error: default value");
    return 0;
}


int *SetDefaultValue(PyObject *obj, PyObject *rule, PyObject *param_title) {
    PyObject *default_value = PyDict_GetItemString(rule, DEFAULT_FIELD_KEY);
    PyObject *type = PyDict_GetItemString(rule, TYPE_FIELD_KEY);

    if (!PyObject_IsInstance(default_value, type) && default_value != Py_None) {
        return SetValidationDefaultError();
    }

    if (!CheckExpandedField(default_value, rule)) {
        return SetValidationDefaultError();
    }

    if (default_value != Py_None) {
        PyDict_SetItem(obj, param_title, default_value);
    }

    return 1;
}


int CheckFieldList(PyObject *json_field, PyObject *rule) {
    PyObject *expanded_rules = GET_EXPANDED_RULE(rule);
    PyObject *element_type = PyDict_GetItemString(expanded_rules, ARRAY_ELEMENT_TYPE_FIELD_KEY);

    for (int i_element = 0; i_element < PyList_Size(json_field); i_element++) {
        PyObject *element = PyList_GetItem(json_field, i_element);
        if (!PyObject_IsInstance(element, element_type)) {
            return 0;
        }
    }

    return 1;
}

int CheckFieldSetFrozen(PyObject *json_field, PyObject *rule) {
    PyObject *expanded_rules = GET_EXPANDED_RULE(rule);
    PyObject *element_type = PyDict_GetItemString(expanded_rules, ARRAY_ELEMENT_TYPE_FIELD_KEY);

    PyObject *list_elements = PySequence_List(json_field);

    for (int i_element = 0; i_element < PyList_Size(list_elements); i_element++) {
        PyObject *element = PyList_GetItem(list_elements, i_element);
        if (!PyObject_IsInstance(element, element_type)) {
            return 0;
        }
    }

    return 1;
}

int CheckFieldTuple(PyObject *json_field, PyObject *rule) {
    PyObject *expanded_rules = GET_EXPANDED_RULE(rule);
    PyObject *element_types = PyDict_GetItemString(expanded_rules, ARRAY_ELEMENT_TYPES_FIELD_KEY);
    for (int i_element = 0; i_element < PyTuple_Size(json_field); i_element++) {
        PyObject *element = PyTuple_GetItem(json_field, i_element);
        PyObject *element_type = PyObject_Type(element);

        if (!PySequence_Contains(element_types, element_type)) {
            return 0;
        }
    }

    return 1;
}

int CheckFieldDict(PyObject *json_field, PyObject *rule) {
    PyObject *expanded_rules = GET_EXPANDED_RULE(rule);
    PyObject *type_key_dict = PyDict_GetItemString(expanded_rules, TYPE_KEY_DICT_FIELD_KEY);
    PyObject *type_value_dict = PyDict_GetItemString(expanded_rules, TYPE_VALUE_DICT_FIELD_KEY);

    PyObject *keys_dict = PyDict_Keys(json_field);
    PyObject *values_dict = PyDict_Values(json_field);

    for (int i_element = 0; i_element < PyList_Size(keys_dict); i_element++) {
        PyObject *key_dict = PyList_GetItem(keys_dict, i_element);
        PyObject *value_dict = PyList_GetItem(values_dict, i_element);
        if (!PyObject_IsInstance(key_dict, type_key_dict)) {
            return 0;
        }
        if (!PyObject_IsInstance(value_dict, type_value_dict)) {
            return 0;
        }
    }

    return 1;
}


int CheckExpandedField(PyObject *json_field, PyObject *rule) {
    PyObject *is_expanded = PyDict_GetItemString(rule, EXPANDED_FIELD_KEY);
    if (PyObject_IsTrue(is_expanded)) {
        PyObject *expanded_rules_for = PyDict_GetItemString(rule, EXPANDED_RULES_FOR_FIELD_KEY);

        if (PyList_CheckExact(json_field)) {
            if (!CheckFieldList(json_field, rule)) {
                return SetValidationError();
            }
        }
        else if (PySet_Check(json_field) || PyFrozenSet_Check(json_field)) {
            if (!CheckFieldSetFrozen(json_field, rule)) {
                return SetValidationError();
            }
        }
        else if (PyTuple_Check(json_field)) {
            if (!CheckFieldTuple(json_field, rule)) {
                return SetValidationError();
            }
        }
        else if (PyDict_Check(json_field)) {
            if (!CheckFieldDict(json_field, rule)) {
                return SetValidationError();
            }
        }
    }

    return 1;
}


int CheckField(PyObject *json_field, PyObject *rule) {
    PyObject *types = PyDict_GetItemString(rule, TYPE_FIELD_KEY);
    PyObject *is_required = PyDict_GetItemString(rule, REQUIRED_FIELD_KEY);

    if (!json_field && PyObject_IsTrue(is_required)) {
        return SetAttributeError();
    }
    if (!json_field) {
        return 1;
    }
    if (!PySequence_Contains(types, PyObject_Type(json_field))) {
        return SetValidationError();
    }

    if (!CheckExpandedField(json_field, rule)) {
        return NULL;
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

    if (!CheckField(json_field, param_rule)) {
        return NULL;
    }

    PyObject *alias = PyDict_GetItemString(param_rule, ALIAS_FIELD_KEY);

    if (!json_field) {
        if (!SetDefaultValue(obj, param_rule, alias)) {
            return NULL;
        }
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


static PyObject *method_is_validate(PyObject *self, PyObject *args) {
    PyObject *obj = method_build_json(self, args);
    PyObject *list_err_obj = PyList_New(0);
    PyErr_Clear();

    if (!obj) {
        PyList_Append(list_err_obj, Py_False);
        PyList_Append(list_err_obj, Py_None);
    } else {
        PyList_Append(list_err_obj, Py_True);
        PyList_Append(list_err_obj, obj);
    }

    return PyList_AsTuple(list_err_obj);
}


static PyMethodDef WyeSerializersMethods[] = {
    {
        "build_json",
        method_build_json,
        METH_VARARGS,
        "Collects json according to the rules"
    },
    {
        "is_validate",
        method_is_validate,
        METH_VARARGS,
        "Validate objects"
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
