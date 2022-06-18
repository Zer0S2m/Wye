from typing import (
	Any, List, Optional,
	Tuple, Union, Mapping,
	Iterator
)
import re

from wye.utils.parser import parse_query_string


class QueryParams(Mapping):
	def __init__(
		self,
		raw_params = None
	) -> None:
		self._raw_params = raw_params
		self._params = []

		self._set_query_params()

	def values(self) -> List[str]:
		return [value for key, value in self._params]

	def keys(self) -> List[str]:
		return [key for key, value in self._params]

	def get(
		self,
		key: str,
		default: Any = None
	):
		try:
			return self[key]
		except KeyError:
			return default

	def items(self) -> List[Tuple[str]]:
		return [
			(key, value)
			for key, value in self._params
		]

	def _set_query_params(self) -> None:
		self._params = parse_query_string(self._raw_params)

	def __getitem__(
		self,
		key: str
	) -> Optional[Tuple[str]]:
		for header in self._params:
			if header[0] == key:
				return header

		raise KeyError(key)

	def __iter__(self) -> Iterator:
		return iter(self.items())

	def __len__(self) -> int:
		return len(self._params)


class URL(str):
	def __init__(
		self,
		url: Union[str, bytes]
	) -> None:
		self._url = url if not isinstance(url, bytes) else url.decode()
		self._reg_full_path = re.compile(r"^https?://([\S][^>\?]+)[\?\#]*")

	def __str__(self) -> str:
		return self._url

	@property
	def scheme(self) -> str:
		return self._url.split(r"://")[0]

	@property
	def query_string(self) -> str:
		return self._url.split(r"?")[-1]

	@property
	def path(self) -> str:
		return f'/{re.match(self._reg_full_path, self._url).group(1).split(r"/", 1)[-1]}'

	@property
	def netloc(self) -> str:
		return re.match(self._reg_full_path, self._url).group(1).split(r"/", 1)[0]


class Headers(Mapping):
	def __init__(
		self,
		raw_headers = None
	) -> None:
		self._raw_headers = raw_headers
		self._headers = []
		self._set_headers()

	def values(self) -> List[str]:
		return [value for key, value in self._headers]

	def keys(self) -> List[str]:
		return [key for key, value in self._headers]

	def get(
		self,
		key: str,
		default: Any = None
	):
		try:
			return self[key]
		except KeyError:
			return default

	def items(self) -> List[Tuple[str]]:
		return [
			(key, value)
			for key, value in self._headers
		]

	def _set_headers(self) -> None:
		for raw_header in self._raw_headers:
			self._headers.append((raw_header[0].decode(), raw_header[1].decode()))

	def __repr__(self) -> str:
		return f"<{self.__class__.__name__}> {self.items()}"

	def __getitem__(
		self,
		key: str
	) -> Optional[Tuple[str]]:
		for header in self._headers:
			if header[0] == key:
				return header

		raise KeyError(key)

	def __iter__(self) -> Iterator:
		return iter(self.items())

	def __len__(self) -> int:
		return len(self._headers)
