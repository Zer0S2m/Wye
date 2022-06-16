from typing import (
	List, Optional
)
import inspect

from wye.routing import (
	Path, Router
)
from wye.types import (
	ASGIApp, ASGIInstance, Scope,
	Send, Receive
)
from wye.request import Request


def request_response(func):
	is_coroutine = inspect.iscoroutinefunction(func)

	def app(scope: Scope):
		async def awaitable(receive: Receive, send: Send):
			request = Request(scope = scope)
			kwargs = scope.get("kwargs", {})

			if is_coroutine:
				responce = await func(request, **kwargs)
			else:
				responce = func(request, **kwargs)

			await responce(receive, send)

		return awaitable

	return app


class Wye:
	def __init__(
		self
	) -> None:
		self.router = Router(routes = [])

	def mount(
		self,
		path: str,
		app: ASGIApp
	) -> None:
		for route in app.router.routes:
			route.path = f"{path}{route.path}"
			route.set_path_regex(path = route.path)

			self.router.routes.append(route)

	def add_route(
		self,
		path: str,
		route: ASGIApp,
		methods: Optional[List[str]] = None
	) -> None:
		if methods is None:
			methods = ["GET"]

		obj = Path(path, request_response(route), protocol = "http", methods = methods)
		self.router.routes.append(obj)

	def route(
		self,
		path: str
	):
		def decorator(func):
			self.add_route(path, func)

		return decorator

	def __call__(
		self,
		scope: Scope
	) -> ASGIInstance:
		return self.router(scope)
