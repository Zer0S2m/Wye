#include <core/core.h>

#include "logs.c.h"
#include "error.c.h"
#include "validators.c.h"
#include "object_build.c.h"
#include "state.c.h"


struct Build {
    PyObject *raw_json; // in python -> Dict[str, Any]
    PyObject *ready_json; // in python -> Dict[str, Any]
    PyObject *rules; // in python -> Dict[str, Any]
    PyObject *base_serializer;
    int is_history;
};

struct KeysTreeList {
    PyObject *keys_tree; // in python -> List[str]
    PyObject *keys_tree_alias; // in python -> List[str]
};

struct BuildFieldCheck {
    PyObject *param_title; // in python str
    PyObject *rule; // in python -> dict
    PyObject *raw_json_obj; // in python -> typing.Any
    PyObject *base_serializer;
};

// For optimization
struct HistoryBuild {
    PyObject *keys_tree_param_title; // in python -> List[str]
    PyObject *part_rule; // in python -> list
    PyObject *keys_tree; // in python -> List[str]
    PyObject *keys_tree_alias; // in python -> List[str]
    PyObject *run_validators; // in python -> Dict[str, bool]
    int *is_any_nesting_in_key; // array
};


struct State __state = {
    NULL
};


/**
 * @brief
 *
 * @param validators in python -> List[Callable]
 * @param build_field_check
 * @return PyObject*
 */
PyObject *ValidationField(PyObject *validators, struct BuildFieldCheck build_field_check) {
    PyObject *new_value = RunValidators(build_field_check.raw_json_obj, validators);

    if (!CheckMaxMinLength(build_field_check.rule, new_value))
        return NULL;

    if (!CheckOpidNumber(build_field_check.rule, new_value))
        return NULL;

    PyObject *fill_type = PyDict_GetItemString(build_field_check.rule, TYPE_FILL_FIELD_KEY);
    if (fill_type && fill_type != Py_None) {
        int is_subclass_base_serializer = PyObject_IsSubclass(fill_type, build_field_check.base_serializer);
        if (!is_subclass_base_serializer) {
            if (!CheckFillType(new_value, fill_type))
                return NULL;
        } else if (is_subclass_base_serializer) {
            PyObject *isinstance_nested_ser = PyObject_CallObject(fill_type, NULL);
            PyObject *rules_nested_ser = PyObject_CallMethodNoArgs(isinstance_nested_ser, BUILD_RULE_METHOD);

            struct HistoryBuild history_build = {
                PyList_New(0),
                PyList_New(0),
                PyList_New(0),
                PyList_New(0),
                PyDict_New()
            };

            struct Build new_build = {
                NULL,
                PyDict_New(),
                rules_nested_ser,
                build_field_check.base_serializer,
                0
            };
            new_value = BuildJsonFromList(
                new_build,
                build_field_check.raw_json_obj,
                &history_build,
                &__state
            );
        }
    }

    PyObject *fill_types = PyDict_GetItemString(build_field_check.rule, TYPES_FILL_FIELD_KEY);
    if (fill_types && fill_types != Py_None) {
        if (PyObject_Length(fill_types) > 0) {
            if (!CheckFillTypes(new_value, fill_types))
                return NULL;
        }
    }

    return new_value;
}


/**
 * @brief Set the Default Value object
 *
 * @param build
 * @param build_field_check
 * @return int*
*/
int *SetDefaultValue(struct Build build, struct BuildFieldCheck build_field_check) {
    PyObject *default_value = PyDict_GetItemString(build_field_check.rule, DEFAULT_FIELD_KEY);
    PyObject *type_field = PyDict_GetItemString(build_field_check.rule, TYPE_FIELD_KEY);

    if (default_value != Py_None && !PyObject_IsInstance(default_value, type_field))
        return SetValidationDefaultError();

    if (default_value == Py_None)
        return (int *) 1;

    build_field_check.raw_json_obj = default_value;
    PyObject *validators = PyDict_GetItemString(build_field_check.rule, VALIDATORS_FIELD_KEY);
    PyObject *new_value = ValidationField(validators, build_field_check);
    if (!new_value)
        return NULL;

    PyObject *alias = PyDict_GetItemString(build_field_check.rule, ALIAS_FIELD_KEY);
    PyDict_SetItem(build.ready_json, alias, new_value);

    return (int *) 1;
}


