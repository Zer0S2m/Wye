from typing import (
	Any, Dict, Optional
)


class BaseField:
	__type__ = None

	def __init__(
		self,
		alias: Optional[str] = None
	) -> None:
		self.alias = alias

	def _build_rules(self) -> Dict[str, Any]:
		obj = {}
		obj["TYPE"] = self.__type__
		obj["ALIAS"] = self.alias

		return obj

	def __call__(self) -> Any:
		return self._build_rules()

	def __repr__(self) -> str:
		return f"Field <{self.__class__.__name__}> type:{self.__type__}"


class INT(BaseField):
	__type__ = int


class STR(BaseField):
	__type__ = str


class FLOAT(BaseField):
	__type__ = float
