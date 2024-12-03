

import sys
import subprocess
import win32event
import win32api
from winerror import ERROR_ALREADY_EXISTS


mutex_name = "my_script_mutex"
mutex_handle = win32event.CreateMutex(None, False, mutex_name)
last_error = win32api.GetLastError()
4
if last_error == ERROR_ALREADY_EXISTS:
    mutex_handle = None
    print("Another instance of the script is already running.")
    sys.exit(1)


run_test = subprocess.run(["C:\\Users\\break\\AppData\\Local\\Arduino15\\packages\\arduino\\tools\\avrdude\\6.3.0-arduino17/bin/avrdude.exe" , "-CC:\\Users\\break\\AppData\\Local\\Arduino15\\packages\\arduino\\tools\\avrdude\\6.3.0-arduino17/etc/avrdude.conf" , "-v", "-V", "-patmega2560", "-cwiring", "-PCOM3", "-b115200", "-D", "-Uflash:w:C:\\Users\\break\\AppData\\Local\\Temp\\arduino\\sketches\\A27D3002C7CDA84EEA0C1D77D1D1911C/TESTCODE.ino.hex:i"], shell=True, capture_output=True, text=True)
#print(run_test)


#dir_run = subprocess.Popen(["cd", "C:\\Users\\cplas\\OneDrive\\Desktop\\Laser\\Laser"], stdout=subprocess.PIPE, text=True)
game_run = subprocess.Popen(["python", "C:\\Users\\break\\Desktop\\lasers2\\Lasers2/laser2_test.py"], stdout=subprocess.PIPE, text=True)

output, error = game_run.communicate()

print(output)




#"C:\Users\break\AppData\Local\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino17/bin/avrdude.exe" "-CC:\Users\break\AppData\Local\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino17/etc/avrdude.conf" -v -V -patmega2560 -cwiring "-PCOM3" -b115200 -D "-Uflash:w:C:\Users\break\AppData\Local\Temp\arduino\sketches\A27D3002C7CDA84EEA0C1D77D1D1911C/TESTCODE.ino.hex:i" 