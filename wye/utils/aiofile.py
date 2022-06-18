from typing import (
	Any, Union, Optional
)
from concurrent.futures import (
	ProcessPoolExecutor, ThreadPoolExecutor
)
import asyncio
import io


class AsyncFile(object):
	class _ReadContent:
		"""
		Данные чтения из кэша читать в дочерней ветке используйте _ReadContent().
		Content для хранения возвращаемого значения
		"""
		def __init__(
			self,
			content: Any = None,
			more_body: bool = True
		):
			self.content = content
			self.more_body = more_body

	def __init__(
		self,
		path :str,
		open_flag: str = "r",
		executor: Optional[Union[ProcessPoolExecutor, ThreadPoolExecutor]] = None
	) -> None:
		self.path = path
		self.open_flag = open_flag
		self._f = open(path, open_flag)
		self._loop = asyncio.get_event_loop()
		self._rw_lock = asyncio.Lock()
		self._executor = executor

	async def read(self) -> bytes:
		if not self._f.readable():
			raise io.UnsupportedOperation()

		async with self._rw_lock:
			over_semaphore = asyncio.Semaphore(0)
			_read_content = self._ReadContent()
			self._loop.run_in_executor(self._executor, self._read, _read_content, over_semaphore)

			await over_semaphore.acquire()

			return _read_content.content

	async def read_by_chunk(
		self,
		chunk: int
	):
		if not self._f.readable():
			raise io.UnsupportedOperation()

		async with self._rw_lock:
			_read_content = self._ReadContent(content = b"file")
			over_semaphore = asyncio.Semaphore(0)

			while _read_content.more_body:
				self._loop.run_in_executor(
					self._executor,
					self._read_by_chunk,
					_read_content,
					over_semaphore,
					chunk
				)

				if _read_content.more_body:
					await over_semaphore.acquire()

				yield _read_content.content

	async def write(
		self,
		content: Any
	) -> None:
		if not self._f.writable():
			raise io.UnsupportedOperation()

		async with self._rw_lock:
			over_semaphore = asyncio.Semaphore(0)
			self._loop.run_in_executor(self._executor, self._write, content, over_semaphore)

			await over_semaphore.acquire()

	async def seek(self,
		offset,
		where: int = 0
	) -> None:
		async with self._rw_lock:
			self._f.seek(offset, where)

	async def close(self) -> None:
		async with self._rw_lock:
			self._f.close()

	def _read(
		self,
		r_content: _ReadContent,
		over_semaphore: asyncio.Semaphore
	) -> None:
		r_content.content = self._f.read(-1)
		over_semaphore.release()

	def _read_by_chunk(
		self,
		r_content: _ReadContent,
		over_semaphore: asyncio.Semaphore,
		chunk: int
	) -> None:
		r_content.content = self._f.read(chunk)
		over_semaphore.release()

		if not len(r_content.content) == chunk:
			r_content.more_body = False

	def _write(
		self,
		content: Any,
		over_semaphore: asyncio.Semaphore
	) -> None:
		self._f.write(content)
		over_semaphore.release()

	async def __aenter__(self):
		return self

	async def __aexit__(
		self,
		exc_type,
		exc_val,
		traceback
	) -> None:
		self._f.close()
