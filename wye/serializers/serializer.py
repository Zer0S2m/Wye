from typing import (
	Any, Dict
)

from wye.serializers.fields import ALIAS


class BaseSerializer:
	def __init__(self) -> None:
		self._build_rules()

	def _build_rules(self) -> Dict[str, Any]:
		rules = {}
		for param, type_ in self.__annotations__.items():
			rules[param] = self.__class__.__dict__[param]()
			if not rules[param][ALIAS]:
				rules[param][ALIAS] = param
		return rules

	def __call__(self) -> Any:
		return self._build_rules()


class Serializer(BaseSerializer):
	def __init__(self, *args, **kwargs) -> None:
		super().__init__(*args, **kwargs)
