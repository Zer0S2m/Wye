from wye.response import (
	FileResponse, PlainTextResponse
)
from wye.routing import (
	Router, Path
)
from wye.types import (
	ASGIApp, Scope, Send,
	Receive
)
from wye.request import Request


class StaticFiles:
	def __init__(
		self,
		directory: str
	) -> None:
		self.directory = directory
		self.router = Router(
			routes = [self.add_route("/", _StaticFileRoute)]
		)

	def add_route(
		self,
		path: str,
		app: ASGIApp
	) -> Path:
		self.set_directory_app(app)
		route = Path(path, app = app, methods = ("GET", "HEAD",), is_path_prefix = True)
		return route

	def set_directory_app(
		self,
		app: ASGIApp
	) -> None:
		app.directory = self.directory


class _StaticFileRoute:
	directory = None

	def __init__(
		self,
		scope: Scope
	) -> None:
		self.scope = scope

	@property
	def request(self) -> Request:
		if not hasattr(self, "_request"):
			self._request = Request(self.scope)
		return self._request

	async def __call__(
		self,
		receive: Receive,
		send: Send
	) -> None:
		if self.scope["method"] not in ("GET", "HEAD"):
			return PlainTextResponse("Method not allowed", status_code=406)

		url_split = self.request.url.path.split(r"/")
		file_name = url_split.pop(-1)

		response = FileResponse(
			file_name = file_name,
			path = "/".join(url_split)
		)
		await response(receive, send)
