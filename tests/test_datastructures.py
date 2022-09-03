from wye.datastructures import (
    Headers, QueryParams, URL
)


def test_url():
    url = URL("https://example.org/test_path?param_1=value_1&param_2=value_2")

    assert url.scheme == "https"
    assert url.query_string == "param_1=value_1&param_2=value_2"
    assert url.path == "/test_path"
    assert url.netloc == "example.org"


def test_headers():
    headers = Headers(raw_headers=[
        (b"header_1", b"valur_header_1"),
        (b"header_2", b"valur_header_2")
    ])

    assert headers.get("header_1") == ("header_1", "valur_header_1")
    assert headers.get("header_2") == ("header_2", "valur_header_2")
    assert headers.get("header_0") is None
    assert headers["header_1"] == ("header_1", "valur_header_1")
    assert headers["header_2"] == ("header_2", "valur_header_2")
    assert headers.values() == ["valur_header_1", "valur_header_2"]
    assert headers.keys() == ["header_1", "header_2"]
    assert headers.items() == [("header_1", "valur_header_1"),
                               ("header_2", "valur_header_2")]
    assert len(headers) == 2

    for header in headers:
        assert header


def test_query_params():
    params = QueryParams(raw_params=b"param_1=valur_param_1&param_2=valur_param_2")

    assert params.get("param_1") == ("param_1", "valur_param_1")
    assert params.get("param_2") == ("param_2", "valur_param_2")
    assert params.get("param_0") is None
    assert params["param_1"] == ("param_1", "valur_param_1")
    assert params["param_2"] == ("param_2", "valur_param_2")
    assert params.values() == ["valur_param_1", "valur_param_2"]
    assert params.keys() == ["param_1", "param_2"]
    assert params.items() == [("param_1", "valur_param_1"),
                              ("param_2", "valur_param_2")]
    assert len(params) == 2

    for param in params:
        assert param
