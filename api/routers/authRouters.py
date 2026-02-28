from fastapi import APIRouter, status
from fastapi.responses import JSONResponse
from utils import jwt
from models import User
from database import AUTH_STORAGE
from core import settings


router = APIRouter(prefix="/token", tags=["Authentication"])


@router.post("/refresh")
async def refresh(refresh_token: str):
  refreshToken = jwt.JWT.to_refresh_token(refresh_token)
  if refreshToken is None:
    return JSONResponse(
      content={"status": False, "message": "Invalid Refresh Token"},
      status_code=status.HTTP_422_UNPROCESSABLE_CONTENT,
    )

  user_id = await AUTH_STORAGE.get_id(refreshToken.get_jti())
  if user_id is None:
    return JSONResponse(
      content={"status": False, "message": "Refresh token expired"},
      status_code=status.HTTP_404_NOT_FOUND,
    )

  user = await User.get_or_none(id=user_id)
  if user is None:
    return JSONResponse(
      content={"status": False, "message": "Refresh token expired"},
      status_code=status.HTTP_404_NOT_FOUND,
    )

  new_jwt = jwt.JWT(user)

  success = await AUTH_STORAGE.update_token(
    old_jti=refreshToken.get_jti(),
    new_jti=new_jwt.refresh_token.get_jti(),
    new_expiry_at=settings.REFRESH_TOKEN_EXPIRY,
  )

  if not success:
    return JSONResponse(
      content={"status": False, "message": "JWT Renewal failed"},
      status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
    )

  return JSONResponse(
    content={
      "status": True,
      "message": "JWT Renewal successful",
      "data": new_jwt.to_dict(),
    },
    status_code=status.HTTP_200_OK,
  )
