from wye.serializers import Serializer, fields, build_json


class Serializer1(Serializer):
    field_1: int = fields.INT(required=False)
    field_2: str = fields.STR(alias="field2")


class Serializer2(Serializer):
    field_1: int = fields.INT(required=True)
    field_2: Serializer1 = fields.SERIALIZER()
    field_3: int = fields.INT(required=True)


class Serializer3(Serializer):
    field_1: Serializer2 = fields.SERIALIZER()
    field_2: str = fields.STR()
    field_3: Serializer1 = fields.SERIALIZER()


data_1 = [
    {
        "field_1": 1,
        "field_2": "asd"
    }
] * 100000

data_2 = [
    {
        "field_1": 1,
        "field_2": {
            "field_1": 1,
            "field_2": "asd"
        },
        "field_3": 1
    }
] * 100000

data_3 = [
    {
        'field_1': {
            "field_1": 1,
            "field_2": {
                "field_1": 1,
                # "field_2": "asd"
            },
            "field_3": 1
        },
        'field_2': 'asd',
        'field_3': {
            "field_1": 1,
            "field_2": "asd"
        }
    }
] * 10000


build_json(data_1, Serializer1())
build_json(data_2, Serializer2())
build_json(data_3, Serializer3())
