from typing import Any

from wye.errors import ErrorSetParamState


class State:
    def __init__(self) -> None:
        self._dict = {}

    def get(
        self,
        key: str,
        default: Any = None
    ) -> Any:
        try:
            return self[key]
        except KeyError:
            return default

    def set(
        self,
        key: str,
        value: Any
    ) -> None:
        if key in self._dict:
            raise ErrorSetParamState(
                f"the key: {key} -already exists,"
                " call the 'update' method to update this setting"
            )

        self[key] = value

    def update(
        self,
        key: str,
        value: Any
    ) -> Any:
        if key not in self._dict:
            raise KeyError(key)

        self._dict[key] = value
        return self[key]

    def _set_config_in_state(
        self,
        data: dict
    ) -> None:
        self._dict.update(data)

    def __getitem__(
        self,
        key: str
    ) -> Any:
        if self._dict[key]:
            return self._dict[key]

        raise KeyError(key)

    def __setitem__(
        self,
        key: str,
        value: Any
    ) -> None:
        self._dict.setdefault(key, value)
