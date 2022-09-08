from wye import (
    Wye, Request, JSONResponse,
    WebSocket
)
from wye.serializers import (
    Serializer, build_json, build_json_from_object
)
from wye.serializers import fields


def validator(value):
    return value * 32


def validator_2(value):
    return value * 10


def validator_3(value):
    value["param_2"] = "asdasasd"
    return value


class Serializer1(Serializer):
    param_1: int = fields.INT(
        12,
        alias="param1",
        required=True,
        gt=60,
        le=100,
        # validators=[validator]
    )
    param_2: str = fields.STR("vvv", alias="param2", required=False, max_length=10)


class Serializer11(Serializer1):
    param_3: int = fields.INT(alias="param3", required=True)
    param_4: str = fields.STR(alias="param4", required=True)


class Serializer111(Serializer11):
    param_5: int = fields.INT(5, alias="param5", required=False)


class Serializer2(Serializer):
    param_1: int = fields.INT(alias="param1", required=False, validators=[validator])
    param_2: Serializer1 = fields.SERIALIZER(
        {"param_1": 1, "param_2": "string"},
        alias="param2",
        required=False,
        validators=[validator_3]
    )
    param_3: float = fields.FLOAT(alias="param3", required=False)


class Serializer3(Serializer):
    param_1: Serializer2 = fields.SERIALIZER(
        {"param_1": 1, "param_2": {"param_1": 1, "param_2": "string"}, "param_3": 1.2},
        alias="param1",
        required=False
    )
    param_2: str = fields.STR(alias="param2")
    param_3: int = fields.INT(15, required=False)
    param_4: Serializer1 = fields.SERIALIZER(
        {"param_1": 1, "param_2": "string 3"},
        required=False
    )


serializer = Serializer3()


class Data2:
    param_1 = 70
    param_2 = "asd"
    param_3 = 1
    param_4 = "value"
    param_5 = 20


serializer_111 = Serializer111(False, param_1=70, param_2="ASd", param_3=1,
                               param_4="asd", param_5=3)
print(serializer_111.collected_json)

print(build_json_from_object(Data2, serializer_111))
print(serializer_111.raw_json)
print(build_json({
    "param_1": 70,
    "param_2": "ASd",
    "param_3": 1,
    "param_4": "asd",
    "param_5": 1
}, serializer_111))
print(build_json({
    "param_1": 70,
    "param_2": "ASd",
    "param_3": 2,
    "param_4": "asd",
    "param_5": 2
}, serializer_111))

# print(serializer.is_validate({
#     "param_1": {
#         "param_1": 123,
#         "param_2": {
#             "param_1": 1,
#             "param_2": "value 1"
#         },
#         "param_3": 10.5
#     },
#     "param_2": "value",
#     "param_3": 12,
#     "param_4": {
#         "param_1": 1,
#         "param_2": "value 1"
#     },
# })[0])
# print(len(build_json([{
#     "param_1": {
#         "param_1": 2,
#         "param_2": {
#             "param_1": 2,
#             "param_2": "value 1"
#         },
#         "param_3": 10.5
#     },
#     "param_2": "value",
#     "param_3": 12,
#     "param_4": {
#         "param_1": 1,
#         "param_2": "value 1"
#     },
# }] * 10000, serializer)))
# print(build_json([{
#     "param_1": {
#         "param_1": 2,
#         "param_2": {
#             "param_1": 1,
#             "param_2": "value 1"
#         },
#         "param_3": 10.5
#     },
#     "param_2": "value",
#     "param_3": 12,
#     "param_4": {
#         "param_1": 1,
#         "param_2": "value 1"
#     },
# }] * 2, serializer))
# print(build_json({
#     "param_1": {
#         "param_1": 123,
#         "param_2": {
#             "param_1": 70,
#             "param_2": "value 1"
#         },
#         "param_3": 10.5
#     },
#     "param_2": "value",
#     "param_3": 12,
#     "param_4": {
#         "param_1": 70,
#         "param_2": "value 2"
#     },
# }, serializer))


app = Wye()


@app.route("/", response_class=JSONResponse)
async def home(request: Request):
    return build_json([{
        "param_1": {
            "param_1": 70,
            "param_2": {
                "param_1": 70,
                "param_2": "value 1"
            },
            "param_3": 10.5
        },
        "param_2": "value",
        "param_3": 12,
        "param_4": {
            "param_1": 70,
            "param_2": "value 2"
        },
    }] * 1000, serializer)


@app.websocket_route('/ws')
async def websocket_endpoint(session: WebSocket):
    await session.accept()
    await session.send_text('Hello, websocket!')
    await session.close()
