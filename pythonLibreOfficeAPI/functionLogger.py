import os
import functools
from datetime import datetime
from threading import Lock

LOGGING_PATH = '/home/admin/SharedFolder/'

LOGGING_FILENAME = 'logPython.txt'


_log_lock = Lock()

def getLogPath():
    path = LOGGING_PATH + str(datetime.now().year)
    log_file_path = os.path.abspath(os.path.join(os.path.dirname(__file__), path, LOGGING_FILENAME))
    
    return log_file_path

def write_log(message):
    timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]
    line = f"[{timestamp}] {message}\n"

    os.makedirs(os.path.dirname(getLogPath()), exist_ok=True)
    with _log_lock:
        with open(getLogPath(), 'a') as f:
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
