from wye.types import Scope
from wye.utils import parse_query_string
from wye.datastructures import URL


class Request:
	def __init__(
		self,
		scope: Scope
	) -> None:
		self._scope = scope

	@property
	def method(self) -> str:
		return self._scope["method"]

	@property
	def url(self):
		if not hasattr(self, "_url"):
			scheme = self._scope["scheme"]
			host, port = self._scope["server"]
			path = self._scope.get("root_path", "") + self._scope["path"]
			query_string = self._scope["query_string"].decode()

			if port:
				url = f"{scheme}://{host}:{port}{path}"
			else:
				url = f"{scheme}://{host}{path}"

			if query_string:
				url += f"?{query_string}"

			self._url = URL(url)

		return self._url

	@property
	def query_params(self) -> None:
		return parse_query_string(self._scope["query_string"])
