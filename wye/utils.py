from typing import (
	Dict, Union
)


def parse_query_string(
	query_string: Union[bytes, str]
) -> Dict[str, Union[str, int]]:
	query_strinf = query_string.decode()

	if not query_string:
		return

	query_params = {}
	query_params_split = query_strinf.split(r"&")

	for query_param in query_params_split:
		query_param_split = query_param.split(r"=")

		try:
			if int(query_param_split[-1]):
				query_params[query_param_split[0]] = int(query_param_split[-1])
		except ValueError as e:
			query_params[query_param_split[0]] = query_param_split[-1]

	return query_params


def check_prefix_start(
	path: str
) -> bool:
	return path[0] == "/"


def check_prefix_end(
	path: str
) -> bool:
	return not path[-1] == "/"
