from .environ import ENV

PRODUCTION = ENV.exist("PRODUCTION")
DOCS_URL = "/docs" if not PRODUCTION else None
REDOC_URL = "/redoc" if not PRODUCTION else None
OPENAPI_URL = "/openapi.json" if not PRODUCTION else None
