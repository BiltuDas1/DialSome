FROM python:3.13.12-alpine3.23 AS builder

# Install Poetry and generate requirements.txt from it
WORKDIR /app
RUN pip install poetry
RUN poetry self add poetry-plugin-export
COPY pyproject.toml poetry.lock ./
RUN poetry export --without-hashes -f requirements.txt --output requirements.txt 

FROM python:3.13.12-alpine3.23 AS main

ENV PYTHONDONTWRITEBYTECODE=1 \
  PYTHONUNBUFFERED=1 \
  PYTHONPATH=/app

WORKDIR /app
COPY --from=builder /app/requirements.txt .

# Install all the required packages
RUN pip install --no-cache-dir -r requirements.txt

COPY . .

ENV PRODUCTION=true

CMD [ "python", "main.py" ]