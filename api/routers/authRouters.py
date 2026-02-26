from fastapi import APIRouter


router = APIRouter(prefix="/token", tags=["Authentication"])

@router.post("/refresh")
async def refresh():
  pass
