from typing import (
	Dict, Any, List,
	Tuple, Union
)

from wye.serializers.fields import ALIAS
import wye_serializers


NoneType = type(None)
RULES = "RULES"


class MetaSerializer(type):
	def __new__(cls, name, bases, namespace):
		for class_ in bases:
			if issubclass(class_, BaseSerializer):
				annotations = namespace.get('__annotations__')
				if not annotations:
					break
				fields = {}
				for field, type_ in annotations.items():
					rules = namespace.get(field)()
					cls.__set_alias_rules(field, rules)
					fields.update({
						f"{field}": (rules, type_)
					})

				class_.__fields__.update({
					f"{name}": fields
				})

		return super().__new__(cls, name, bases, namespace)

	@classmethod
	def __set_alias_rules(
		cls,
		field: str,
		rules: Dict[str, Any]
	) -> None:
		if not rules[ALIAS]:
			rules.update({
				f"{ALIAS}": field
			})


class BaseSerializer(metaclass=MetaSerializer):
	__fields__ = {}

	def __init__(self) -> None:
		self._rules = self._build_rules()

	def is_validate(
		self,
		json: Union[Dict[str, Any], List[Dict[str, Any]]],
		alias: bool = True
	) -> Tuple[bool, Union[Dict[str, Any], List[Dict[str, Any]]]]:
		obj = wye_serializers.is_validate(json, self._rules)
		return obj

	def _build_rules(self) -> Dict[str, Any]:
		for serializer_name, fields in self.__fields__.items():
			for field, rules in fields.items():
				type_ = None
				if isinstance(rules, tuple):
					type_ = rules[1]
				elif isinstance(rules, dict):
					type_ = rules[f"{RULES}"][1]

				if issubclass(type_, BaseSerializer):
					self.__fields__[serializer_name][field] = {
						**self.__fields__[type_.__name__],
						f"{RULES}": (*rules,)
					}

		final_serializer = self.__class__.__name__
		return self.__fields__.get(final_serializer)

	def __call__(self) -> Dict[str, Any]:
		return self._rules


class Serializer(BaseSerializer):
	"""Main serailizer"""