/**
 * @brief
 *
 * @param build_field_check
 * @return int*
*/
int *CheckField(struct BuildFieldCheck build_field_check) {
    PyObject *is_required = PyDict_GetItemString(build_field_check.rule, REQUIRED_FIELD_KEY);
    if (!build_field_check.raw_json_obj && PyObject_IsTrue(is_required)) {
        PyObject *default_value = PyDict_GetItemString(build_field_check.rule, DEFAULT_FIELD_KEY);
        if (default_value == Py_None)
            return SetAttributeError();
    }

    // If the "required" parameter is False
    if (!build_field_check.raw_json_obj)
        return (int *) 1;

    PyObject *types_field = PyDict_GetItemString(build_field_check.rule, TYPE_FIELD_KEY);
    if (!PySequence_Contains(types_field, PyObject_Type(build_field_check.raw_json_obj)))
        return SetValidationError();

    return (int *) 1;
}


/**
 * @brief Set the Field object
 *
 * @param build
 * @param build_field_check
 * @return int*
*/
int *SetField(struct Build build, struct BuildFieldCheck build_field_check) {
    if (!build_field_check.raw_json_obj) {
        if (!SetDefaultValue(build, build_field_check))
            return NULL;

        return (int *) 1;
    }

    PyObject *validators = PyDict_GetItemString(build_field_check.rule, VALIDATORS_FIELD_KEY);
    PyObject *new_value = ValidationField(validators, build_field_check);
    if (!new_value)
        return NULL;

    PyObject *alias = PyDict_GetItemString(build_field_check.rule, ALIAS_FIELD_KEY);
    PyDict_SetItem(build.ready_json, alias, new_value);

    return (int *) 1;
}


/**
 * @brief
 *
 * @param key in python -> str
 * @param keys_tree in python -> list
 * @param level_key in python -> int
 * @return int*
 */
int IsThereAnyNestingInKey(PyObject *key, PyObject *keys_tree, int level_key_tree, Py_ssize_t level) {
    if (PyList_Size(keys_tree) <= level_key_tree + 1)
        return 0;

    PyObject *next_key_tree = PyList_GET_ITEM(keys_tree, level_key_tree + 1);
    PyObject *current_key_tree = PyList_GetItem(keys_tree, level_key_tree);

    if (PyList_Size(current_key_tree) > PyList_Size(next_key_tree))
        return 0;

    PyObject *first_key_tree = PyList_GetItem(next_key_tree, level);

    if (key == first_key_tree)
        return 1;

    return 0;
}


/**
 * @brief Get the Level Key Param object
 *
 * @param keys_level in python -> List[str]; example - ["param_1", "param_2"]
 * @return PyObject*
*/
PyObject *GetParamFromLevelKeys(PyObject *keys_level) {
    Py_ssize_t length_keys_level = PyList_Size(keys_level);
    return PyList_GetItem(keys_level, length_keys_level - 1);
}


/**
 * @brief Get the Part Raw Json object
 *
 * @param raw_json in python -> Dict[str, Any]
 * @param key_tree in python -> List[str]
 * @return PyObject*
*/
PyObject *GetPartRawJson(PyObject *raw_json, PyObject *key_tree) {
    return GetPartReadyJson(raw_json, key_tree);
}


/**
 * @brief Get the Part Rule object
 *
 * @param rule in python -> Dict[str, Any]
 * @param key_tree in python -> List[str]
 * @return PyObject*
*/
PyObject *GetPartRule(PyObject *rule, PyObject *key_tree) {
    return GetPartReadyJson(rule, key_tree);
}


