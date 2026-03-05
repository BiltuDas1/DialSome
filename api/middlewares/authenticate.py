from fastapi import Depends, HTTPException, status
from fastapi.security import HTTPBearer, HTTPAuthorizationCredentials
from utils import jwt, auth_token
from core import logger

security = HTTPBearer()


async def verify_jwt(
  credentials: HTTPAuthorizationCredentials = Depends(security),
) -> str:
  token = credentials.credentials
  try:
    accessToken: auth_token.access_token.AuthAccessToken | None = (
      jwt.JWT.to_access_token(token)
    )
    if accessToken is None:
      logger.LOGGER.debug("Invalid Access Token Format")
      raise HTTPException(
        status_code=status.HTTP_401_UNAUTHORIZED,
        detail="Invalid or Expired Access Token",
        headers={"WWW-Authenticate": "Bearer"},
      )
    return accessToken.get_sub()
  except Exception as e:
    logger.LOGGER.error(f"verify_jwt: {e}")
    raise HTTPException(
      status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
      detail=f"{e}",
      headers={"WWW-Authenticate": "Bearer"},
    )
