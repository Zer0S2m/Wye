from typing import Dict, Any

import wye_temp_engine


def build_template(
    path_temp: str,
    context: Dict[str, Any]
) -> Any:
    return wye_temp_engine.build_template(path_temp, context)
