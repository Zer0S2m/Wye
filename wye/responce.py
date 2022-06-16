from typing import Any
import json

from wye.types import (
	Send, Receive
)


class Response:
	media_type = None
	charset = "utf-8"

	def __init__(
		self,
		content: Any,
		status_code: int = 200,
		media_type: str = None,
	) -> None:
		self.body = self.render(content)
		self.status_code = status_code
		self.media_type = media_type

	def render(
		self,
		content: Any
	) -> bytes:
		if isinstance(content, bytes):
			return content
		return content.encode(self.charset)

	async def __call__(
		self,
		receive: Receive,
		send: Send
	) -> None:
		await send({
			"type": "http.response.start",
			"status": self.status_code,
		})
		await send({
			"type": "http.response.body",
			"status": self.status_code,
			"body": self.body
		})


class HTMLResponse(Response):
    media_type = "text/html"


class PlainTextResponse(Response):
    media_type = "text/plain"


class JSONResponse(Response):
	media_type = "application/json"

	def render(
		self,
		content: dict
	) -> bytes:
		return json.dumps(content).encode(self.charset)
