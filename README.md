# Wye

Wye — это облегченная библиотека [ASGI](https://asgi.readthedocs.io/en/latest/).

## Requirements

Вы должны установить сервер ASGI: [uvicorn](http://www.uvicorn.org/), [daphne](https://github.com/django/daphne/), или [hypercorn](https://pgjones.gitlab.io/hypercorn/).

```bash
pip install uvicorn
```

## Responses

### Response

Принимает какой-то текст или байты и возвращает в формате который вы укажите
в аргументе `media_type`

```python
from wye import Response


class App:
    def __init__(self, scope) -> None:
        self.scope = scope

    async def __call__(self, receive, send):
        response = Response('Hello, world!', media_type='text/plain')
        await response(receive, send)
```

### HTMLResponse

Принимает какой-то текст или байты и возвращает данные в формате `text/html`

```python
from wye import HTMLResponse


class App:
    def __init__(self, scope) -> None:
        self.scope = scope

    async def __call__(self, receive, send):
        response = HTMLResponse('<html><body><h1>Hello, world!</h1></body></html>')
        await response(receive, send)
```

### PlainTextResponse

Принимает какой-то текст или байты и возвращает данные в формате `text/plain`

```python
from wye import PlainTextResponse


class App:
    def __init__(self, scope) -> None:
        self.scope = scope

    async def __call__(self, receive, send):
        response = PlainTextResponse('Hello, world!')
        await response(receive, send)
```

### JSONResponse

Принимает какой-то словарь (`Dict`) и возвращает данные в формате `application/json`

```python
from wye import JSONResponse


class App:
    def __init__(self, scope) -> None:
        self.scope = scope

    async def __call__(self, receive, send):
        response = JSONResponse({"data": "Hello, world!"})
        await response(receive, send)
```

### StreamingResponse

Принимает какой-то генератор и возвращает данные в формате который вы укажите

```python
from wye import StreamingResponse


async def generator():
    for num in range(1, 11):
        yield str(num)


class App:
    def __init__(self, scope) -> None:
        self.scope = scope

    async def __call__(self, receive, send):
        response = StreamingResponse(generator, media_type = "text/plain")
        await response(receive, send)
```

### FileResponse

Принимает название файла и возвращает файл

```python
from wye import FileResponse


class App:
    def __init__(self, scope) -> None:
        self.scope = scope

    async def __call__(self, receive, send):
        response = FileResponse("example_file.txt", path = "folder/example")
        await response(receive, send)
```

### WebSocket

WebSocket, который выполняет ту же роль, что и HTTP-запрос, но позволяет отправлять и
получать данные в сеансе веб-сокета.

```python
from starlette import WebSocket


class App:
    def __init__(self, scope):
        self.scope = scope

    async def __call__(self, receive, send):
        session = WebSocket(self.scope, receive=receive, send=send)
        await session.accept()
        await session.send_text('Hello, socket!')
        await session.close()
```

#### Принятие соединения
`await session.accept(subprotocol=None)`

#### Отправка данных
`await session.send_text(data)`
`await session.send_bytes(data)`
`await session.send_json(data)`

#### Получение данных
`await session.receive_text()`
`await session.receive_bytes()`
`await session.receive_json()`

#### Закрытие соединения
`await session.close(code=1000)`

## Routing

### Router

```python
from wye import (
    Router, Path, PlainTextResponse
)


class Home:
    def __init__(self, scope) -> None:
        self.scope = scope

    async def __call__(self, recieve, send):
        response = PlainTextResponse(content = "Home")
        await response(recieve, send)


class About:
    def __init__(self, scope) -> None:
        self.scope = scope

    async def __call__(self, recieve, send):
        response = PlainTextResponse(content = "About")
        await response(recieve, send)


app = Router([
    Path(path = "/home", app = Home),
    Path(path = "/about", app = About)
])
```

## Application

### Wye

```python
from wye import (
    Wye, PlainTextResponse, FileResponse
)


app = Wye()


@app.route("/")
async def home(request):
    return PlainTextResponse("home")


@app.route(
    "/test",
    response_class = PlainTextResponse
)
async def test(request):
    return "home"


@app.route("/about")
async def about(request):
    return FileResponse("about.pdf")
```

#### Methods

1) `.add_route(path, func, methods=["GET"])` - добавить маршрут
Функция должна принимать параметр `request` и возвращать `Response`, *example*:
`func(request, **kwargs) -> response`

2) `.route(path, *, response_class)` - Добавить маршрут
Функция должна принимать параметр `request` и возвращать `Optional[Response]`, *example*:
`func(request, **kwargs) -> Optional[response]`
Формат `декоратор`

    Аргументы:
    - `path: str` - путь
    - `response_class: Type[Response]` - класс ответа

3) `add_websocket_route(path, route)` - Добавить маршрут сеанса веб-сокета. Функция должна быть
сопрограмой. Сигнатура: `func(session, **kwargs)`

4) `.websocket_route(path)` - Добавить маршрут WebSocket, формат декоратора.

5) `.mount(path)` - Монтировать приложение `Wye`

#### Args

1) `config` *(`str`)* - название файла конфигурации *(будет искать в корневом каталоге вашего проекта)*
Все данные прочитанные из файла будут записываться в глобальный стейт приложения `Wye`

Пример:

```
📦 main_app
 ┣ 📜 app.py
 ┗ 📜 config.ini
```

`📜 config.ini`:
```ini
[example]
example_1 = 10
example_2 = False
example_3 = "example"
example_4 = 3.14

[default]
example_1 = ["example", "example", "example"]
```

`📜 app.py`:
```python
from wye import Wye


app = Wye(config = "config.ini")
app.state["example"]["example_1"]
```

