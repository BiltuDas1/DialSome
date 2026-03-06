from fastapi import APIRouter, HTTPException, Depends, status
from fastapi.responses import JSONResponse
from firebase_admin import messaging
from models.user import User
import uuid_utils as uuid
from models import voice
from middlewares import authenticate
import asyncio


router = APIRouter(prefix="/voicecall", tags=["Voicecall"])


async def send_fcm_notification(fcm_token: str, payload: dict):
  # Construct the FCM Data Message
  message = messaging.Message(
    data=payload, token=fcm_token, android=messaging.AndroidConfig(priority="high")
  )

  # Send the message via Firebase
  try:
    response = messaging.send(message)
    return response
  except Exception as e:
    raise HTTPException(status_code=500, detail=f"FCM Send Error: {str(e)}")


@router.post("/send")
async def call_person(
  data: voice.VoiceData, user_id: str = Depends(authenticate.verify_jwt)
):
  current_user, target_user = await asyncio.gather(
    User.get_or_none(id=user_id), User.get_or_none(email=data.email)
  )

  if current_user is None:
    return JSONResponse(
      content={"status": False, "message": "Invalid Session"},
      status_code=status.HTTP_401_UNAUTHORIZED,
    )

  if target_user is None or target_user.fcm_token is None:
    return JSONResponse(
      content={"status": False, "message": "User not found"},
      status_code=status.HTTP_404_NOT_FOUND,
    )

  room_id = str(uuid.uuid7())
  payload = {
    "type": "incoming_call",
    "room_id": room_id,
    "caller_email": target_user.email,
  }
  await send_fcm_notification(target_user.fcm_token, payload)
  return JSONResponse(
    content={
      "status": True,
      "message": "Offer sent successfully",
      "data": {"room_id": room_id},
    },
    status_code=status.HTTP_200_OK,
  )
