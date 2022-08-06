from typing import (
	Any, Dict, Type
)

from wye.serializers import BaseSerializer

import wye_serializers


def build_json(
	json: Dict[str, Any],
	rules: Type[BaseSerializer]
) -> Dict[str, Any]:
	return wye_serializers.build_json(json, rules())
