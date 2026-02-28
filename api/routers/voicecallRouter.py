from fastapi import APIRouter, HTTPException
from firebase_admin import messaging
from models.user import User


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


@router.post("/send")
async def call_person(email: str, Offer: str):
  payload = {
    "type": "offer",
    "sdp": Offer,
    "caller_email": "caller@example.com",  # Ideally fetched from the sender's session
  }
  await send_fcm_notification(email, payload)
  return {"status": "Offer sent successfully"}


@router.post("/receive")
async def receive_call(email: str, Answer: str):
  payload = {
    "type": "answer",
    "sdp": Answer,
  }
  await send_fcm_notification(email, payload)
  return {"status": "Answer sent successfully"}
