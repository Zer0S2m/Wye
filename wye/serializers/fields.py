from typing import (
    Any, Dict, Optional,
    Callable, List, Union,
    Iterable
)

from wye.errors import (
    ErrorMaxLengthType, ErrorMinLengthType, ErrorMinLengthLargerMaxLength,
    ErrorFillType
)
from wye.types import FILL_TYPE


FILL_TYPES = (int, list, dict, float, set, frozenset, tuple, bool,)


ALIAS = "ALIAS"
TYPE = "TYPE"
DEFAULT = "DEFAULT"
REQUIRED = "REQUIRED"
IS_SERIALIZER = "IS_SERIALIZER"
VALIDATORS = "VALIDATORS"
MAX_LENGTH = "MAX_LENGTH"
MIN_LENGTH = "MIN_LENGTH"
TYPE_FILL_ = "TYPE_FILL"
TYPES_FILL = "TYPES_FILL"
GT = "GT"
GE = "GE"
LT = "LT"
LE = "LE"


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
        gt: Union[None, int, float] = None,
        ge: Union[None, int, float] = None,
        lt: Union[None, int, float] = None,
        le: Union[None, int, float] = None
    ) -> None:
        self._default = default
        self._alias = alias
        self._required = required
        self._validators = validators
        self._max_length = max_length
        self._min_length = min_length
        self._gt = gt
        self._ge = ge
        self._lt = lt
        self._le = le

        self._check_min_max_length()
        self._check_min_larger_max()

        self._rules = self._build_rules()

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
            f"{GT}": self._gt,
            f"{GE}": self._ge,
            f"{LT}": self._lt,
            f"{LE}": self._le
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
        return self._rules

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


class LIST(BaseField):
    __type__ = list

    def __init__(
        self,
        *args,
        fill_type: FILL_TYPE = None,
        **kwargs,
    ) -> None:
        super(LIST, self).__init__(*args, **kwargs)

        self._check_fill_type(fill_type)
        self._fill_type = fill_type

        self._rules.update({
            f"{TYPE_FILL_}": self._fill_type
        })

    @property
    def fill_type(self) -> Optional[Any]:
        return self._fill_type

    @fill_type.setter
    def fill_type(self, value: FILL_TYPE) -> None:
        self._check_fill_type(value)
        self._fill_type = value

    def _check_fill_type(self, fill_type: FILL_TYPE) -> None:
        if fill_type is not None and not issubclass(fill_type, FILL_TYPES):
            raise ErrorFillType(f"'{FILL_TYPES}' not containt '{fill_type}'")


class TUPLE(BaseField):
    __type__ = tuple

    def __init__(
        self,
        *args,
        fill_types: Optional[Iterable[FILL_TYPE]] = None,
        **kwargs,
    ) -> None:
        super(TUPLE, self).__init__(*args, **kwargs)

        self._check_fill_types(fill_types)
        self._fill_types = fill_types

        self._rules.update({
            f"{TYPES_FILL}": self._fill_types
        })

    @property
    def fill_types(self) -> Optional[Iterable[FILL_TYPE]]:
        return self._fill_types

    @fill_types.setter
    def fill_types(self, value: Optional[Iterable[FILL_TYPE]]) -> None:
        self._check_fill_types(value)
        self._fill_types = value

    def _check_fill_types(
        self,
        fill_types: Iterable[FILL_TYPE]
    ) -> None:
        if fill_types is not None or not fill_types:
            return

        for fill_type in fill_types:
            if not issubclass(fill_type, FILL_TYPES):
                raise ErrorFillType(f"'{fill_type}' not containt '{fill_type}'")


class SERIALIZER(BaseField):
    __type__ = dict
    __is_serializer__ = True
