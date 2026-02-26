from .environ import ENV
from .debug import DEBUG
from utils import eddsa


DOCS_URL = "/docs" if DEBUG else None
REDOC_URL = "/redoc" if DEBUG else None
OPENAPI_URL = "/openapi.json" if DEBUG else None
CLIENT_ID = "87640868239-dje4suitg3fi100c8hirlunckcji4g40.apps.googleusercontent.com"

# Load REDIS_URI Environment Variable
if not ENV.exist("REDIS_URI"):
  raise EnvironmentError("REDIS_URI can't be empty")
REDIS_URI = str(ENV.get("REDIS_URI"))

# Load POSTGRESQL_URI Environment Variable
if not ENV.exist("POSTGRESQL_URI"):
  raise EnvironmentError("POSTGRESQL_URI doesn't exist")
POSTGREDSQL_URI = str(ENV.get("POSTGRESQL_URI"))

# EdDSA Key
if not ENV.exist("EDDSA_PRIVATE_KEY"):
  raise EnvironmentError("EDDSA_PRIVATE_KEY can't be empty")
EDDSA_KEY = eddsa.EdDSA(str(ENV.get("EDDSA_PRIVATE_KEY")))

# User Token
REFRESH_TOKEN_EXPIRY = 7 * 24 * 60 * 60  # 7 Days
ACCESS_TOKEN_EXPIRY = 60 * 60  # 1 hour
