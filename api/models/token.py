from pydantic import BaseModel


class RefreshTokenPayload(BaseModel):
  refresh_token: str
