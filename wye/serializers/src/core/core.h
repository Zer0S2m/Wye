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
int *IsThereAnyNestingInKey(PyObject *key, PyObject *keys_tree, int level_key);
PyObject *GetParamFromLevelKeys(PyObject *keys_level);
PyObject *GetPartReadyJson(PyObject *ready_json, PyObject *key_tree);
PyObject *GetPartRule(PyObject *rules, PyObject *key_tree);
void BuildInitialAssemblyReadyJson(PyObject *ready_json, PyObject *keys_tree);
void FindAllKeysRawJson(PyObject *raw_json, PyObject *result, PyObject *path);
int *BuildSingleField(struct Build build, PyObject *key_tree_element);
int *BuildJson(struct Build build);
PyObject *BuildJsonFromList(struct Build obj_build, PyObject *raw_json);
static PyObject *method_build_json(PyObject *self, PyObject *args);


#define RULES_FIELD_KEY "rules"

#define TYPE_FIELD_KEY "TYPE"
#define ALIAS_FIELD_KEY "ALIAS"
#define DEFAULT_FIELD_KEY "DEFAULT"
#define REQUIRED_FIELD_KEY "REQUIRED"
