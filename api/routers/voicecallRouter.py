from fastapi import APIRouter, HTTPException
from firebase_admin import messaging
from models.user import User
import uuid_utils as uuid
from pydantic import BaseModel


router = APIRouter(prefix="/voicecall", tags=["Voicecall"])


async def send_fcm_notification(target_email: str, payload: dict):
  # Find the recipient user in the database
  user = await User.get_or_none(email=target_email.lower())

  if not user or not user.fcm_token:
    raise HTTPException(
      status_code=404, detail="Recipient not found or FCM token missing"
    )

  # Construct the FCM Data Message
  # Data messages allow the app to process logic (like starting WebRTC)
  # even if it's in the background.
  message = messaging.Message(
    data=payload,
    token=user.fcm_token,
  )

  # Send the message via Firebase
  try:
    response = messaging.send(message)
    return response
  except Exception as e:
    raise HTTPException(status_code=500, detail=f"FCM Send Error: {str(e)}")


class VoiceData(BaseModel):
  email: str

@router.post("/send")
async def call_person(data: VoiceData):
  room_id = str(uuid.uuid7())
  payload = {
    "type": "incoming_call",
    "room_id": room_id,
    "caller_email": "caller@example.com",
  }
  await send_fcm_notification(data.email, payload)
  return {"status": "Offer sent successfully", "room_id": room_id}
