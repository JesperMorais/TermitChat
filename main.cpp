#include <iostream>
#include <string>

//TCP SERVER
//STARTA SERVER PÅ PORT -p
//Ta in connections och ge dem username motsvarande
//Hantera användares medelanden
//UI?

using namespace std;

int main(int argc, char* argv[]){
string username;
string ip;

for (int i = 1; i < argc; ++i)
{
    if (string(argv[i]) == "-u" && i + 1 < argc)
    { username = argv[++i];}
    else if (string(argv[i]) == "-ip" && i + 1 < argc)
    { ip = argv[++i]; }
}

if (username.empty() || ip.empty())
{
    cerr << "Usage: ./myprogram -u <username> -ip <server>" << std::endl;
    return 1;
}

    std::cout << "Username: " << username << ", Server IP: " << ip << std::endl;
    return 0;
}