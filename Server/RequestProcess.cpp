#include <string>
#include <fstream>
#include "json.hpp"

using namespace std;
using namespace nlohmann;

const string RootDir = "C:\\IntelliEco";

int main(int argc, char ** argv)
{
    ifstream usersStream(RootDir + "\\users.json");
    json users;
    usersStream >> users;

    ifstream requestStream(argv[1] + string("\\request.json"));
    json request;
    requestStream >> request;

    ofstream responseStream(argv[1] + string("\\response.json"));
    json response;
    
    string username, authority;
    request["username"].get_to(username);
    if (users.find(username) != users.end())
    {
        if (users[username]["password"] == request["password"])
        {
            users[username]["authority"].get_to(authority);
            response["authority"] = authority;

            string requestType;
            request["request"].get_to(requestType);

            if (requestType == "login");
        }
        else response["authority"] = "unauthorised";
    }
    else response["authority"] = "unauthorised";

    responseStream << response;
    responseStream.close();

    return 0;
}
