from fastapi import FastAPI
from fastapi.responses import HTMLResponse
from routers import voicecallRouter, usersRouter

app = FastAPI()
app.include_router(voicecallRouter.router)
app.include_router(usersRouter.router)


@app.get("/", response_class=HTMLResponse)
async def root():
  return """
  <DOCTYPE html>
  <html>
    <head>
      <title>DialSome API</title>
    </head>
    <body>
      <p>Service is working</p>
    </body>
  </html>
  """


if __name__ == "__main__":
  import uvicorn

  uvicorn.run(app, host="0.0.0.0", port=8000)
