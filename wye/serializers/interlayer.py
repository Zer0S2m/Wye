from typing import (
    Type, Union, List
)

from wye.serializers import Serializer
from wye.types import JSON
import wye_serializers


def build_json(
    json: Union[JSON, List[JSON]],
    rules: Type[Serializer]
) -> Union[JSON, List[JSON]]:
    return wye_serializers.build_json(json, rules())
