from fastapi import APIRouter


router = APIRouter(prefix="/users", tags=["Users"])


@router.post("/login")
async def login_user():
  pass


@router.post("/register")
async def register_user():
  pass


@router.post("/ban")
async def ban_user():
  pass
