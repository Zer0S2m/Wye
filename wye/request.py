from wye.types import Scope
from wye.utils import (
	parse_query_string, create_url
)
from wye.datastructures import (
	URL, Headers
)


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
	def url(self) -> URL:
		if not hasattr(self, "_url"):
			self._url = URL(create_url(self._scope))

		return self._url

	@property
	def query_params(self) -> None:
		return parse_query_string(self._scope["query_string"])

	@property
	def headers(self) -> Headers:
		if not hasattr(self, "_headers"):
			self._headers = Headers(self._scope["headers"])

		return self._headers