### StaticFiles
```python
from wye import (
    Wye, PlainTextResponse, StaticFiles
)


app = Wye()

app.mount("/static", app = StaticFiles(directory = "static"))
app.mount("/example", app = StaticFiles(directory = "example"))


@app.route("/")
async def home(request):
    return PlainTextResponse("home")
```

Разбор следующей файловой структуры:
```
📦 main_app
 ┣ 📜 __init__.py
 ┣ 📜 app.py
 ┣ 📂 app_search
 ┃ ┃ ┣ 📜 __init__.py
 ┃ ┃ ┣ 📜 app.py
 ┃ ┃ ┗ ...
 ┃ ┗ ...
 ┗ ...
```

файл `📦 main_app > 📂 app_search > 📜 app.py`
```python
from wye import (
    Wye, StaticFiles
)
app = Wye()
app.mount("/static", StaticFiles("static"))
```

файл `📦 main_app > 📜 app.py`
```python
from wye import (
    Wye, StaticFiles
)
from app_search.app import app as app_search
app = Wye()
app.mount("/app_search", app_search)
```

При монтирование приложений, в которых находятся приложение `StaticFiles` лучше указывать путь названием таким, которое содержит сама папка, в данном случае `app_search`. Так `Wye` поймёт где искать файл

### Request

Запрос, который принимет ваш обработчик:
```python
from wye import Request
```

Сигнатура: `Request(scope, receive=None)`

#### Property

1) `Request.url` - получить url, по которому был запрос
2) `Request.method` - получить метод
3) `Request.query_params` - получить данные строки запросы

#### Methods

1) `await Request.body()` - Тело запроса в байтах
2) `await Request.json()` - Тело запроса, проанализированное как `JSON`

Вы также можете получить данные через `async for`:
```python
async def app(receive, send):
    request = Request(scope, receive)
    body = b''
    async for chunk in request.stream():
        body += chunk
```

### URL

```python
from wye import URL
```

Сигнатура: `URL(url: str | bytes)`

1) `Request.url` - получить url, по которому был запрос
2) `Request.url.query_string` - получить строку запроса
3) `Request.url.path` - получить путь, по которому был сделан
4) `Request.url.scheme` - получить протокол
5) `Request.url.netloc` - получить сетевое расположение

### Headers

```python
from wye import Headers
```

Сигнатура: `Headers(raw_headers = None)`

1) `Request.headers` - получить Headers
2) `Request.headers.values()` - получить значения
3) `Request.headers.keys()` - получить ключи
4) `Request.headers.items()` - получить список заголовков
5) `Request.headers.get(key)` - получить заголовок `(название, значение)`

### QueryParams

```python
from wye import QueryParams
```

Сигнатура: `Headers(raw_params = None)`

1) `Request.query_params` - получить QueryParams
2) `Request.query_params.values()` - получить значения
3) `Request.query_params.keys()` - получить ключи
4) `Request.query_params.items()` - получить список паметры строки
5) `Request.query_params.get(key)` - получить параметр строки `(название, значение)`

## Serializers

`src` - `wye/serializers`

### Schemas

```python
from wye.serializers import Serializer
```

Пример:
```python
from wye.serializers import Serializer
from wye.serializers import fields


class Serializer1(Serializer):
    param_1: int = fields.INT(default = 10, alias = "param1")
    param_2: str = fields.STR(alias = "param2")
    param_3: float = fields.FLOAT()
    param_4: bool = fields.BOOL(alias = "param4")


class Serializer2(Serializer):
    param_1: Serializer_1 = fields.SERIALIZER(alias = "param1")
    param_2: int = fields.INT(alias = "param2")


class Serializer3(Serializer2):
    param_3: Serializer2 = fields.SERIALIZER(alias = "param3")
    param_4: int = fields.INT(alias = "param4")
```

Методы:
- `.is_validate(json: Union[Dict[str, Any], List[Dict[str, Any]]])` - Провалидировать обьекты. Возвращает `Tuple[bool, Union[Dict[str, Any], List[Dict[str, Any]]]]`


```python
from wye.serializers import build_json
```

- `build_json(json: Union[Dict[str, Any], List[Dict[str, Any]]], rules: Type[BaseSerializer])` -
Собирает `json` по правилам. Возвращает `Union[Dict[str, Any], List[Dict[str, Any]]]`

### Fields

```python
from wye.serializers import fields
```

Следующие аргументы:
- `default: Any` - дефолтное значение, работает если поле необязательное (`param_1: Optional[int]`), по умолчанию `None`
- `alias: str` - публичное имя поля, по умолчанию `None`
- `required: bool` - обязательный ли параметр
- `validators: Callable[[Any], Any]` - валидаторы, принимают один параметр, пример:
    ```python
    from wye.serializers import Serializer
    from wye.serializers import fields

    def validator(value):
        return value * 10

    class Serializer2(Serializer):
        param_1: int = fields.INT(validators=[validator])
    ```
- `max_length: int` - для строковых значений это добавляет соответствующую проверку
- `min_length: int` - для строковых значений это добавляет соответствующую проверку
- `gt: Union[int, float]` - для числовых значений *(int, float)* добавляет проверку `больше чем`
- `ge: Union[int, float]` - для числовых значений это добавляет проверку `больше или равно`
- `lt: Union[int, float]` - для числовых значений это добавляет проверку `меньше чем`
- `le: Union[int, float]` - для числовых значений это добавляет проверку `меньше или равно`

1) Типы:
    - `fields.BOOL` - Булевое значение
    - `fields.STR` - Строка
    - `fields.INT` - Целочисленное число
    - `fields.FLOAT` - Число с плавающей точкой
    - `fields.SERIALIZER` - Вложенный сериализатор
    - `fields.BYTES` - байты
