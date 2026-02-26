from fastapi import APIRouter, Header, HTTPException, status
from fastapi.responses import JSONResponse
from typing import Annotated
from utils import google_service, jwt, auth_token
from models import User
from core import logger
from services import users


router = APIRouter(prefix="/users", tags=["Users"])


@router.post("/login")
async def login_user(authorization: Annotated[str | None, Header()] = None):
  if authorization is None:
    print("No Google Authorization header provided", flush=True)
    return JSONResponse(
      content={"status": False, "message": "No Authorization header provided"},
      status_code=status.HTTP_406_NOT_ACCEPTABLE,
    )

  token = authorization.split(" ")[1]
  try:
    result = google_service.verify_google_token(token)
    user = await users.is_user_exist(email=result["email"])
    if user is None:
      return JSONResponse(
        content={"status": False, "message": "User doesn't exist"},
        status_code=status.HTTP_404_NOT_FOUND,
      )
    
    jwt_token: auth_token.AuthToken = jwt.JWT(user)

    logger.LOGGER.debug("Login Successful")
    return JSONResponse(
      content={
        "status": True, 
        "message": "Login Successful",
        "data": {
          "id": str(user.id),
          "email": user.email,
          "firstname": user.firstname,
          "lastname": user.lastname,
          "jwt": jwt_token.to_dict()
        }
      },
      status_code=status.HTTP_200_OK,
    )
  except HTTPException:
    logger.LOGGER.debug("Invalid JWT")
    return JSONResponse(
      content={"status": False, "message": "Invalid JWT"},
      status_code=status.HTTP_422_UNPROCESSABLE_CONTENT,
    )


@router.post("/register")
async def register_user(authorization: Annotated[str | None, Header()] = None):
  if authorization is None:
    print("No Google Authorization header provided", flush=True)
    return JSONResponse(
      content={"status": False, "message": "No Authorization header provided"},
      status_code=status.HTTP_406_NOT_ACCEPTABLE,
    )

  token = authorization.split(" ")[1]
  try:
    result = google_service.verify_google_token(token)

    try:
      _, created = await User.get_or_create(
        firstname=result["given_name"],
        lastname=result["family_name"],
        email=result["email"],
        google_id=result["sub"],
      )

      if created:
        logger.LOGGER.debug("Registration Successful")
        return JSONResponse(
          content={"status": True, "message": "Registration Successful"},
          status_code=status.HTTP_201_CREATED,
        )
      else:
        logger.LOGGER.debug("Email already exist")
        return JSONResponse(
          content={"status": False, "message": "Email already exist"},
          status_code=status.HTTP_409_CONFLICT,
        )
    except Exception as e:
      logger.LOGGER.error(f"Error: {e}")
      return JSONResponse(
        content={"status": False, "message": "Internal server error"},
        status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
      )
  except HTTPException:
    logger.LOGGER.debug("Invalid JWT")
    return JSONResponse(
      content={"status": False, "message": "Invalid JWT"},
      status_code=status.HTTP_422_UNPROCESSABLE_CONTENT,
    )


@router.post("/ban")
async def ban_user():
  pass