/**
 * @brief Get the Part Ready Json object
 *
 * @param ready_json in python -> Dict[str, Any]
 * @param key_tree in python -> List[str]; example ["param_1", "param_2"]
 * @return PyObject* - in python -> dict
*/
PyObject *GetPartReadyJson(PyObject *ready_json, PyObject *key_tree) {
    PyObject *part_ready_json = ready_json;
    Py_ssize_t length_key_rule = PyList_Size(key_tree);

    for (int i_key_tree = 0; i_key_tree < length_key_rule - 1; i_key_tree++) {
        PyObject *key_tree_element = PyList_GetItem(key_tree, i_key_tree);

        part_ready_json = PyDict_GetItem(part_ready_json, key_tree_element);
        if (!part_ready_json)
            return NULL;
    }

    return part_ready_json;
}


/**
 * @brief
 *
 * @param ready_json in python -> Dict[str, Any]
 * @param keys_tree in python -> List[List[str]]; example [["param_1"], ["param_1", "param_2"]] ->
 * {"param_1": {"param_2": ...}}
*/
void BuildInitialAssemblyReadyJson(PyObject *ready_json, PyObject *keys_tree) {
    PyObject *part_ready_json;

    for (int i_key_tree = 0; i_key_tree < PyList_Size(keys_tree); i_key_tree++) {
        part_ready_json = ready_json;

        PyObject *key_tree = PyList_GetItem(keys_tree, i_key_tree);
        Py_ssize_t length_key_tree = PyList_Size(key_tree);

        if (length_key_tree == 1)
            continue;

        for (int j_key_tree = 0; j_key_tree < length_key_tree; j_key_tree++) {
            if (j_key_tree == length_key_tree - 1)
                break;

            PyObject *key_tree_part = PyList_GetItem(key_tree, j_key_tree);

            PyObject *key_tree_part_from_dict = PyDict_GetItem(part_ready_json, key_tree_part);
            if (key_tree_part_from_dict) {
                part_ready_json = PyDict_GetItem(part_ready_json, key_tree_part);
                continue;
            } else
                PyDict_SetItem(part_ready_json, key_tree_part, PyDict_New());
        }
    }
}


/**
 * @brief
 *
 * @param rules in python -> Dict[str, Any]
 * @param result in python -> List[List[str]]
 * @param path in python -> List[str]
 * @param alias_path in python -> List[str]
*/
void FindAllKeysRawJson(
    PyObject *rules,
    struct KeysTreeList *keys_tree_list,
    PyObject *path,
    PyObject *alias_path
) {
    if (PyDict_Check(rules)) {
        PyObject *keys = PyDict_Keys(rules);
        PyObject *values = PyDict_Values(rules);

        for (int i_key = 0; i_key < PyList_Size(keys); i_key++) {
            PyObject *key = PyList_GetItem(keys, i_key);
            if (PyUnicode_Compare(key, PY_RULES_FIELD_KEY)) {
                PyObject *extend_list_new_path = PyList_New(0);
                PyList_Append(extend_list_new_path, key);
                PyObject *extend_list_new_alias_path = PyList_New(0);

                PyObject *part_rule = PyList_GetItem(values, i_key);

                PyObject *params_rules = PyDict_GetItem(part_rule, PY_RULES_FIELD_KEY);
                if (params_rules) {
                    PyObject *rules_for_alias = GET_RULES(params_rules);
                    PyObject *alias = PyDict_GetItemString(rules_for_alias, ALIAS_FIELD_KEY);
                    PyList_Append(extend_list_new_alias_path, alias);
                } else {
                    params_rules = GET_RULES(PyDict_GetItem(rules, key));
                    PyObject *alias = PyDict_GetItemString(params_rules, ALIAS_FIELD_KEY);
                    PyList_Append(extend_list_new_alias_path, alias);
                }

                PyObject *new_path = PyList_New(0);
                PyObject_CallMethod(new_path, "extend", "O", path);
                PyObject_CallMethod(new_path, "extend", "O", extend_list_new_path);
                PyObject *new_alias_path = PyList_New(0);
                PyObject_CallMethod(new_alias_path, "extend", "O", alias_path);
                PyObject_CallMethod(new_alias_path, "extend", "O", extend_list_new_alias_path);

                PyList_Append(keys_tree_list->keys_tree, new_path);
                PyList_Append(keys_tree_list->keys_tree_alias, new_alias_path);

                FindAllKeysRawJson(part_rule, keys_tree_list, new_path, new_alias_path);
            }
        }
    }
}


