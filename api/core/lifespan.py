from fastapi import FastAPI
from contextlib import asynccontextmanager
from tortoise import Tortoise
from . import logger
import database


@asynccontextmanager
async def APILifespan(app: FastAPI):
  yield  # All API tasks are here

  await Tortoise.close_connections()
  logger.LOGGER.debug("Closed connection of Database Server")
  await database.CACHE.close()
  logger.LOGGER.debug("Closed connection of Cache Server")
  await database.AUTH_STORAGE.close()
  logger.LOGGER.debug("Closed connection of Auth Storage (Cache) Server")
