from fastapi import FastAPI
from fastapi.responses import JSONResponse
from fastapi import status
from routers import voicecallRouter, usersRouter
from core import settings

app = FastAPI(
  title="DialSome",
  docs_url=settings.DOCS_URL,
  redoc_url=settings.REDOC_URL,
  openapi_url=settings.OPENAPI_URL
)
app.include_router(voicecallRouter.router)
app.include_router(usersRouter.router)


@app.get("/")
async def root():
  return JSONResponse(
    {
      "status": True,
      "message": "Service is working"
    }, status_code=status.HTTP_200_OK
  )


if __name__ == "__main__":
  import uvicorn
  uvicorn.run(app, host="0.0.0.0", port=8000)
