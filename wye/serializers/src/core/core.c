#include <core/core.h>


struct Build {
    PyObject *raw_json;
    PyObject *ready_json;
    PyObject *rules;
};

struct BuildFieldCheck {
    PyObject *param_title;
    PyObject *rule;
    PyObject *raw_json_obj;
};


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


int *SetDefaultValue(struct Build obj_build, struct BuildFieldCheck build_field_check) {
    PyObject *default_value = PyDict_GetItemString(build_field_check.rule, DEFAULT_FIELD_KEY);
    PyObject *type_field = PyDict_GetItemString(build_field_check.rule, TYPE_FIELD_KEY);

    if (!PyObject_IsInstance(default_value, type_field) && default_value != Py_None)
        return SetValidationDefaultError();

    if (default_value == Py_None)
        return 1;

    PyObject *alias = PyDict_GetItemString(build_field_check.rule, ALIAS_FIELD_KEY);
    PyDict_SetItem(obj_build.ready_json, alias, default_value);

    return 1;
}


int *CheckField(struct BuildFieldCheck build_field_check) {
    PyObject *is_required = PyDict_GetItemString(build_field_check.rule, REQUIRED_FIELD_KEY);
    if (!build_field_check.raw_json_obj && PyObject_IsTrue(is_required))
        return SetAttributeError();

    // If the "required" parameter is False
    if (!build_field_check.raw_json_obj)
        return 1;

    PyObject *types_field = PyDict_GetItemString(build_field_check.rule, TYPE_FIELD_KEY);
    if (!PySequence_Contains(types_field, PyObject_Type(build_field_check.raw_json_obj)))
        return SetValidationError();

    return 1;
}


int *SetField(struct Build obj_build, struct BuildFieldCheck build_field_check) {
    if (!build_field_check.raw_json_obj) {
        if (!SetDefaultValue(obj_build, build_field_check))
            return NULL;

        return 1;
    }

    PyObject *alias = PyDict_GetItemString(build_field_check.rule, ALIAS_FIELD_KEY);
    PyDict_SetItem(obj_build.ready_json, alias, build_field_check.raw_json_obj);

    return 1;
}


PyObject *BuildJson(struct Build obj_build) {
    PyObject *keys_rules = PyDict_Keys(obj_build.rules);

    for (int i_key_rule = 0; i_key_rule < PyList_Size(keys_rules); i_key_rule++) {
        PyObject *key_rule_title = PyList_GetItem(keys_rules, i_key_rule);
        PyObject *rules_param = PyDict_GetItem(obj_build.rules, key_rule_title);
        PyObject *rule = PyDict_GetItemString(rules_param, RULES_FIELD_KEY);

        if (!rule) {

        } else {
            PyObject *raw_json_obj = PyDict_GetItem(obj_build.raw_json, key_rule_title);

            struct BuildFieldCheck build_field_check = { key_rule_title, rule, raw_json_obj };

            if (!CheckField(build_field_check))
                return NULL;

            if (!SetField(obj_build, build_field_check))
                return NULL;
        }
    }

    return 1;
}


static PyObject *method_build_json(PyObject *self, PyObject *args) {
    struct Build obj_build;

    if (!PyArg_ParseTuple(args, "OO", &obj_build.raw_json, &obj_build.rules))
        return NULL;

    obj_build.ready_json = PyDict_New();
    if (!BuildJson(obj_build))
        return NULL;

    return obj_build.ready_json;
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
