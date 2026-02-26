from fastapi import FastAPI
from contextlib import asynccontextmanager
from tortoise import Tortoise
from . import logger


@asynccontextmanager
async def APILifespan(app: FastAPI):
  yield  # All API tasks are here

  await Tortoise.close_connections()
  logger.LOGGER.debug("Closed connection of Database Server")
