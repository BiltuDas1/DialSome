from models import User
from . import access_token, refresh_token
import jwt as jwt_token
from utils.auth_token import AuthToken
from core import logger


class JWT(AuthToken):
  def __init__(self, user: User):
    self.access_token = access_token.AccessToken(str(user.id))
    self.refresh_token = refresh_token.RefreshToken(str(user.id))

  def to_dict(self) -> dict[str, str]:
    return {
      "access_token": self.access_token.get_token(),
      "refresh_token": self.refresh_token.get_token(),
    }

  @classmethod
  def to_refresh_token(cls, token: str) -> refresh_token.RefreshToken | None:
    try:
      return refresh_token.RefreshToken(sub="", refresh_token=token)
    except jwt_token.exceptions.ExpiredSignatureError:
      logger.LOGGER.debug("RefreshToken: Expired token")
      return None
    except jwt_token.exceptions.InvalidTokenError:
      logger.LOGGER.debug("RefreshToken: Token is malformed or invalid")
      return None

  @classmethod
  def to_access_token(cls, token: str) -> access_token.AccessToken | None:
    try:
      return access_token.AccessToken(sub="", access_token=token)
    except jwt_token.exceptions.ExpiredSignatureError:
      logger.LOGGER.debug("AccessToken: Expired token")
      return None
    except jwt_token.exceptions.InvalidTokenError:
      logger.LOGGER.debug("AccessToken: Token is malformed or invalid")
      return None
