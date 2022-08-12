#include <stdio.h>
#include <stdlib.h>


int SetValidationError();
int SetAttributeError();
int SetValidationDefaultError();
int *SetDefaultValue(PyObject *obj, PyObject *rule, PyObject *param_title);
int CheckFieldList(PyObject *json_field, PyObject *rule);
int CheckFieldSetFrozen(PyObject *json_field, PyObject *rule);
int CheckFieldTuple(PyObject *json_field, PyObject *rule);
int CheckFieldDict(PyObject *json_field, PyObject *rule);
int CheckField(PyObject *json_field, PyObject *rule);
int CheckExpandedField(PyObject *json_field, PyObject *rule);
int BuildJson(
    PyObject *obj, PyObject *json, PyObject *params_rules, PyObject *rules, int index_param_rule
);
static PyObject *method_build_json(PyObject *self, PyObject *args);
static PyObject *method_is_validate(PyObject *self, PyObject *args);

#define TYPE_FIELD_KEY "TYPE"
#define ALIAS_FIELD_KEY "ALIAS"
#define DEFAULT_FIELD_KEY "DEFAULT"
#define REQUIRED_FIELD_KEY "REQUIRED"
#define EXPANDED_FIELD_KEY "EXPANDED"
#define EXPANDED_RULES_FIELD_KEY "EXPANDED_RULES"
#define ARRAY_ELEMENT_TYPE_FIELD_KEY "ELEMENT_TYPE"
#define ARRAY_ELEMENT_TYPES_FIELD_KEY "ELEMENT_TYPES"
#define TYPE_KEY_DICT_FIELD_KEY "TYPE_KEY"
#define TYPE_VALUE_DICT_FIELD_KEY "TYPE_VALUE"

#define EXPANDED_RULES_LIST_FIELD_KEY "list"
#define EXPANDED_RULES_SET_FIELD_KEY "set"
#define EXPANDED_RULES_TUPLE_FIELD_KEY "tuple"
#define EXPANDED_RULES_FOR_FIELD_KEY "FOR"

#define GET_EXPANDED_RULE(rule) PyDict_GetItemString(rule, EXPANDED_RULES_FIELD_KEY)
