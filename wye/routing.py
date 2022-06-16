from typing import (
	List, Tuple, Sequence
)
import re

from wye.types import (
	Scope, ASGIInstance, ASGIApp
)
from wye.responce import Response


class Route:
	def matches(
		self,
		scope: Scope
	) -> Tuple[bool, Scope]:
		raise NotImplementedError()

	def __call__(
		self,
		scope: Scope
	) -> ASGIInstance:
		raise NotImplementedError()


class Path(Route):
	def __init__(
		self,
		path: str,
		app: ASGIApp,
		methods: Sequence[str] = (),
		protocol: str = None,
	) -> None:
		self.path = path
		self.app = app
		self.protocol = protocol
		self.methods = methods
		regex = f"^{path}$"
		regex = re.sub("{([a-zA-Z_][a-zA-Z0-9_]*)}", r"(?P<\1>[^/]+)", regex)
		self.path_regex = re.compile(regex)

	def matches(
		self,
		scope: Scope
	) -> Tuple[bool, Scope]:
		if self.protocol is None or scope["type"] == self.protocol:
			match = self.path_regex.match(scope["path"])
			if match:
				kwargs = dict(scope.get("kwargs", {}))
				kwargs.update(match.groupdict())
				child_scope = dict(scope)
				child_scope["kwargs"] = kwargs
				return True, child_scope

		return False, {}

	def __call__(
		self,
		scope: Scope
	) -> ASGIInstance:
		if self.methods and scope["method"] not in self.methods:
			return Response("Method not allowed", 406, media_type="text/plain")

		return self.app(scope)


class Router:
	def __init__(
		self,
		routes: List[Route]
	) -> None:
		self.routes = routes

	def __call__(
		self,
		scope: Scope
	) -> ASGIInstance:
		for route in self.routes:
			matched, child = route.matches(scope)
			if matched:
				return route(child)

		return self.not_found(scope)

	def not_found(
		self,
		scope: Scope
	) -> ASGIInstance:
		return Response("Not found", 404, media_type = "text/plain")
