from fastapi import HTTPException, Depends, status
from fastapi.security import HTTPBearer, HTTPAuthorizationCredentials
from utils import google_service
from core import logger

security = HTTPBearer()


async def get_current_user_email(
  auth: HTTPAuthorizationCredentials = Depends(security),
):
  token = auth.credentials
  try:
    payload = google_service.verify_google_token(token)
    return payload
  except Exception:
    logger.LOGGER.debug("Invalid JWT")
    raise HTTPException(
      status_code=status.HTTP_401_UNAUTHORIZED,
      detail="Invalid or expired Google token",
    )
