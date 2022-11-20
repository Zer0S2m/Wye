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
struct State;

// Build json
PyObject *ValidationField(PyObject *, struct BuildFieldCheck);
int *SetDefaultValue(struct Build, struct BuildFieldCheck);
int *CheckField(struct BuildFieldCheck);
int *SetField(struct Build, struct BuildFieldCheck);
int IsThereAnyNestingInKey(PyObject *, PyObject *, int, Py_ssize_t);
PyObject *GetParamFromLevelKeys(PyObject *);
PyObject *GetPartRule(PyObject *, PyObject *);
PyObject *GetPartReadyJson(PyObject *, PyObject *);
void BuildInitialAssemblyReadyJson(PyObject *, PyObject *);
void FindAllKeysRawJson(PyObject *, struct KeysTreeList *, PyObject *, PyObject *);
int *BuildSingleField(struct Build, PyObject *);
void ClearReadyJsonFromEmptyDict(PyObject *);
void SetDefaultValueInRawJson(PyObject *, PyObject *, PyObject *);
int *BuildJson(struct Build, struct HistoryBuild *, struct State *);
PyObject *BuildJsonFromList(struct Build , PyObject *, struct HistoryBuild *, struct State *);

static PyObject *method_build_json(PyObject *, PyObject *);
static PyObject *method_is_validate(PyObject *, PyObject *);
static PyObject *method_build_json_from_object(PyObject *, PyObject *);


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

#define WARNING_REQUIRED_PARAM_STR PyUnicode_FromString(" - is a required field")

#define LAST_KEY_TREE (Py_ssize_t) 1
#define SINGLE_LEVEL_JSON (Py_ssize_t) 2

#define GET_RULES(rules) PySequence_GetItem(rules, 0)
#define GET_TYPE(rules) PySequence_GetItem(rules, 1)
#define GET_RULES_SERIALIZER(rules) PyDict_GetItemString(rules, RULES_FIELD_KEY)
