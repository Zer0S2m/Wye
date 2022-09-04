from collections.abc import Mapping
from enum import Enum
import json

from wye.types import (
    Scope, Receive, Send
)
from wye.datastructures import (
    URL, QueryParams, Headers
)
from wye.utils.parser import create_url


class WebSocketState(Enum):
    CONNECTING = 0
    CONNECTED = 1
    DISCONNECTED = 2


class WebSocket(Mapping):
    def __init__(
        self,
        scope: Scope,
        receive: Receive = None,
        send: Send = None
    ) -> None:
        assert scope["type"] == "websocket"
        self._scope = scope
        self._receive = receive
        self._send = send
        self.client_state = WebSocketState.CONNECTING
        self.application_state = WebSocketState.CONNECTING

    def __getitem__(self, key):
        return self._scope[key]

    def __iter__(self):
        return iter(self._scope)

    def __len__(self):
        return len(self._scope)

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

    async def receive(self):
        if self.client_state == WebSocketState.CONNECTING:
            message = await self._receive()
            message_type = message["type"]
            assert message_type == "websocket.connect"
            self.client_state = WebSocketState.CONNECTED
            return message
        elif self.client_state == WebSocketState.CONNECTED:
            message = await self._receive()
            message_type = message["type"]
            assert message_type in {"websocket.receive", "websocket.disconnect"}
            if message_type == "websocket.disconnect":
                self.client_state = WebSocketState.DISCONNECTED
            return message
        else:
            raise RuntimeError(
                'Cannot call "receive" once a disconnect message has been received.'
            )

    async def send(self, message):
        message_type = message["type"]
        if self.application_state == WebSocketState.CONNECTING:
            assert message_type in {"websocket.accept", "websocket.close"}
            if message_type == "websocket.close":
                self.application_state = WebSocketState.DISCONNECTED
            else:
                self.application_state = WebSocketState.CONNECTED
            await self._send(message)
        elif self.application_state == WebSocketState.CONNECTED:
            assert message_type in {"websocket.send", "websocket.close"}
            if message_type == "websocket.close":
                self.application_state = WebSocketState.DISCONNECTED
            await self._send(message)
        else:
            raise RuntimeError('Cannot call "send" once a close message has been sent.')

    async def accept(self, subprotocol=None):
        if self.client_state == WebSocketState.CONNECTING:
            await self.receive()
        await self.send({"type": "websocket.accept", "subprotocol": subprotocol})

    async def close(self, code=1000):
        await self.send({"type": "websocket.close", "code": code})

    async def send_text(self, data):
        await self.send({"type": "websocket.send", "text": data})

    async def send_bytes(self, data):
        await self.send({"type": "websocket.send", "bytes": data})

    async def send_json(self, data):
        encoded = json.dumps(data).encode("utf-8")
        await self.send({"type": "websocket.send", "bytes": encoded})
