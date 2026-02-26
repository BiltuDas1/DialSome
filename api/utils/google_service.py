from core import settings
from fastapi import HTTPException, status
from google.oauth2 import id_token
from google.auth.transport import requests


def verify_google_token(token_from_client: str):
  try:
    idinfo = id_token.verify_oauth2_token(
      token_from_client, requests.Request(), settings.CLIENT_ID
    )
    return idinfo

  except Exception:
    raise HTTPException(
      status_code=status.HTTP_401_UNAUTHORIZED, detail="Invalid Google Token"
    )
