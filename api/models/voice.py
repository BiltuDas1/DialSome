from pydantic import BaseModel


class VoiceData(BaseModel):
  email: str
