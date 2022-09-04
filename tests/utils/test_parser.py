from wye.utils.parser import (
    parse_query_string, create_url
)


def test_parse_query_string():
    params = parse_query_string("param_1=value_1&param_2=value_2")

    assert params == [
        ("param_1", "value_1"),
        ("param_2", "value_2")
    ]


def test_create_url():
    url = create_url({
        "scheme": "http",
        "server": ("example.org", ""),
        "root_path": "",
        "path": "/example_path",
        "query_string": b"param_1=value_1"
    })
    assert url == "http://example.org/example_path?param_1=value_1"
