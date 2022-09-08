class BaseError(Exception):
    def __init__(
        self,
        *args: object
    ) -> None:
        self.message = args[0]

    def __str__(self) -> str:
        return self.message


class ErrorPrefix(BaseError):
    """Not prefix in route"""


class ErrorSetParamState(BaseError):
    """Already exists key in state"""


class ErrorDisconnect(Exception):
    """Disconnect"""


class ErrorMaxLengthType(BaseError):
    """Invalid type parametr max_length"""


class ErrorMinLengthType(BaseError):
    """Invalid type parametr min_length"""


class ErrorMinLengthLargerMaxLength(BaseError):
    """min_length > max_length"""


class ErrorValidationJson(BaseError):
    """When methos is_validate return False"""


class ErrorNotField(BaseError):
    """Ton field"""
