from typing import Any, Optional
from mimetypes import guess_type
from email.utils import formatdate
import json
import os

from wye.types import (
	Send, Receive
)
from wye.utils.aiofile import AsyncFile
from wye.utils.sets import set_header
from wye.utils.control import check_isfile
from wye.utils.parser import create_path


class Response:
	media_type = None
	charset = "utf-8"

	def __init__(
		self,
		content: Any,
		status_code: int = 200,
		media_type: Optional[str] = None,
	) -> None:
		self.body = self.render(content)
		self.status_code = status_code

		if media_type is not None:
			self.media_type = media_type

		self.init_headers()

	def render(
		self,
		content: Any
	) -> bytes:
		if isinstance(content, bytes):
			return content
		return content.encode(self.charset)

	def init_headers(self) -> None:
		self.headers = []

		if hasattr(self, "body") and self.body:
			self.headers.append(set_header("content-length", f"{len(self.body)}"))
		if hasattr(self, "media_type") and self.media_type:
			content_type = self.media_type
			if content_type.startswith("text/"):
				content_type += f"; charset={self.charset.upper()}"

			self.headers.append(set_header("content-type", content_type))

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
		return json.dumps(content).encode("utf-8")


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
		if media_type is None:
			self.media_type = guess_type(self.file_name or self.path)[0] or "text/plain"
		else:
			self.media_type = media_type

		self.set_path()
		self.init_headers()
		self.set_stat_headers()

	def set_stat_headers(self) -> None:
		stat = os.stat(self.path)

		self.headers.append(set_header("content-disposition", f'attachment; filename="{self.file_name}"'))
		self.headers.append(set_header("content-length", f"{stat.st_size}"))
		self.headers.append(set_header("last-modified", formatdate(stat.st_ctime)))

	def set_path(self) -> None:
		parts_path = list(self.path.split(r"/"))
		if self.path:
			path = create_path(*parts_path, self.file_name)
		else:
			path = create_path(self.file_name)

		self.path = path
		self.check_path()

	def check_path(self) -> None:
		if not check_isfile(self.path):
			raise RuntimeError(f"StaticFile at path '{self.path}' does not exist.")

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
		async with AsyncFile(self.path, open_flag = "rb") as file:
			async for body in file.read_by_chunk(self.chunk_size):
				more_body = len(body) == self.chunk_size

				await send({
					"type": "http.response.body",
					"more_body": more_body,
					"body": body
				})
