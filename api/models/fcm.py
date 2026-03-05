from pydantic import BaseModel


class FCMUpdate(BaseModel):
  fcm_token: str
