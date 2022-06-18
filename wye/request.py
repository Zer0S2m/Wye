from wye.types import Scope
from wye.utils.parser import create_url
from wye.datastructures import (
	URL, Headers, QueryParams
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
	def query_params(self) -> QueryParams:
		if not hasattr(self, "_query_params"):
			self._query_params = QueryParams(self._scope["query_string"])

		return self._query_params

	@property
	def headers(self) -> Headers:
		if not hasattr(self, "_headers"):
			self._headers = Headers(self._scope["headers"])

		return self._headers
