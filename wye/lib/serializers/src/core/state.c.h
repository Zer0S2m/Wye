/* State */

struct State;

void SetCurrentField(struct State *, PyObject *);
void ResetCurrentField(struct State *);
void SetWarningStr(struct State *);


struct State {
    PyObject *current_field;  // in python -> str
    PyObject *current_error; // in python -> str
};


/**
 * @brief Set the Current Field object
 *
 * @param state
 * @param field in python -> str
 */
void SetCurrentField(struct State *state, PyObject *field) {
    state->current_field = field;
}


/**
 * @brief
 *
 * @param state
 */
void ResetCurrentField(struct State *state) {
    state->current_field = NULL;
}


/**
 * @brief Set the Warning Str object
 *
 * @param state
 * @param field in python -> str
 */
void SetWarningStr(struct State *state) {
    state->current_error = PyUnicode_Concat(state->current_field, WARNING_REQUIRED_PARAM_STR);
}
