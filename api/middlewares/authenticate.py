from fastapi import Depends, HTTPException, status
from fastapi.security import HTTPBearer, HTTPAuthorizationCredentials

security = HTTPBearer()


async def verify_jwt(credentials: HTTPAuthorizationCredentials = Depends(security)):
  token = credentials.credentials
  # In a real app, you'd decode a JWT here
  if token != "my-super-secret-token":
    raise HTTPException(
      status_code=status.HTTP_401_UNAUTHORIZED,
      detail="Invalid or expired token",
      headers={"WWW-Authenticate": "Bearer"},
    )
  return