/**
 * @brief
 *
 * @param build
 * @param key_tree_element in python -> str
 * @return int*
*/
int *BuildSingleField(struct Build build, PyObject *key_tree_element) {
    PyObject *rules_param = PyDict_GetItem(build.rules, key_tree_element);

    PyObject *rule = GET_RULES(rules_param);
    PyObject *raw_json_obj = PyDict_GetItem(build.raw_json, key_tree_element);

    struct BuildFieldCheck build_field_check = {
        key_tree_element,
        rule,
        raw_json_obj,
        build.base_serializer
    };

    if (!CheckField(build_field_check))
        return NULL;

    if (!SetField(build, build_field_check))
        return NULL;

    return (int *) 1;
}


void ClearReadyJsonFromEmptyDict(PyObject *ready_json) {
    if (PyDict_Check(ready_json)) {
        PyObject *keys = PyDict_Keys(ready_json);
        PyObject *values = PyDict_Values(ready_json);

        for (int i_key = 0; i_key < PyList_Size(keys); i_key++) {
            PyObject *key = PyList_GetItem(keys, i_key);
            PyObject *value = PyDict_GetItem(ready_json, key);
            if (PyDict_Check(value)) {
                if (PyObject_Length(value) == 0) {
                    PyDict_DelItem(ready_json, key);
                    PyErr_Clear();
                }
            }

            PyObject *part_ready_json = PyList_GetItem(values, i_key);

            ClearReadyJsonFromEmptyDict(part_ready_json);
        }
    }
}


/**
 * @brief Set the Default Value In Raw Json object
 *
 * @param raw_json in python -> Dict[str, ANy]
 * @param key_tree in python -> List[str]
 * @param value in python -> Any
 */
void SetDefaultValueInRawJson(PyObject *raw_json, PyObject *key_tree, PyObject *value) {
    PyObject *part_raw_json = raw_json;

    Py_ssize_t length_key_tree = PyList_Size(key_tree);
    for (Py_ssize_t i_key_tree = 0; i_key_tree < length_key_tree - SINGLE_LEVEL_JSON; i_key_tree++) {
        PyObject *param = PyList_GetItem(key_tree, i_key_tree);

        if (!PyDict_GetItem(part_raw_json, param))
            PyDict_SetItem(part_raw_json, param, PyDict_New());

        part_raw_json = PyDict_GetItem(part_raw_json, param);
    }

    PyDict_SetItem(
        part_raw_json,
        PyList_GetItem(key_tree, length_key_tree - SINGLE_LEVEL_JSON),
        value
    );
}


