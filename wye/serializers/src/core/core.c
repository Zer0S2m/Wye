#include <core/core.h>


struct BuildState {
    int level;
    PyObject *rule;
    PyObject *keys_level; // Example: ["param_1", "param_2"] -> {"param_1": {"param_2": ...}}
    PyObject *ready_json;
    PyObject *raw_json_obj;
};

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


int *SetValidationError() {
    PyErr_SetString(PyExc_TypeError, "<WyeSerializers>: Validation error");
    return (int *) 0;
}

int *SetAttributeError() {
    PyErr_SetString(PyExc_AttributeError, "<WyeSerializers>: Missing required parameters");
    return (int *) 0;
}

int *SetValidationDefaultError() {
    PyErr_SetString(PyExc_AttributeError, "<WyeSerializers>: Validation error: default value");
    return (int *) 0;
}


int *SetDefaultValue(struct Build obj_build, struct BuildFieldCheck build_field_check) {
    PyObject *default_value = PyDict_GetItemString(build_field_check.rule, DEFAULT_FIELD_KEY);
    PyObject *type_field = PyDict_GetItemString(build_field_check.rule, TYPE_FIELD_KEY);

    if (!PyObject_IsInstance(default_value, type_field) && default_value != Py_None)
        return SetValidationDefaultError();

    if (default_value == Py_None)
        return (int *) 1;

    PyObject *alias = PyDict_GetItemString(build_field_check.rule, ALIAS_FIELD_KEY);
    PyDict_SetItem(obj_build.ready_json, alias, default_value);

    return (int *) 1;
}


int *CheckField(struct BuildFieldCheck build_field_check) {
    PyObject *is_required = PyDict_GetItemString(build_field_check.rule, REQUIRED_FIELD_KEY);
    if (!build_field_check.raw_json_obj && PyObject_IsTrue(is_required))
        return SetAttributeError();

    // If the "required" parameter is False
    if (!build_field_check.raw_json_obj)
        return (int *) 1;

    PyObject *types_field = PyDict_GetItemString(build_field_check.rule, TYPE_FIELD_KEY);
    if (!PySequence_Contains(types_field, PyObject_Type(build_field_check.raw_json_obj)))
        return SetValidationError();

    return (int *) 1;
}


int *SetField(struct Build obj_build, struct BuildFieldCheck build_field_check) {
    if (!build_field_check.raw_json_obj) {
        if (!SetDefaultValue(obj_build, build_field_check))
            return NULL;

        return (int *) 1;
    }

    PyObject *alias = PyDict_GetItemString(build_field_check.rule, ALIAS_FIELD_KEY);
    PyDict_SetItem(obj_build.ready_json, alias, build_field_check.raw_json_obj);

    return (int *) 1;
}


int *GetReadyJsonPart(struct BuildState build_state) {
    int length_keys_level = PyList_Size(build_state.keys_level);

    if (length_keys_level == 1)
        return NULL;

    for (int i_key_level = 0; i_key_level < length_keys_level - 1; i_key_level++) {
        PyObject *key_level = PyList_GetItem(build_state.keys_level, i_key_level);

        PyObject *raw_jsom_obj_by_level = PyDict_GetItem(build_state.raw_json_obj, key_level);
        if (!raw_jsom_obj_by_level)
            return (int *) 0;

        PyObject *ready_json_obj_by_level = PyDict_GetItem(build_state.ready_json, key_level);

        if (!ready_json_obj_by_level) {
            PyObject *alias_key_json = PyDict_GetItemString(build_state.rule, ALIAS_FIELD_KEY);
            PyDict_SetItem(build_state.ready_json, alias_key_json, PyDict_New());
            ready_json_obj_by_level = build_state.ready_json;
            PySequence_SetItem(build_state.keys_level, i_key_level, alias_key_json);
        }

        build_state.ready_json = ready_json_obj_by_level;
    }

    return (int *) 1;
}


PyObject *GetLevelKeyParam(PyObject *keys_level) {
    int length_keys_level = PyList_Size(keys_level);
    return PyList_GetItem(keys_level, length_keys_level - 1);
}


int *_BuildJson(struct BuildState build_state) {
    return (int *) 1;
}


int *BuildJson(struct Build obj_build, struct BuildState build_state) {
    PyObject *keys_rules = PyDict_Keys(obj_build.rules);

    for (int i_key_rule = 0; i_key_rule < PyList_Size(keys_rules); i_key_rule++) {
        PyObject *key_rule_title = PyList_GetItem(keys_rules, i_key_rule);
        PyObject *rules_param = PyDict_GetItem(obj_build.rules, key_rule_title);

        PyObject *rule = PyDict_GetItemString(rules_param, RULES_FIELD_KEY);
        PyObject *is_serializer = PyDict_GetItemString(rule, IS_SERIALIZER_FIELD_KEY);
        PyObject *raw_json_obj = PyDict_GetItem(obj_build.raw_json, key_rule_title);

        PyObject *keys_level = PyList_New(0);
        PyList_Append(keys_level, key_rule_title);

        build_state.rule = rule;
        build_state.keys_level = keys_level;
        build_state.raw_json_obj = raw_json_obj;
        GetReadyJsonPart(build_state);

        PyObject *level_key_param = GetLevelKeyParam(build_state.keys_level);
        struct BuildFieldCheck build_field_check = { level_key_param, build_state.rule, build_state.raw_json_obj };

        if (!CheckField(build_field_check))
            return NULL;

        if (!SetField(obj_build, build_field_check))
            return NULL;
    }

    return (int *) 1;
}


PyObject *BuildJsonFromList(struct Build obj_build, PyObject *raw_json) {
    PyObject *list_ready_json = PyList_New(0);

    for (int i_raw_json_obj = 0; i_raw_json_obj < PyList_Size(raw_json); i_raw_json_obj++) {
        struct BuildState build_state;

        PyObject *raw_json_obj = PyList_GetItem(raw_json, i_raw_json_obj);
        obj_build.raw_json = raw_json_obj;

        if (!BuildJson(obj_build, build_state))
            return NULL;

        PyList_Append(list_ready_json, PyDict_Copy(obj_build.ready_json));
        PyDict_Clear(obj_build.ready_json);
    }

    return list_ready_json;
}


static PyObject *method_build_json(PyObject *self, PyObject *args) {
    PyObject *raw_json;
    struct Build obj_build;

    if (!PyArg_ParseTuple(args, "OO", &raw_json, &obj_build.rules))
        return NULL;

    obj_build.ready_json = PyDict_New();

    if (PyList_Check(raw_json)) {
        PyObject *list_ready_json = BuildJsonFromList(obj_build, raw_json);
        if (!list_ready_json)
            return NULL;

        obj_build.ready_json = list_ready_json;
    } else {
        obj_build.raw_json = raw_json;
        struct BuildState build_state = { 0, NULL, NULL, obj_build.ready_json , NULL };

        if (!BuildJson(obj_build, build_state))
            return NULL;
    }

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
