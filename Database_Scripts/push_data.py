#!/usr/bin/env python3
from pathlib import Path
import sys
import time

# Add Firebase_Scripts to import path so we can reuse init_pyrebase_from_env
ROOT = Path(__file__).parent
sys.path.insert(0, str(ROOT / "Firebase_Scripts"))

import test_connection as tc


def main():
    db = tc.init_pyrebase_from_env()
    if not db:
        print("No database client available. Ensure env vars or service account are set.")
        return 1

    payload = {
        "message": "Test push from push_data.py",
        "created_at": int(time.time())
    }

    try:
        res = db.child("test_push").push(payload)
        print("Successfully pushed data. Response:", res)
        return 0
    except Exception as e:
        print("Failed to push data:", e)
        return 2


if __name__ == "__main__":
    # raise SystemExit(main())
    main()
