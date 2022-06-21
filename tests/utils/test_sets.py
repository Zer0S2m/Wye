from wye.utils.sets import set_header


def test_set_header():
	assert (b"header_1", b"value_1") == set_header("header_1", "value_1")