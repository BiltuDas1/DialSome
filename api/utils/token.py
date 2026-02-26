import secrets
import string


def generate_token(length: int = 32) -> str:
  """
  Generate URLSafe Random generated token

  :param length: Length of the output string
  :type length: int
  :return: Returns randomly generated URL safe string
  :rtype: str
  """
  alphabet = string.ascii_letters + string.digits + "-_"
  return "".join(secrets.choice(alphabet) for _ in range(length))


def generate_password(length: int = 20) -> str:
  """
  Generate Randomly generated Password

  :param length: Length of the output string
  :type length: int
  :return: Returns randomly generated password string
  :rtype: str
  """
  letters = "abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ"
  digits = "23456789"
  symbols = "!@#$%^&*()_+-=[]{}"
  alphabet = letters + digits + symbols
  return "".join(secrets.choice(alphabet) for _ in range(length))
