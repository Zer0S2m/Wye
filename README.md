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

### FileResponse

Принимает название файла и возвращает файд

```python
from wye import FileResponse

class App:
    def __init__(self, scope) -> None:
        self.scope = scope

    async def __call__(self, receive, send):
        response = FileResponse("example_file.txt")
        await response(receive, send)
```

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
from wye import Wye
```

#### Methods

1) `.add_route(path, func, methods=["GET"])` - добавить маршрут
Функция должна принимать параметр `request` и возвращать `Response`, *example*:
`func(request, **kwargs) -> responce`

2) `.route(path)` - Добавить маршрут
Функция должна принимать параметр `request` и возвращать `Response`, *example*:
`func(request, **kwargs) -> responce`

3) `.mount(path)` - Монтировать приложение `Wye`

### Request

Запрос, который принимет ваш обработчик:

```python
from wye import Request
```

1) `Request.url` - получить url, по которому был запрос
2) `Request.method` - получить метод
3) `Request.query_params` - получить данные строки запросы

### URL

```python
from wye import URL
```

1) `Request.url` - получить url, по которому был запрос
2) `Request.url.query_string` - получить строку запроса
3) `Request.url.path` - получить путь, по которому был сделан
4) `Request.url.scheme` - получить протокол
5) `Request.url.netloc` - получить сетевое расположение

### Headers

```python
from wye import Headers
```

1) `Request.headers` - получить Headers
2) `Request.headers.values()` - получить значения
3) `Request.headers.keys()` - получить ключи
4) `Request.headers.items()` - получить список заголовков
5) `Request.headers.get(key)` - получить заголовок `(название, значение)`

### QueryParams

```python
from wye import QueryParams
```

1) `Request.query_params` - получить QueryParams
2) `Request.query_params.values()` - получить значения
3) `Request.query_params.keys()` - получить ключи
4) `Request.query_params.items()` - получить список паметры строки
5) `Request.query_params.get(key)` - получить параметр строки `(название, значение)`
