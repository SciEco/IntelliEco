import os
import time
import subprocess

rootDir = "C:\\IntelliEco"
bufferDir = "C:\\IntelliEco\\buffer"
while True:
    list = os.listdir(bufferDir)
    for i in range(0, len(list)):
        uuid = list[i]
        uuidDir = bufferDir + "\\" + uuid;
        requestDir = uuidDir + "\\request"
        responseDir = uuidDir + "\\response"
        receiveDir = uuidDir + "\\receive"
        if os.path.exists(requestDir):
            print(f"A request has been posted by {uuid}.")
            os.rmdir(requestDir)
            if os.path.exists(responseDir):
                os.rmdir(responseDir)
            subprocess.call(rootDir + "\\RequestProcess.exe " + uuidDir)
            os.remove(uuidDir + "\\request.json")
            os.mkdir(responseDir)
        if os.path.exists(receiveDir):
            print(f"A response has been fetched by {uuid}.")
            os.rmdir(receiveDir)
            os.remove(uuidDir + "\\response.json")
            os.rmdir(uuidDir)
            
    time.sleep(1)
