from tortoise.models import Model
from tortoise import fields
from typing import cast
from utils import security, token


class User(Model):
  id = fields.UUIDField(primary_key=True)
  firstname = fields.CharField(max_length=50)
  lastname = fields.CharField(max_length=50)
  email = fields.CharField(max_length=255, unique=True)
  google_id = fields.CharField(max_length=50, unique=True, index=True)
  password = fields.CharField(max_length=60)
  created_at = fields.DatetimeField(auto_now_add=True, null=False)
  is_active = fields.BooleanField(default=False, null=False)

  @classmethod
  async def create(cls, **kwargs):
    # Make email lowercase (If exist)
    if "email" in kwargs:
      kwargs["email"] = cast(str, kwargs["email"]).lower()

    # Hash password (If exist)
    if "password" in kwargs:
      kwargs["password"] = security.hash_password(cast(str, kwargs["password"]))
    else:
      kwargs["password"] = security.hash_password(token.generate_password())

    return await super().create(**kwargs)

  def set_password(self, password: str):
    self.password = security.hash_password(password)
