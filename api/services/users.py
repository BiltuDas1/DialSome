from models import User
from core import logger


async def is_user_exist(email: str) -> User | None:
  """
  Checks if the user record exist in the database or not
  """
  user = await User.get_or_none(email=email)
  if user is None:
    logger.LOGGER.debug(f"User doesn't exist: {email}")
    return None
  return user