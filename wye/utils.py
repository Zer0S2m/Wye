from typing import (
	Tuple, Union
)

from wye.types import Scope


def parse_query_string(
	query_string: Union[bytes, str]
) -> Tuple[str, int]:
	query_strinf = query_string.decode()

	if not query_string:
		return

	query_params = []
	query_params_split = query_strinf.split(r"&")

	for query_param in query_params_split:
		query_param_split = query_param.split(r"=")

		try:
			if int(query_param_split[-1]):
				query_params.append((query_param_split[0], int(query_param_split[-1])))
		except ValueError as e:
			query_params.append((query_param_split[0], query_param_split[-1]))

	return query_params


def create_url(
	scope: Scope
) -> str:
	scheme = scope["scheme"]
	host, port = scope["server"]
	path = scope.get("root_path", "") + scope["path"]
	query_string = scope["query_string"].decode()

	if port:
		url = f"{scheme}://{host}:{port}{path}"
	else:
		url = f"{scheme}://{host}{path}"

	if query_string:
		url += f"?{query_string}"

	return url


def check_prefix_start(
	path: str
) -> bool:
	return path[0] == "/"


def check_prefix_end(
	path: str
) -> bool:
	return not path[-1] == "/"
