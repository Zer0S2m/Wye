import pathlib

from wye.utils.control import (
	check_prefix_start, check_prefix_end, check_isfile
)
from wye.utils.parser import create_path


def test_check_prefix_start():
	assert check_prefix_start("/path")
	assert check_prefix_start("path") == False


def test_check_prefix_end():
	assert check_prefix_end("/path")
	assert check_prefix_end("/path/") == False


def test_check_isfile():
	assert check_isfile("not_folder/not_file") == False
	assert check_isfile(create_path("README.md"))
