import os

from .parser import create_path_normpath


def check_prefix_start(
	path: str
) -> bool:
	return path[0] == "/"


def check_prefix_end(
	path: str
) -> bool:
	return not path[-1] == "/"


def check_isfile(
	*args: str
) -> bool:
	path = create_path_normpath(*args)
	if os.path.isfile(path):
		True

	return False
