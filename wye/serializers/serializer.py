from typing import (
	get_args, get_origin, Dict,
	Any, Union, List,
	Tuple, Type, _GenericAlias
)

from wye.serializers.fields import (
	ALIAS, REQUIRED, ELEMENT_TYPE,
	EXPANDED_RULES, EXPANDED, ELEMENT_TYPES
)
import wye_serializers


NoneType = type(None)


class BaseListSetSerializer:
	def _build_rules_one(
		self,
		rule: Dict[str, Any],
		type_field: Type[_GenericAlias],
		type_origin: Union[list, set]
	) -> Dict[str, Any]:
		if get_origin(type_field) == type_origin and isinstance(type_field, _GenericAlias):
			rule.update({
				f"{EXPANDED}": True
			})
			rule[EXPANDED_RULES].update({
				f"{ELEMENT_TYPE}": get_args(type_field)[0]
			})


class BaseListSerializer(BaseListSetSerializer):
	def _build_rules_list(
		self,
		rule: Dict[str, Any],
		type_field: Type[_GenericAlias]
	) -> Dict[str, Any]:
		return self._build_rules_one(rule, type_field, list)


class BaseSetSerializers(BaseListSetSerializer):
	def _build_rules_set(
		self,
		rule: Dict[str, Any],
		type_field: Type[_GenericAlias]
	) -> Dict[str, Any]:
		return self._build_rules_one(rule, type_field, set)


class BaseTupleSerializers:
	def _build_rules_tuple(
		self,
		rule: Dict[str, Any],
		type_field: Type[_GenericAlias]
	) -> Dict[str, Any]:
		if get_origin(type_field) == tuple and isinstance(type_field, _GenericAlias):
			rule.update({
				f"{EXPANDED}": True
			})
			rule[EXPANDED_RULES].update({
				f"{ELEMENT_TYPES}": get_args(type_field)
			})


class BaseSerializer(
	BaseListSerializer,
	BaseSetSerializers,
	BaseTupleSerializers
):
	def __init__(self) -> None:
		self._rules = self._build_rules()

	def is_validate(
		self,
		json: Union[Dict[str, Any], List[Dict[str, Any]]],
		alias: bool = True
	) -> Tuple[bool, Union[Dict[str, Any], List[Dict[str, Any]]]]:
		rules = self._set_alias_rules(alias)
		data = wye_serializers.is_validate(json, rules)
		return data

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
			type_ = self.__set_required_field(rules[param], type_)
			self._build_rules_list(rules[param], type_)
			self._build_rules_set(rules[param], type_)
			self._build_rules_tuple(rules[param], type_)

		return rules

	def __set_required_field(
		self,
		rule: Dict[str, Any],
		type_field: Any
	) -> Type[_GenericAlias]:
		if get_origin(type_field) is Union:
			args = get_args(type_field)
			for type_ in args:
				if type_ == NoneType:
					new_rule = {}
					new_rule[REQUIRED] = False
					rule.update(new_rule)
					return args[0]
		return type_field

	def __call__(self) -> Dict[str, Any]:
		return self._rules


class Serializer(BaseSerializer):
	def __init__(self, *args, **kwargs) -> None:
		super().__init__(*args, **kwargs)
