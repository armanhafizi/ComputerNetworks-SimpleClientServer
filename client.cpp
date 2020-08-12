#include <iostream>
#include <unistd.h>
#include <winsock2.h>
using namespace std;
class Payload
{
private:
    string type;
    string data;
public:
    Payload()
    {
        this->type = "";
        this->data = "";
    }
    Payload(string type, string data)
    {
        this->type = type;
        this->data = data;
    }
    void setType(string type){this->type = type;}
    string getType(){return this->type;}
    void setData(string data){this->data = data;}
    string getData(){return this->data;}
};
int main()
{
    while(true)// always connect a new client
    {
        int iResult = 0; // error handling
        WSADATA WSAData;
        SOCKET server;
        SOCKADDR_IN addr;
        // initialize Winsock
        iResult = WSAStartup(MAKEWORD(2,0), &WSAData);
        if (iResult != 0)
        {
            printf("WSAStartup failed with error: %d\n", iResult);
            return 1;
        }
        // create a SOCKET for connecting to server
        server = socket(AF_INET, SOCK_STREAM, 0);
        if(server == INVALID_SOCKET)
        {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }
        // set arguments
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        addr.sin_family = AF_INET;
        addr.sin_port = htons(5555);
        // connect to server.
        connect(server, (SOCKADDR *)&addr, sizeof(addr));
        cout << "Connected to the server." << endl;
        // define data
        Payload * pt = new Payload[1];
        char i[1024]; // input message
        char o[1024]; // output message
        int state;// receive initial message 0
        string u, p; // username and password
        state = 0; // state of the program
        cin.clear();
        while(true) // always
        {
            // receive the buffer
            memset(i, 0, sizeof(i));
            recv(server, i, sizeof(i), 0);
            string strBuff(i);
            if(strBuff == "userPass") // send username and password
            {
                strcpy(o, "ack");
                send(server, o, sizeof(o), 0);
                memset(o, 0, sizeof(o));
                memset(i, 0, sizeof(i));
                recv(server, i, sizeof(i), 0);
                cout << i;
                getline(cin, u);
                strcpy(o, u.c_str());
                send(server, o, sizeof(o), 0);
                memset(o, 0, sizeof(o));
                memset(i, 0, sizeof(i));
                recv(server, i, sizeof(i), 0);
                cout << i;
                getline(cin, p);
                strcpy(o, p.c_str());
                send(server, o, sizeof(o), 0);
                memset(o, 0, sizeof(o));
            }
            else if (strBuff == "loggedIn") // client logged in
            {
                strcpy(o, "ack");
                send(server, o, sizeof(o), 0);
                memset(i, 0, sizeof(i));
                recv(server, i, sizeof(i), 0);
                cout << i;
                strcpy(o, "ack");
                send(server, o, sizeof(o), 0);
                memset(o, 0, sizeof(o));
            }
            else if(strBuff == "chooseMode" || strBuff == "messageMode") // choose mode or message mode
            {
                strcpy(o, "ack");
                send(server, o, sizeof(o), 0);
                memset(i, 0, sizeof(i));
                recv(server, i, sizeof(i), 0);
                cout << i;
                string tmp;
                getline(cin, tmp);
                strcpy(o, tmp.c_str());
                send(server, o, sizeof(o), 0);
                memset(i, 0, sizeof(i));
            }
            else if(strBuff == "cmd") // system command
            {
                strcpy(o, "ack");
                send(server, o, sizeof(o), 0);
                memset(i, 0, sizeof(i));
                recv(server, i, sizeof(i), 0);
                cout << i;
                string tmp;
                getline(cin, tmp);
                strcpy(o, tmp.c_str());
                send(server, o, sizeof(o), 0);
                memset(i, 0, sizeof(i));
                if(tmp != "exit") // print system call's result
                {
                    system(tmp.c_str()); // in-client system call
                    memset(i, 0, sizeof(i));
                    recv(server, i, sizeof(i), 0);// dummy
                    strcpy(o, "ack");
                    send(server, o, sizeof(o), 0);
                }
            }
            else if(strBuff == "send") // send message
            {
                strcpy(o, "ack");
                send(server, o, sizeof(o), 0);
                memset(i, 0, sizeof(i));
                recv(server, i, sizeof(i), 0);
                cout << i;
                string tmp;
                getline(cin, tmp);
                strcpy(o, tmp.c_str());
                send(server, o, sizeof(o), 0);
                memset(i, 0, sizeof(i));
                if(tmp != "exit")
                {
                    memset(i, 0, sizeof(i));
                    recv(server, i, sizeof(i), 0);
                    cout << i;
                    strcpy(o, "ack");
                    send(server, o, sizeof(o), 0);
                }
            }
            else if(strBuff == "receive") // see received messages
            {
                strcpy(o, "ack");
                send(server, o, sizeof(o), 0);
                memset(i, 0, sizeof(i));
                recv(server, i, sizeof(i), 0);
                cout << i;
                strcpy(o, "ack");
                send(server, o, sizeof(o), 0);
                memset(i, 0, sizeof(i));
                recv(server, i, sizeof(i), 0);
                strcpy(o, "ack");
                send(server, o, sizeof(o), 0);
                string s(i);
                int num = stoi(s);
                for(int c = 0; c != num; c++)
                {
                    memset(i, 0, sizeof(i));
                    recv(server, i, sizeof(i), 0);
                    cout << i << endl;
                    strcpy(o, "ack");
                    send(server, o, sizeof(o), 0);
                }
            }
            else if(strBuff == "logOut")
            {
                break; // go to close socket
            }
        }
        cout << "Successfully logged out.\n********************************************\n";
        // wait 3s
        Sleep(3000);
        closesocket(server);
        WSACleanup();
        //cout << "Socket closed." << endl;
    }
    return 0;
}
