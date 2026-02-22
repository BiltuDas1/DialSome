from fastapi import APIRouter


router = APIRouter(prefix="/voicecall", tags=["Voicecall"])


@router.post("/send")
async def call_person(email: str, Offer: str):
  pass


@router.post("/receive")
async def receive_call(email: str, Answer: str):
  pass
