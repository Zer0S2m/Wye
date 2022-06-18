from typing import Tuple


def set_header(
	key_header: str,
	value_header: str
) -> Tuple[bytes]:
	return (key_header.encode("utf-8"), value_header.encode("utf-8"))
