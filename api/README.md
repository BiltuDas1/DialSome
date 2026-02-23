# DialSome API

Use the following command to convert `pyproject.toml` to `requirements.txt`

```sh
poetry export -f requirements.txt --output requirements.txt --without-hashes --only main
```