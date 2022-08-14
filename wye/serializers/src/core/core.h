#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <stdio.h>
#include <stdlib.h>


int SetValidationError();
int SetAttributeError();
int SetValidationDefaultError();

int *CheckField(struct BuildFieldCheck build_field_check);
int *SetDefaultValue(struct Build obj_build, struct BuildFieldCheck build_field_check);
PyObject *BuildJson(struct Build obj_build);
static PyObject *method_build_json(PyObject *self, PyObject *args);

#define RULES_FIELD_KEY "rules"

#define TYPE_FIELD_KEY "TYPE"
#define ALIAS_FIELD_KEY "ALIAS"
#define DEFAULT_FIELD_KEY "DEFAULT"
#define REQUIRED_FIELD_KEY "REQUIRED"
