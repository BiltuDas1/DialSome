from models import User
from core import logger, settings
from utils import jwt
from database import AUTH_STORAGE


async def login(email: str) -> tuple[User, jwt.JWT] | None:
  """
  Checks if the user record exist in the database or not
  """
  user = await User.get_or_none(email=email)
  if user is None:
    logger.LOGGER.debug(f"User doesn't exist: {email}")
    return None

  new_jwt = jwt.JWT(user)
  await AUTH_STORAGE.add_token(
    jti=new_jwt.refresh_token.get_jti(),
    user_id=str(user.id),
    expire_at=settings.REFRESH_TOKEN_EXPIRY,
  )

  return user, new_jwt


async def register(first_name: str, last_name: str, email: str, google_id: str) -> bool:
  """
  Store the user information in the database
  Returns:
    - True if the user is created, False if user already exist
  """
  _, created = await User.update_or_create(
    firstname=first_name,
    lastname=last_name,
    email=email,
    google_id=google_id,
  )
  return created
