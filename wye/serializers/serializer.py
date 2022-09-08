from typing import (
    Dict, Any, List,
    Tuple, Union
)

from wye.serializers.fields import ALIAS
from wye.errors import (
    ErrorValidationJson, ErrorNotField
)
from wye.types import JSON
import wye_serializers


NoneType = type(None)
RULES = "RULES"


class MetaSerializer(type):
    def __new__(cls, name, bases, namespace):
        fields = {}

        for class_ in bases:
            if issubclass(class_, Serializer) and class_ != Serializer:
                fields.update(class_.__fields__)

            annotations = namespace.get('__annotations__')
            if not annotations:
                break
            for field, type_ in annotations.items():
                rules = namespace.get(field)()
                cls.__set_alias_rules(field, rules)
                fields.update({
                    f"{field}": (rules, type_)
                })

            class_.__fields__.update(fields)
            class_.__serializers__.update({
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


class Serializer(metaclass=MetaSerializer):
    """Main serailizer"""

    __serializers__ = {}
    __build_serializers__ = {}
    __fields__ = {}

    def __init__(
        self,
        is_validate: bool = False,
        **kwargs: JSON
    ) -> None:
        self._is_validate = is_validate
        self._rules = self._build_rules()
        self._set_values(kwargs)

    def is_validate(
        self,
        json: Union[JSON, List[JSON]]
    ) -> Tuple[bool, Union[JSON, List[JSON]]]:
        obj = wye_serializers.is_validate(json, self._rules)
        return obj

    @property
    def collected_json(self):
        if not hasattr(self, "_collected_json"):
            self._collected_json = wye_serializers.build_json(
                self._raw_json, self._rules
            )
        return self._collected_json

    @property
    def raw_json(self):
        return self._raw_json

    def _build_rules(self) -> JSON:
        for serializer_name, fields in self.__serializers__.items():
            for field, rules in fields.items():
                type_ = None
                if isinstance(rules, tuple):
                    type_ = rules[1]
                elif isinstance(rules, dict):
                    type_ = rules[RULES][1]

                if issubclass(type_, Serializer) and \
                   f"{field}__{serializer_name}" not in self.__build_serializers__:
                    self.__serializers__[serializer_name][field] = {
                        **self.__serializers__[type_.__name__],
                        f"{RULES}": (*rules,)
                    }
                    self.__build_serializers__[
                        f"{field}__{serializer_name}"
                    ] = True

        final_serializer = self.__class__.__name__
        return self.__serializers__.get(final_serializer)

    def _set_values(self, values: JSON):
        is_validate, _ = self.is_validate(values)
        if not is_validate and self._is_validate:
            raise ErrorValidationJson("Validation error")

        for param, value in values.items():
            if param not in self.__fields__:
                raise ErrorNotField(f"{param} - not field in scheme")
            setattr(self, param, value)

        self._raw_json = values

    def __call__(self) -> JSON:
        return self._rules
