#include <string>
#include <string.h>
#include <fstream>
#include <iostream>
#include <windows.h>
#include <list>
#include "json.hpp"

using namespace std;
using namespace nlohmann;

int execute(const string & cmd)
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

list<string> listFiles(const string & dir)
{
    HANDLE hFind;
    WIN32_FIND_DATA findData;
    LARGE_INTEGER size;

    char dir_[100] = {'\0'};
    strcpy(dir_, dir.c_str());
    strcat(dir_, "\\*.*");

    hFind = FindFirstFile(dir_, &findData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        //cout << "Failed to find first file!\n";
        return list<string>();
    }
    list<string> filenames;
    do
    {
        // 忽略"."和".."两个结果 
        if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) continue;
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;    // 是否是目录 
        filenames.emplace_back(findData.cFileName);
    } while (FindNextFile(hFind, &findData));
    return filenames;
}

string getExtName(const string & filename)
{
    string extName;
    auto pos = filename.find_last_of('.');
    if (pos == string::npos) return string();
    return filename.substr(pos + 1);
}

const string RootDir = "C:\\IntelliEco";
const string RecordDir = "C:\\IntelliEco\\record";

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
                int mothCount = execute("MothCount.exe " + string(uuidDir) + "\\" + imageFilename);
                newFilename = to_string(time) + '_' + username;
                json record;
                record["uploader"] = username;
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
            if (requestType == "refresh")
            {
                cout << username << " requested records. \n";
                
                list<string> filenames = listFiles(RecordDir);
                response["records"] = json::array();
                for (string & filename : filenames)
                {
                    string extName = getExtName(filename);
                    if (extName == "json")
                    {
                        json record;
                        ifstream recordStream(RecordDir + "\\" + filename);
                        recordStream >> record;

                        string uploader;
                        long long int time = 0;
                        double longtitude = 0, latitude = 0;
                        int mothCount = 0;

                        record["uploader"].get_to(uploader);
                        record["time"].get_to(time);
                        record["longtitude"].get_to(longtitude);
                        record["latitude"].get_to(latitude);
                        record["mothCount"].get_to(mothCount);

                        response["records"].push_back(json::object({
                            {"uploader", uploader},
                            {"time", time},
                            {"longtitude", longtitude},
                            {"latitude", latitude},
                            {"mothCount", mothCount}
                        }));
                    }
                }
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

    responseStream << response.dump(4);
    responseStream.close();

    return 0;
}
