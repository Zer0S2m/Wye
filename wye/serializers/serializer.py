from typing import (
	get_args, get_origin, Dict,
	Any, Union, List,
	Tuple, Type, _GenericAlias
)

from wye.serializers.fields import (
	ALIAS, REQUIRED, ELEMENT_TYPE,
	EXPANDED_RULES
)
import wye_serializers


NoneType = type(None)


class BaseListSerializer:
	def _build_rules_list(
		self,
		rule: Dict[str, Any],
		type_field: Type[_GenericAlias]
	) -> Dict[str, Any]:
		if isinstance(get_origin(type_field), list):
			rule[EXPANDED_RULES].update({
				f"{ELEMENT_TYPE}": get_args(type_field)[0]
			})


class BaseSerializer(BaseListSerializer):
	def __init__(self) -> None:
		self._rules = self._build_rules()

	def is_validate(
		self,
		json: Union[Dict[str, Any], List[Dict[str, Any]]],
		alias: bool = True
	) -> Tuple[bool, Union[Dict[str, Any], List[Dict[str, Any]]]]:
		rules = self._set_alias_rules(alias)
		is_valid, obj = wye_serializers.is_validate(json, rules)
		return (is_valid, obj)

	def _set_alias_rules(
		self,
		alias: bool
	) -> Dict[str, Any]:
		if not alias:
			return self._rules

		obj = {}
		for _, rule in self._rules.items():
			obj[rule[ALIAS]] = rule
		return obj

	def _build_rules(self) -> Dict[str, Any]:
		rules = {}

		for param, type_ in self.__annotations__.items():
			rules[param] = self.__class__.__dict__[param]()
			if not rules[param][ALIAS]:
				rules[param][ALIAS] = param
			self.__set_required_field(rules[param], type_)
			self._build_rules_list(rules[param], type_)

		return rules

	def __set_required_field(
		self,
		rule: Dict[str, Any],
		type_field: Any
	) -> None:
		if get_origin(type_field) is Union:
			for type_ in get_args(type_field):
				if type_ == NoneType:
					new_rule = {}
					new_rule[REQUIRED] = False
					rule.update(new_rule)

	def __call__(self) -> Dict[str, Any]:
		return self._rules


class Serializer(BaseSerializer):
	def __init__(self, *args, **kwargs) -> None:
		super().__init__(*args, **kwargs)
