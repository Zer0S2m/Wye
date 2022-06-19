from typing import (
	Tuple, Union
)
import os
import re

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


def create_path_join(
	*args: str
) -> str:
	reg = re.compile(r"^\\|/")
	parts = []
	for arg in args:
		parts.append(
			re.sub(reg, "", create_path_normpath(arg))
		)

	return os.path.join(*parts)


def create_path_normpath(
	path: str
) -> str:
	return os.path.normpath(path)
