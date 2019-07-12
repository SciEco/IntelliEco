#include <string>
#include <string.h>
#include <fstream>
#include <iostream>
#include <windows.h>
#include "json.hpp"

using namespace std;
using namespace nlohmann;

const string RootDir = "C:\\IntelliEco";
const string RecordDir = "C:\\IntelliEco\\record";

int execute(string cmd)
{
    DWORD    dwExitCode = -1;

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
	
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    char cmd_[100] = {'\0'};
    strcpy(cmd_, cmd.c_str());
	
    // Start the child process. 
    if( !CreateProcess( NULL, // an exe file. 
        cmd_,        // parameter for your exe file. 
        NULL,             // Process handle not inheritable. 
        NULL,             // Thread handle not inheritable. 
        FALSE,            // Set handle inheritance to FALSE. 
        0,                // No creation flags. 
        NULL,             // Use parent's environment block. 
        NULL,             // Use parent's starting directory. 
        &si,              // Pointer to STARTUPINFO structure.
        &pi )             // Pointer to PROCESS_INFORMATION structure.
		) 
    {
        MessageBox(NULL,  "CreateProcess failed.",  "ERROR",NULL );
    }
	
    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );
	
    GetExitCodeProcess(pi.hProcess, & dwExitCode);
	
    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

    return dwExitCode;
}

int main(int argc, char ** argv)
{
    ifstream usersStream(RootDir + "\\users.json");
    json users;
    usersStream >> users;

    string uuidDir = argv[1];
    ifstream requestStream(uuidDir + string("\\request.json"));
    json request;

    requestStream >> request;
    ofstream responseStream(uuidDir + string("\\response.json"));
    json response;

    string username, authority;
    request["username"].get_to(username);

    if (users.find(username) != users.end())
    {
        if (users[username]["password"] == request["password"])
        {
            users[username]["authority"].get_to(authority);
            response["authority"] = authority;

            cout << authority << ' ' << username << " logged in. \n";

            string requestType;
            request["request"].get_to(requestType);
            if (requestType == "login");
            if (requestType == "upload")
            {
                cout << username << " has uploaded a data sample. \n";

                long long int time = 0;
                double longtitude = 0, latitude = 0;
                string imageFilename, newFilename;
                request["time"].get_to(time);
                request["longtitude"].get_to(longtitude);
                request["latitude"].get_to(latitude);
                request["imageFilename"].get_to(imageFilename);
                int mothCount = execute("MothCountTest.exe " + string(uuidDir) + "\\" + imageFilename);
                newFilename = to_string(time) + '_' + username;
                json record;
                record["username"] = username;
                record["time"] = time;
                record["longtitude"] = longtitude;
                record["latitude"] = latitude;
                record["mothCount"] = mothCount;
                record["imageFilename"] = newFilename + ".jpg";
                ofstream recordStream(RecordDir + "\\" + newFilename + ".json");
                recordStream << record;
                system(("move " + (uuidDir + "\\" + imageFilename) + ' ' + (RecordDir + "\\" + newFilename + ".jpg")).c_str());
                response["mothCount"] = mothCount;
            }
        }
        else
        {
            response["authority"] = "unauthorised";
            cout << username << " unauthorised. \n";
        }
    }
    else
    {
        response["authority"] = "unauthorised";
        cout << username << " not found. \n";
    }

    responseStream << response;
    responseStream.close();

    return 0;
}