/**
 * @brief
 *
 * @param build
 * @param history_build
 * @return int*
*/
int *BuildJson(struct Build build, struct HistoryBuild *history_build, struct State *state) {
    struct KeysTreeList keys_tree_list = { PyList_New(0), PyList_New(0) };
    PyObject *keys_tree = keys_tree_list.keys_tree;
    PyObject *keys_tree_alias = keys_tree_list.keys_tree_alias;
    if (!build.is_history) {
        FindAllKeysRawJson(build.rules, &keys_tree_list, PyList_New(0), PyList_New(0));
        history_build->keys_tree = keys_tree;
        history_build->keys_tree_alias = keys_tree_alias;
        history_build->is_any_nesting_in_key = (int *) malloc(
            sizeof(int) * PyList_Size(keys_tree) - (Py_ssize_t) 1
        );
    } else {
        keys_tree = history_build->keys_tree;
        keys_tree_alias = history_build->keys_tree_alias;
    }


    BuildInitialAssemblyReadyJson(build.ready_json, keys_tree_alias);

    for (int i_key_tree = 0; i_key_tree < PyList_Size(keys_tree); i_key_tree++) {
        PyObject *key_tree = PyList_GetItem(keys_tree, i_key_tree);
        Py_ssize_t length_key_tree = PyList_Size(key_tree);
        PyObject *key_tree_element = PyList_GetItem(key_tree, length_key_tree - 1);

        int is_any_nesting_in_key;

        if (length_key_tree == 1) {
            PyList_Append(history_build->keys_tree_param_title, key_tree_element);
            PyList_Append(history_build->part_rule, PyLong_FromLong(0));

            if (!build.is_history) {
                is_any_nesting_in_key = IsThereAnyNestingInKey(
                    key_tree_element,
                    keys_tree,
                    i_key_tree,
                    length_key_tree - LAST_KEY_TREE
                );
                history_build->is_any_nesting_in_key[i_key_tree] = is_any_nesting_in_key;
                if (is_any_nesting_in_key)
                    continue;
            } else if (history_build->is_any_nesting_in_key[i_key_tree])
                continue;

            if (!BuildSingleField(build, key_tree_element)) {
                SetCurrentField(state, key_tree_element);
                SetWarningStr(state);
                return NULL;
            }
        } else {
            PyObject *param_title, *part_rule;
            if (!build.is_history) {
                param_title = GetParamFromLevelKeys(key_tree);
                part_rule = GetPartRule(build.rules, key_tree);

                PyList_Append(history_build->keys_tree_param_title, param_title);
                PyList_Append(history_build->part_rule, part_rule);
            }
            else {
                param_title = PyList_GetItem(history_build->keys_tree_param_title, i_key_tree);
                part_rule = PyList_GetItem(history_build->part_rule, i_key_tree);
            }

            if (!build.is_history) {
                is_any_nesting_in_key = IsThereAnyNestingInKey(
                    key_tree_element,
                    keys_tree,
                    i_key_tree,
                    length_key_tree - LAST_KEY_TREE
                );
                history_build->is_any_nesting_in_key[i_key_tree] = is_any_nesting_in_key;
                if (is_any_nesting_in_key)
                    continue;
            } else if (history_build->is_any_nesting_in_key[i_key_tree])
                continue;

            PyObject *key_tree_alias = PyList_GetItem(keys_tree_alias, i_key_tree);
            PyObject *part_ready_json = GetPartReadyJson(build.ready_json, key_tree_alias);
            PyObject *rules_params = GET_RULES_SERIALIZER(part_rule);
            PyObject *rules = GET_RULES(rules_params);

            PyObject *part_raw_json = GetPartRawJson(build.raw_json, key_tree);
            if (!part_raw_json) {
                PyObject *default_value = PyDict_GetItemString(rules, DEFAULT_FIELD_KEY);
                if (default_value == Py_None && !PyDict_Check(default_value) &&
                    PyObject_IsTrue(PyDict_GetItemString(rules, REQUIRED_FIELD_KEY)))
                        return SetAttributeError();
                else if (default_value != Py_None && PyDict_Check(default_value)) {
                    SetDefaultValueInRawJson(build.raw_json, key_tree, default_value);
                    part_raw_json = GetPartRawJson(build.raw_json, key_tree);
                }
                else
                    continue;
            }

            PyObject *validators = PyDict_GetItemString(rules, VALIDATORS_FIELD_KEY);
            if (PySequence_Length(validators) > 0) {
                PyObject *path_to_serializer_dict = SplitKeysTreeRunValidatorDict(key_tree);
                if (!PyDict_GetItem(history_build->run_validators, path_to_serializer_dict)) {
                    part_raw_json = RunValidators(part_raw_json, validators);
                    PyDict_SetItem(history_build->run_validators, path_to_serializer_dict, Py_True);
                };
            }

            struct Build new_build = { part_raw_json, part_ready_json, part_rule };

            if (!BuildSingleField(new_build, param_title)) {
                SetCurrentField(state, param_title);
                SetWarningStr(state);
                return NULL;
            }
        }
    }

    ClearReadyJsonFromEmptyDict(build.ready_json);

    return (int *) 1;
}


/**
 * @brief
 *
 * @param build
 * @param raw_json in python -> List[Dict[str, Any]]
 * @param history_build
 * @return PyObject*
 */
