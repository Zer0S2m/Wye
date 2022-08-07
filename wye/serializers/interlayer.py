from typing import (
	Any, Dict, Type,
	Union, List
)

from wye.serializers import BaseSerializer

import wye_serializers


def build_json(
	json: Union[Dict[str, Any], List[Dict[str, Any]]],
	rules: Type[BaseSerializer]
) -> Union[Dict[str, Any], List[Dict[str, Any]]]:
	return wye_serializers.build_json(json, rules())
