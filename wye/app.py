from typing import (
	List, Optional, Type,
	Union
)
import inspect

from wye.routing import (
	Path, Router
)
from wye.response import Response
from wye.types import (
	ASGIApp, ASGIInstance, Scope,
	Send, Receive
)
from wye.request import Request
from wye.utils.control import (
	check_prefix_start, check_prefix_end
)
from wye.errors import ErrorPrefix
from wye.state import State


def request_response(
	func,
	response_class: Union[Type[Response], None] = None
):
	is_coroutine = inspect.iscoroutinefunction(func)

	def app(scope: Scope):
		async def awaitable(receive: Receive, send: Send):
			request = Request(scope = scope, receive = receive)
			kwargs = scope.get("kwargs", {})

			if is_coroutine:
				response = await func(request, **kwargs)
			else:
				response = func(request, **kwargs)

			if response_class:
				response = response_class(response)

			await response(receive, send)

		return awaitable

	return app


class Wye:
	def __init__(
		self
	) -> None:
		self.router = Router(routes = [])
		self.state = State()

	def mount(
		self,
		path: str,
		app: ASGIApp
	) -> None:
		if not check_prefix_start(path):
			raise ErrorPrefix("Invalid prefix at the beginning of the path")
		if not check_prefix_end(path):
			raise ErrorPrefix("Incorrect path ending")

		for route in app.router.routes:
			route.path = f"{path}{route.path}"
			route.set_path_regex(path = route.path)

			self.router.routes.append(route)

	def add_route(
		self,
		path: str,
		route: ASGIApp,
		methods: Optional[List[str]] = None,
		response_class: Union[Type[Response], None] = None
	) -> None:
		if methods is None:
			methods = ["GET"]

		obj = Path(
			path = path,
			app = request_response(
				func = route,
				response_class = response_class
			),
			protocol = "http",
			methods = methods
		)
		self.router.routes.append(obj)

	def route(
		self,
		path: str,
		*,
		response_class: Union[Type[Response], None] = None
	):
		def decorator(func):
			self.add_route(
				path = path,
				route = func,
				response_class = response_class
			)

		return decorator

	def __call__(
		self,
		scope: Scope
	) -> ASGIInstance:
		return self.router(scope)
