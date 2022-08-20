from typing import (
	Any, Dict, Optional
)


ALIAS = "ALIAS"
TYPE = "TYPE"
DEFAULT = "DEFAULT"
REQUIRED = "REQUIRED"
IS_SERIALIZER = "IS_SERIALIZER"


class BaseField:
	__type__ = None
	__is_serializer__ = False

	def __init__(
		self,
		default: Any = None,
		*,
		alias: Optional[str] = None,
		required: bool = True
	) -> None:
		self._default = default
		self._alias = alias
		self._required = required

	def _build_rules(self) -> Dict[str, Any]:
		rules = {}
		rules[TYPE] = (self.__type__,)
		rules[ALIAS] = self._alias
		rules[DEFAULT] = self._default
		rules[REQUIRED] = self._required
		rules[IS_SERIALIZER] = self.__is_serializer__

		return rules

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
