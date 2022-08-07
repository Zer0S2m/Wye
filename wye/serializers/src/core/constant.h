int SetValidationError();
int SetAttributeError();
void *SetDefaultValue(PyObject *obj, PyObject *rules, PyObject *param_title);
int CheckField(PyObject *json_field, PyObject *type, PyObject *is_required);
int BuildJson(
    PyObject *obj, PyObject *json, PyObject *params_rules, PyObject *rules, int index_param_rule
);
static PyObject *method_build_json(PyObject *self, PyObject *args);

#define TYPE_FIELD_KEY "TYPE"
#define ALIAS_FIELD_KEY "ALIAS"
#define DEFAULT_FIELD_KEY "DEFAULT"
#define REQUIRED_FIELD_KEY "REQUIRED"
