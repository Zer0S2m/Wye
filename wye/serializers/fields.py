from typing import (
    Any, Dict, Optional,
    Callable, List
)

from wye.errors import (
    ErrorMaxLengthType, ErrorMinLengthType, ErrorMinLengthLargerMaxLength
)


ALIAS = "ALIAS"
TYPE = "TYPE"
DEFAULT = "DEFAULT"
REQUIRED = "REQUIRED"
IS_SERIALIZER = "IS_SERIALIZER"
VALIDATORS = "VALIDATORS"
MAX_LENGTH = "MAX_LENGTH"
MIN_LENGTH = "MIN_LENGTH"


class BaseField:
    __type__ = None
    __is_serializer__ = False

    def __init__(
        self,
        default: Any = None,
        *,
        required: bool = True,
        alias: Optional[str] = None,
        validators: List[Callable[[Any], Any]] = [],
        max_length: Optional[int] = None,
        min_length: Optional[int] = None,
    ) -> None:
        self._default = default
        self._alias = alias
        self._required = required
        self._validators = validators
        self._max_length = max_length
        self._min_length = min_length

        self._check_min_max_length()
        self._check_min_larger_max()

    def _build_rules(self) -> Dict[str, Any]:
        rules = {
            f"{TYPE}": (self.__type__,),
            f"{ALIAS}": self._alias,
            f"{DEFAULT}": self._default,
            f"{REQUIRED}": self._required,
            f"{IS_SERIALIZER}": self.__is_serializer__,
            f"{VALIDATORS}": self._validators,
            f"{MAX_LENGTH}": self._max_length,
            f"{MIN_LENGTH}": self._min_length,
        }
        return rules

    def _check_min_max_length(self) -> None:
        if not isinstance(self._max_length, (type(None), int)):
            raise ErrorMaxLengthType("Invalid type for 'max_length' parameter")
        if not isinstance(self._min_length, (type(None), int)):
            raise ErrorMinLengthType("Invalid type for 'min_length' parameter")

    def _check_min_larger_max(self) -> None:
        if isinstance(self._min_length, int) and isinstance(self._max_length, int):
            if self._min_length > self._max_length:
                raise ErrorMinLengthLargerMaxLength(
                    "parameter 'min_length' is greater than 'max_length'"
                )

    @property
    def default(self) -> Any:
        return self._default

    @property
    def alias(self) -> str:
        return self._alias

    @alias.setter
    def alias(self, value: str) -> None:
        if not value:
            raise ValueError("No 'alias'")
        self._alias = value

    @property
    def required(self) -> bool:
        return self._required

    @required.setter
    def required(self, value: bool):
        self._required = value

    def __call__(self) -> Dict[str, Any]:
        return self._build_rules()

    def __repr__(self) -> str:
        return f"Field <{self.__class__.__name__}> type:{self.__type__}"


class INT(BaseField):
    __type__ = int


class STR(BaseField):
    __type__ = str


class FLOAT(BaseField):
    __type__ = float


class BOOL(BaseField):
    __type__ = bool


class BYTES(BaseField):
    __type__ = bytes


class SERIALIZER(BaseField):
    __type__ = dict
    __is_serializer__ = True
