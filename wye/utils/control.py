def check_prefix_start(
	path: str
) -> bool:
	return path[0] == "/"


def check_prefix_end(
	path: str
) -> bool:
	return not path[-1] == "/"
