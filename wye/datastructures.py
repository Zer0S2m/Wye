from typing import Union
import re


class QueryParams:
	pass


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


class Header:
	pass
