class ErrorPrefix(Exception):
	def __init__(
		self,
		*args: object
	) -> None:
		self.message = args[0]

	def __str__(self) -> str:
		return self.message


class ErrorSetParamState(Exception):
	def __init__(
		self,
		*args: object
	) -> None:
		self.message = args[0]

	def __str__(self) -> str:
		return self.message
