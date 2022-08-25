#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <stdio.h>
#include <stdlib.h>


struct Build;
struct BuildFieldCheck;

// Set error
int *SetValidationError();
int *SetAttributeError();
int *SetValidationDefaultError();

// Build json
int *SetDefaultValue(struct Build build, struct BuildFieldCheck build_field_check);
int *CheckField(struct BuildFieldCheck build_field_check);
int *SetField(struct Build build, struct BuildFieldCheck build_field_check);
int *IsThereAnyNestingInKey(PyObject *key, PyObject *keys_tree, int level_key_tree, int level);
PyObject *GetParamFromLevelKeys(PyObject *keys_level);
PyObject *GetPartRule(PyObject *rules, PyObject *key_tree);
PyObject *GetPartReadyJson(PyObject *ready_json, PyObject *key_tree);
void BuildInitialAssemblyReadyJson(PyObject *ready_json, PyObject *keys_tree);
void FindAllKeysRawJson(PyObject *rules, PyObject *result, PyObject *path);
int *BuildSingleField(struct Build build, PyObject *key_tree_element);
int *BuildJson(struct Build build);
PyObject *BuildJsonFromList(struct Build obj_build, PyObject *raw_json);
static PyObject *method_build_json(PyObject *self, PyObject *args);


#define RULES_FIELD_KEY "RULES"
#define PY_RULES_FIELD_KEY PyUnicode_FromString(RULES_FIELD_KEY)
#define TYPE_OBJ_FIELD_KEY "type"

#define TYPE_FIELD_KEY "TYPE"
#define ALIAS_FIELD_KEY "ALIAS"
#define DEFAULT_FIELD_KEY "DEFAULT"
#define REQUIRED_FIELD_KEY "REQUIRED"
#define IS_SERIALIZER_FIELD_KEY "IS_SERIALIZER"

#define GET_RULES(rules) PySequence_GetItem(rules, 0)
