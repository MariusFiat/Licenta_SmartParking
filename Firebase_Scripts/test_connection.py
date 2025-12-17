import os
import sys
from pathlib import Path

try:
  from dotenv import load_dotenv
  env_path = Path('.env')
  if env_path.exists():
    load_dotenv(dotenv_path=env_path)
except Exception:
  # dotenv not installed; continue reading env vars normally
  pass

try:
  import pyrebase
except Exception:
  pyrebase = None

try:
  import firebase_admin
  from firebase_admin import credentials
except Exception:
  firebase_admin = None


def _get_pyrebase_config_from_env():
  return {
    'apiKey': os.getenv('FIREBASE_API_KEY'),
    'authDomain': os.getenv('FIREBASE_AUTH_DOMAIN'),
    'databaseURL': os.getenv('FIREBASE_DATABASE_URL'),
    'projectId': os.getenv('FIREBASE_PROJECT_ID'),
    'storageBucket': os.getenv('FIREBASE_STORAGE_BUCKET'),
    'messagingSenderId': os.getenv('FIREBASE_MESSAGING_SENDER_ID'),
    'appId': os.getenv('FIREBASE_APP_ID'),
    'measurementId': os.getenv('FIREBASE_MEASUREMENT_ID'),
  }


def _all_pyrebase_values_present(cfg):
  required = ['apiKey', 'authDomain', 'databaseURL', 'projectId', 'storageBucket', 'appId']
  return all(cfg.get(k) for k in required)

def init_pyrebase_from_env():
  cfg = _get_pyrebase_config_from_env()
  if not _all_pyrebase_values_present(cfg):
    missing = [k for k, v in cfg.items() if not v]
    print('Missing Pyrebase env vars:', ', '.join(missing))
    return None
  if not pyrebase:
    print('`pyrebase` is not installed in the environment.')
    return None
  try:
    fb = pyrebase.initialize_app(cfg)
    db = fb.database()
    print('Initialized Pyrebase (Realtime Database) client.')
    return db
  except Exception as e:
    print('Failed to initialize pyrebase:', e)
    return None


def main():
  db = init_pyrebase_from_env()

  if not db:
    print('\nNo Firebase credentials detected.\n')
    print('Options:')
    print('- Set environment variables for Pyrebase (e.g. FIREBASE_API_KEY, FIREBASE_DATABASE_URL, etc).')
    print('- Or set `GOOGLE_APPLICATION_CREDENTIALS` or `SERVICE_ACCOUNT_PATH` to a service account JSON file for the Admin SDK.')
    sys.exit(1)

  # At this point at least one client is available. Avoid printing secrets.
  if db:
    print('Realtime Database client ready (not showing credentials).')


if __name__ == '__main__':
  main()