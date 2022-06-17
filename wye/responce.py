from typing import Any, Optional
from mimetypes import guess_type
import json

from wye.types import (
	Send, Receive
)
from wye.datastructures import AsyncFile


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


class FileResponse(Response):
	chunk_size = 4096

	def __init__(
		self,
		file_name: str,
		path: Optional[str] = None,
		media_type: Optional[str] = None
	) -> None:
		self.file_name = file_name
		self.path = path
		self.status_code = 200

		if not media_type:
			self.media_type = guess_type(self.file_name or self.path)[0] or "text/plain"
		else:
			self.media_type = media_type

		self._set_headers()

	def _set_headers(
		self
	) -> None:
		self.headers = [
			("media_type".encode("latin-1"), self.media_type.encode("latin-1")),
			("content-disposition".encode("latin-1"), f'attachment; filename="{self.file_name}"'.encode("latin-1"))
		]

	async def __call__(
		self,
		receive: Receive,
		send: Send
	) -> None:
		await send({
			"type": "http.response.start",
			"status": self.status_code,
			"headers": self.headers
		})

		async with AsyncFile(self.file_name, open_flag = "rb") as file:
			async for body in file.read_by_chunk(self.chunk_size):
				more_body = len(body) == self.chunk_size

				await send({
					"type": "http.response.body",
					"more_body": more_body,
					"body": body
				})
