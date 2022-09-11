from typing import (
    Mapping, Any, Callable,
    Awaitable, Dict, Union
)


Scope = Mapping[str, Any]
Message = Mapping[str, Any]

Receive = Callable[[], Awaitable[Message]]
Send = Callable[[Message], Awaitable[None]]

ASGIInstance = Callable[[Receive, Send], Awaitable[None]]
ASGIApp = Callable[[Scope], ASGIInstance]

JSON = Dict[str, Any]
FILL_TYPE = Union[int, list, dict, float, set, frozenset, tuple, bool]
