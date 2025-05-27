import os
import time
import functools
from datetime import datetime
from threading import Lock

LOGGING_PATH = '/home/admin/SharedFolder'

LOGGING_FILENAME = 'logPython.txt'

log_file_path = os.path.abspath(os.path.join(os.path.dirname(__file__), LOGGING_PATH, LOGGING_FILENAME))
_log_lock = Lock()

def write_log(message):
    timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]
    line = f"[{timestamp}] {message}\n"

    with _log_lock:
        with open(log_file_path, 'a') as f:
            f.write(line)

def log_function(func):
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        write_log(f"Entering function: {func.__qualname__}")
        try:
            return func(*args, **kwargs)
        finally:
            write_log(f"Exiting function: {func.__qualname__}")
    return wrapper
