from typing import (
    Mapping, Any, Callable,
    Awaitable
)


Scope = Mapping[str, Any]
Message = Mapping[str, Any]

Receive = Callable[[], Awaitable[Message]]
Send = Callable[[Message], Awaitable[None]]

ASGIInstance = Callable[[Receive, Send], Awaitable[None]]
ASGIApp = Callable[[Scope], ASGIInstance]