PyObject *BuildJsonFromList(
    struct Build build,
    PyObject *raw_json,
    struct HistoryBuild *history_build,
    struct State *state
) {
    Py_ssize_t length_raw_json = PyList_Size(raw_json);
    PyObject *list_ready_json = PyList_New(length_raw_json);

    for (int i_raw_json_obj = 0; i_raw_json_obj < length_raw_json; i_raw_json_obj++) {
        PyObject *raw_json_obj = PyList_GetItem(raw_json, i_raw_json_obj);
        build.raw_json = raw_json_obj;

        if (!BuildJson(build, history_build, state))
            return NULL;

        build.is_history = 1;

        PyList_SetItem(list_ready_json, i_raw_json_obj, PyDict_Copy(build.ready_json));
        PyDict_Clear(build.ready_json);
        PyDict_Clear(history_build->run_validators);
    }

    return list_ready_json;
}


/**
 * @brief
 *
 * @param self
 * @param args
 * @return PyObject*
 */
static PyObject *method_build_json(PyObject *self, PyObject *args) {
    PyObject *raw_json;
    struct Build build;

    if (!PyArg_ParseTuple(args, "OOO", &raw_json, &build.rules, &build.base_serializer))
        return NULL;

    struct HistoryBuild history_build = {
        PyList_New(0),
        PyList_New(0),
        PyList_New(0),
        PyList_New(0),
        PyDict_New()
    };
    build.is_history = 0;

    build.ready_json = PyDict_New();

    if (PyList_Check(raw_json)) {
        PyObject *list_ready_json = BuildJsonFromList(build, raw_json, &history_build, &__state);
        if (!list_ready_json) {
            DisplayLog(__state.current_error, "r");
            return NULL;
        }

        build.ready_json = list_ready_json;
    } else {
        build.raw_json = raw_json;
        if (!BuildJson(build, &history_build, &__state)) {
            DisplayLog(__state.current_error, "r");
            return NULL;
        }
    }

    free(history_build.is_any_nesting_in_key);

    return build.ready_json;
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


static PyObject *method_build_json_from_object(PyObject *self, PyObject *args) {
    PyObject *raw_object;
    struct Build build;

    if (!PyArg_ParseTuple(args, "OOO", &raw_object, &build.rules, &build.base_serializer))
        return NULL;

    struct HistoryBuild history_build = {
        PyList_New(0),
        PyList_New(0),
        PyList_New(0),
        PyList_New(0),
        PyDict_New()
    };
    build.is_history = 0;

    build.ready_json = PyDict_New();

    struct KeysTreeList keys_tree_list = { PyList_New(0), PyList_New(0) };
    FindAllKeysRawJson(build.rules, &keys_tree_list, PyList_New(0), PyList_New(0));

    if (PyList_Check(raw_object)) {
        Py_ssize_t length_raw_objects = PyList_Size(raw_object);
        PyObject *raw_json_list = PyList_New(length_raw_objects);

        for (int i_raw_object = 0; i_raw_object < length_raw_objects; i_raw_object++) {
            PyObject *raw_object_ = PyList_GetItem(raw_object, i_raw_object);
            PyObject *raw_json = PyDict_New();
            ConvertObjectToJson(raw_json, raw_object_, keys_tree_list.keys_tree);
            PyList_SetItem(raw_json_list, i_raw_object, raw_json);
        }

        PyObject *list_ready_json = BuildJsonFromList(build, raw_json_list, &history_build, &__state);
        if (!list_ready_json)
            return NULL;

        build.ready_json = list_ready_json;
    } else {
        PyObject *raw_json = PyDict_New();
        ConvertObjectToJson(raw_json, raw_object, keys_tree_list.keys_tree);
        build.raw_json = raw_json;
        if (!BuildJson(build, &history_build, &__state))
            return NULL;
    }

    free(history_build.is_any_nesting_in_key);

    return build.ready_json;
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
        "Checks if json is valid"
    },
    {
        "build_json_from_object",
        method_build_json_from_object,
        METH_VARARGS,
        "Collects json from objects according to the rules"
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
