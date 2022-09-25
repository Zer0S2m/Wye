#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>


struct Build;
struct KeysTreeList;
struct BuildFieldCheck;
struct HistoryBuild;

// Build json
PyObject *ValidationField(PyObject *validators, struct BuildFieldCheck build_field_check);
int *SetDefaultValue(struct Build build, struct BuildFieldCheck build_field_check);
int *CheckField(struct BuildFieldCheck build_field_check);
int *SetField(struct Build build, struct BuildFieldCheck build_field_check);
int IsThereAnyNestingInKey(PyObject *key, PyObject *keys_tree, int level_key_tree, Py_ssize_t level);
PyObject *GetParamFromLevelKeys(PyObject *keys_level);
PyObject *GetPartRule(PyObject *rules, PyObject *key_tree);
PyObject *GetPartReadyJson(PyObject *ready_json, PyObject *key_tree);
void BuildInitialAssemblyReadyJson(PyObject *ready_json, PyObject *keys_tree);
void FindAllKeysRawJson(PyObject *rules, struct KeysTreeList *keys_tree_list, PyObject *path, PyObject *alias_path);
int *BuildSingleField(struct Build build, PyObject *key_tree_element);
void ClearReadyJsonFromEmptyDict(PyObject *ready_json);
void SetDefaultValueInRawJson(PyObject *raw_json, PyObject *key_tree, PyObject *value);
int *BuildJson(struct Build build, struct HistoryBuild *history_build);
PyObject *BuildJsonFromList(struct Build build, PyObject *raw_json, struct HistoryBuild *history_build);
static PyObject *method_build_json(PyObject *self, PyObject *args);
static PyObject *method_is_validate(PyObject *self, PyObject *args);
static PyObject *method_build_json_from_object(PyObject *self, PyObject *args);


#define RESET_COLOR "\033[0m"
#define GREEN_COLOR "\033[32m"
#define RED_COLOR "\033[31m"

#define RULES_FIELD_KEY "RULES"
#define PY_RULES_FIELD_KEY PyUnicode_FromString(RULES_FIELD_KEY)
#define TYPE_OBJ_FIELD_KEY "type"

#define TYPE_FIELD_KEY "TYPE"
#define ALIAS_FIELD_KEY "ALIAS"
#define DEFAULT_FIELD_KEY "DEFAULT"
#define REQUIRED_FIELD_KEY "REQUIRED"
#define VALIDATORS_FIELD_KEY "VALIDATORS"
#define IS_SERIALIZER_FIELD_KEY "IS_SERIALIZER"
#define MAX_LENGTH_FIELD_KEY "MAX_LENGTH"
#define MIN_LENGTH_FIELD_KEY "MIN_LENGTH"
#define TYPE_FILL_FIELD_KEY "TYPE_FILL"
#define TYPES_FILL_FIELD_KEY "TYPES_FILL"
#define GT_FIELD_KEY "GT"
#define GE_FIELD_KEY "GE"
#define LT_FIELD_KEY "LT"
#define LE_FIELD_KEY "LE"

#define BUILD_RULE_METHOD PyUnicode_FromString("_build_rules")
#define RULES_ATTR_KEY PyUnicode_FromString("_rules")

#define LAST_KEY_TREE (Py_ssize_t) 1
#define SINGLE_LEVEL_JSON (Py_ssize_t) 2

#define GET_RULES(rules) PySequence_GetItem(rules, 0)
#define GET_TYPE(rules) PySequence_GetItem(rules, 1)
#define GET_RULES_SERIALIZER(rules) PyDict_GetItemString(rules, RULES_FIELD_KEY)
