import json

from wye.types import (
    Scope, Receive
)
from wye.utils.parser import create_url
from wye.datastructures import (
    URL, Headers, QueryParams
)
from wye.errors import ErrorDisconnect


class Request:
    def __init__(
        self,
        scope: Scope,
        receive: Receive
    ) -> None:
        self._scope = scope
        self._receive = receive
        self._body = b""

    @property
    def method(self) -> str:
        return self._scope["method"]

    @property
    def url(self) -> URL:
        if not hasattr(self, "_url"):
            self._url = URL(create_url(self._scope))
        return self._url

    @property
    def query_params(self) -> QueryParams:
        if not hasattr(self, "_query_params"):
            self._query_params = QueryParams(self._scope["query_string"])
        return self._query_params

    @property
    def headers(self) -> Headers:
        if not hasattr(self, "_headers"):
            self._headers = Headers(self._scope["headers"])
        return self._headers

    async def stream(self):
        while True:
            message = await self._receive()
            if message["type"] == "http.request":
                yield message.get("body", b"")
                if not message.get("more_body", False):
                    break
            elif message["type"] == "http.disconnect":
                raise ErrorDisconnect()

    async def body(self):
        if not getattr(self, "_body"):
            async for chunk in self.stream():
                self._body += chunk
        return self._body

    async def json(self):
        return json.loads(await self.body())
