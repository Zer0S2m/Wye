from typing import (
	Any, Dict, Optional
)


ALIAS = "ALIAS"
TYPE = "TYPE"
DEFAULT = "DEFAULT"
REQUIRED = "REQUIRED"
EXPANDED = "EXPANDED"
EXPANDED_RULES = "EXPANDED_RULES"
EXPANDED_RULES_FOR = "FOR"
ELEMENT_TYPE = "ELEMENT_TYPE"
ELEMENT_TYPES = "ELEMENT_TYPES"
TYPE_KEY_DICT = "TYPE_KEY"
TYPE_VALUE_DICT = "TYPE_VALUE"

EXPANDED_RULES_LIST = "list"
EXPANDED_RULES_SET = "set"
EXPANDED_RULES_TUPLE = "tuple"
EXPANDED_RULES_DICT = "dict"
EXPANDED_RULES_FROZENSET = "frozenset"


class BaseField:
	__type__ = None

	def __init__(
		self,
		default: Any = None,
		alias: Optional[str] = None
	) -> None:
		self._default = default
		self._alias = alias
		self._required = True

	def _build_rules(self) -> Dict[str, Any]:
		rules = {}
		rules[TYPE] = (self.__type__,)
		rules[ALIAS] = self._alias
		rules[DEFAULT] = self._default
		rules[REQUIRED] = self._required
		rules[EXPANDED] = False

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


class LIST(BaseField):
	__type__ = list

	def _build_rules(self) -> Dict[str, Any]:
		rules = super()._build_rules()
		rules[EXPANDED] = False
		rules[EXPANDED_RULES] = {}
		rules[EXPANDED_RULES][EXPANDED_RULES_FOR] = EXPANDED_RULES_LIST

		return rules


class TUPLE(BaseField):
	__type__ = tuple

	def _build_rules(self) -> Dict[str, Any]:
		rules = super()._build_rules()
		rules[EXPANDED] = False
		rules[EXPANDED_RULES] = {}
		rules[EXPANDED_RULES][EXPANDED_RULES_FOR] = EXPANDED_RULES_TUPLE

		return rules


class SET(BaseField):
	__type__ = set

	def _build_rules(self) -> Dict[str, Any]:
		rules = super()._build_rules()
		rules[EXPANDED] = False
		rules[EXPANDED_RULES] = {}
		rules[EXPANDED_RULES][EXPANDED_RULES_FOR] = EXPANDED_RULES_SET

		return rules


class DICT(BaseField):
	__type__ = dict

	def _build_rules(self) -> Dict[str, Any]:
		rules = super()._build_rules()
		rules[EXPANDED] = False
		rules[EXPANDED_RULES] = {}
		rules[EXPANDED_RULES][EXPANDED_RULES_FOR] = EXPANDED_RULES_DICT

		return rules

class BYTES(BaseField):
	__type__ = bytes


class FROZENSET(BaseField):
	__type__ = frozenset

	def _build_rules(self) -> Dict[str, Any]:
		rules = super()._build_rules()
		rules[EXPANDED] = False
		rules[EXPANDED_RULES] = {}
		rules[EXPANDED_RULES][EXPANDED_RULES_FOR] = EXPANDED_RULES_FROZENSET

		return rules


class UNION(BaseField):
	__type__ = "union"
