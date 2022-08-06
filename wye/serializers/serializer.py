from typing import (
	get_args, get_origin, Dict,
	Any, Union
)

from wye.serializers.fields import (
	ALIAS, REQUIRED
)


NoneType = type(None)


class BaseSerializer:
	def __init__(self) -> None:
		self._rules = self._build_rules()

	def _build_rules(self) -> Dict[str, Any]:
		rules = {}

		for param, type_ in self.__annotations__.items():
			rules[param] = self.__class__.__dict__[param]()
			if not rules[param][ALIAS]:
				rules[param][ALIAS] = param
			self.__set_required_field(rules[param], type_)

		return rules

	def __set_required_field(
		self,
		rules_one_field: Dict[str, Any],
		type_field: Any
	) -> None:
		if get_origin(type_field) is Union:
			for type_ in get_args(type_field):
				if type_ == NoneType:
					new_rule = {}
					new_rule[REQUIRED] = False
					rules_one_field.update(new_rule)

	def __call__(self) -> Dict[str, Any]:
		return self._rules


class Serializer(BaseSerializer):
	def __init__(self, *args, **kwargs) -> None:
		super().__init__(*args, **kwargs)
