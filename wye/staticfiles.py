from wye.response import (
    FileResponse, PlainTextResponse
)
from wye.routing import (
    Router, Path
)
from wye.types import (
    ASGIApp, Scope, Send,
    Receive, ASGIInstance
)
from wye.request import Request


class StaticFiles:
    def __init__(
        self,
        directory: str
    ) -> None:
        self.directory = directory
        self.router = Router(
            routes=[self.add_route("/", _StaticFileRoute)]
        )

    def add_route(
        self,
        path: str,
        app: ASGIApp
    ) -> Path:
        self.set_directory_app(app)
        route = Path(path, app=app, methods=("GET", "HEAD",),
                     is_path_prefix=True)
        return route

    def set_directory_app(
        self,
        app: ASGIApp
    ) -> None:
        app.directory = self.directory


class _StaticFileRoute:
    directory = None
    methods = ("GET", "HEAD")

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

    def not_fount(
        self,
        scope: Scope
    ) -> ASGIInstance:
        return PlainTextResponse("Not file", status_code=404)

    async def __call__(
        self,
        receive: Receive,
        send: Send
    ) -> None:
        if self.scope["method"] not in self.methods:
            response = PlainTextResponse("Method not allowed", status_code=406)
            await response(receive, send)

        url_split = self.request.url.path.split(r"/")
        file_name = url_split.pop(-1)

        try:
            response = FileResponse(
                file_name=file_name,
                path="/".join(url_split)
            )
            await response(receive, send)
        except RuntimeError:
            response = self.not_fount(self.scope)
            await response(receive, send)
