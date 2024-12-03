
import sys
import subprocess
import win32event
import win32api
from winerror import ERROR_ALREADY_EXISTS


mutex_name = "my_script_mutex"
mutex_handle = win32event.CreateMutex(None, False, mutex_name)
last_error = win32api.GetLastError()

if last_error == ERROR_ALREADY_EXISTS:
    mutex_handle = None
    print("Another instance of the script is already running.")
    sys.exit(1)


try:
    upload_prog = subprocess.run(["C:\\Users\\break\\AppData\\Local\\Arduino15\\packages\\arduino\\tools\\avrdude\\6.3.0-arduino17/bin/avrdude.exe" , "-CC:\\Users\\break\\AppData\\Local\\Arduino15\\packages\\arduino\\tools\\avrdude\\6.3.0-arduino17/etc/avrdude.conf" , "-v", "-V", "-patmega2560", "-cwiring", "-PCOM3", "-b115200", "-D", "-Uflash:w:C:\\Users\\break\\AppData\\Local\\Temp\\arduino\\sketches\\C88CE14B9FB7D47E1247DD64C4B52D81/FINALCODE.ino.ino.hex:i"], shell=True, capture_output=True, text=True)
except Exception as e:
    print(e)
