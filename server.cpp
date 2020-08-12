#include <unistd.h>
#include<iostream>
#include<winsock2.h>
#include<vector>
using namespace std;
vector<string> split(string str)
{
    vector<string> v;
    string s = "";
    int i = 0;
    while(str[i] != '\0')
    {
        if(str[i] == ' ')
        {
            v.push_back(s);
            s = "";
            for(int j = i+1; j != str.length(); j++)
                s += str[j];
            break;
        }
        else
        {
            s += str[i];
        }
        i++;
    }
    v.push_back(s);
    return v;
}
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
class User
{
private:
    string username;
    string password;
    vector<string> allMsg;
    vector<string> newMsg;
public:
    User(string username, string password)
    {
        this->username = username;
        this->password = password;
    }
    void setUsername(string username){this->username = username;}
    string getUsername(){return this->username;}
    void setPassword(string password){this->password = password;}
    string getPassword(){return this->password;}
    void addAll(string str){this->allMsg.push_back(str);}
    vector<string> getAll(){return this->allMsg;}
    void addNew(string str){this->newMsg.push_back(str);}
    vector<string> getNew(){return this->newMsg;}
    void readMsg(){this->newMsg.clear();}
};
int main()
{
    vector<User> u; // vector containing users' information
    int iResult = 0; // error handling
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN serverAddr;
    // initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,0), &WSAData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
    // create a SOCKET for connecting to server
    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    // set arguments
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5555);
    // setup the TCP listening socket
    iResult = bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
    if(iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        closesocket(server);
        WSACleanup();
        return 1;
    }
    while(true)
    {
        SOCKET client;
        SOCKADDR_IN clientAddr;
        // listen
        iResult = listen(server, 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("listen failed with error: %d\n", WSAGetLastError());
            closesocket(server);
            WSACleanup();
            return 1;
        }
        cout << "Listening for incoming connections..." << endl;
        // define data for income and outcome messaging
        Payload * p = new Payload[1];
        char i[1024];// input message
        char o[1024];// output message
        int clientAddrSize = sizeof(clientAddr);
        int state;// userPass 0, wrongPass 1, register 2, loggedIn 3, chooseMode 4, cmd 5, logOut 6, messageMode 7
                //   send 8, wrongReceiver 9, receive 10, showUnread 11
        int n = 0;// keep track of user
        int k = 0;// iterator
        vector<string> v;
        if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
        {
            cout << "Client connected!" << endl;
            state = 0; // userPass
            while(true) // always
            {
                if(state == 0 || state == 1)// userPass or wrongPass
                {
                    strcpy(o, "userPass");
                    send(client, o, sizeof(o), 0);
                    memset(o, 0, sizeof(o));
                    memset(i, 0, sizeof(i));
                    recv(client, i, sizeof(i), 0);// ack
                    if(state == 0)//userPass
                        strcpy(o, "Welcome to the server!\nEnter username: ");
                    else
                        strcpy(o, "Wrong password!\nEnter username: ");
                    send(client, o, sizeof(o), 0);
                    memset(o, 0, sizeof(o));
                    memset(i, 0, sizeof(i));
                    recv(client, i, sizeof(i), 0);
                    v = split(i);
                    strcpy(o, "Enter password: ");
                    send(client, o, sizeof(o), 0);
                    memset(o, 0, sizeof(o));
                    memset(i, 0, sizeof(i));
                    recv(client, i, sizeof(i), 0);
                    v.push_back(i);
                    state = 0;
                    for(n = 0; n != u.size(); n++)
                    {
                        if(v[0] == u[n].getUsername())
                        {
                            if(v[1] == u[n].getPassword())
                            {
                                state = 3;// loggedIn
                                break;
                            }
                            else
                            {
                                state = 1;// wrongPass
                                break;
                            }
                        }
                    }
                    if(state == 0)// new user
                        state = 2;// register
                }
                else if(state == 2)// register
                {
                    User tmpUser(v[0], v[1]);
                    u.push_back(tmpUser);
                    state = 3;// loggedIn
                }
                else if(state == 3)// loggedIn
                {
                    strcpy(o, "loggedIn");
                    send(client, o, sizeof(o), 0);
                    memset(o, 0, sizeof(o));
                    memset(i, 0, sizeof(i));
                    recv(client, i, sizeof(i), 0);// ack
                    strcpy(o, "Successfully logged in!\n");
                    send(client, o, sizeof(o), 0);
                    memset(o, 0, sizeof(o));
                    memset(i, 0, sizeof(i));
                    recv(client, i, sizeof(i), 0);// ack
                    state = 4;// chooseMode
                }
                else if(state == 4)// chooseMode
                {
                    strcpy(o, "chooseMode");
                    send(client, o, sizeof(o), 0);
                    memset(o, 0, sizeof(o));
                    memset(i, 0, sizeof(i));
                    recv(client, i, sizeof(i), 0);// ack
                    strcpy(o, "Choose mode:\n1.CMD 2.Log Out 3.Message Mode\n");
                    send(client, o, sizeof(o), 0);
                    memset(o, 0, sizeof(o));
                    memset(i, 0, sizeof(i));
                    recv(client, i, sizeof(i), 0);
                    v = split(i);
                    if(v[0] == "1")
                        state = 5;// cmd
                    else if(v[0] == "2")// logOut
                        state = 6;// exit
                    else if(v[0] == "3")
                        state = 7;// messageMode
                }
                else if(state == 5)// cmd
                {
                    strcpy(o, "cmd");
                    send(client, o, sizeof(o), 0);
                    memset(o, 0, sizeof(o));
                    memset(i, 0, sizeof(i));
                    recv(client, i, sizeof(i), 0);// ack
                    strcpy(o, "Type your system command or 'exit' to return to mode choosing:\n");
                    send(client, o, sizeof(o), 0);
                    memset(o, 0, sizeof(o));
                    memset(i, 0, sizeof(i));
                    recv(client, i, sizeof(i), 0);
                    v = split(i);
                    if(v[0] == "exit")
                        state = 4; // chooseMode
                    else// system command
                    {
                        //int result = system(i);
                        //strcpy(o, ("Result: " + to_string(result) + "\n").c_str());
                        strcpy(o, "ack");
                        send(client, o, sizeof(o), 0);// dummy
                        memset(o, 0, sizeof(o));
                        memset(i, 0, sizeof(i));
                        recv(client, i, sizeof(i), 0);// ack
                    }
                }
                else if(state == 6)// logOut
                {
                    strcpy(o, "logOut");
                    send(client, o, sizeof(o), 0);
                    memset(o, 0, sizeof(o));
                    break; // go to close socket
                }
                else if(state == 7)// messageMode
                {
                    strcpy(o, "messageMode");
                    send(client, o, sizeof(o), 0);
                    memset(o, 0, sizeof(o));
                    memset(i, 0, sizeof(i));
                    recv(client, i, sizeof(i), 0);// ack
                    strcpy(o, "Choose message option or 'exit' to return to mode choosing:\n1.Send 2.Receive 3.Show unread\n");
                    send(client, o, sizeof(o), 0);
                    memset(o, 0, sizeof(o));
                    memset(i, 0, sizeof(i));
                    recv(client, i, sizeof(i), 0);
                    v = split(i);
                    if(v[0] == "1")
                        state = 8;// send
                    else if(v[0] == "2")
                        state = 10;// receive
                    else if(v[0] == "3")
                        state = 11;// showUnread
                    else if(v[0] == "exit")
                        state = 4;// chooseMode
                }
                else if(state == 8 || state == 9)// send or wrongReceiver
                {
                    strcpy(o, "send");
                    send(client, o, sizeof(o), 0);
                    memset(o, 0, sizeof(o));
                    memset(i, 0, sizeof(i));
                    recv(client, i, sizeof(i), 0);// ack
                    strcpy(o, "Send message in the following format or 'exit' to return to message options:\n'%receiver username% %body%'\n");
                    send(client, o, sizeof(o), 0);
                    memset(o, 0, sizeof(o));
                    memset(i, 0, sizeof(i));
                    recv(client, i, sizeof(i), 0);
                    v = split(i);
                    state = 9; // wrongReceiver
                    if(v[0] == "exit")
                    {
                        state = 7; // messageMode
                        continue;
                    }
                    for(k = 0; k != u.size(); k++)
                    {
                        if(v[0] == u[k].getUsername())
                        {
                            string msg = "";
                            u[k].addAll(u[n].getUsername() + ": " + v[1]);
                            u[k].addNew(u[n].getUsername() + ": " + v[1]);
                            strcpy(o, "Message sent.\n");
                            send(client, o, sizeof(o), 0);
                            memset(o, 0, sizeof(o));
                            memset(i, 0, sizeof(i));
                            recv(client, i, sizeof(i), 0); // ack
                            state = 8; // still send
                            break;
                        }
                    }
                    if(state == 9) // wrongReceiver
                    {
                        strcpy(o, "Wrong receiver username entered.\n");
                        send(client, o, sizeof(o), 0);
                        memset(o, 0, sizeof(o));
                        memset(i, 0, sizeof(i));
                        recv(client, i, sizeof(i), 0); // ack
                    }
                }
                else if(state == 10 || state == 11)// receive or showUnread
                {
                    strcpy(o, "receive");
                    send(client, o, sizeof(o), 0);
                    memset(o, 0, sizeof(o));
                    memset(i, 0, sizeof(i));
                    recv(client, i, sizeof(i), 0);// ack
                    if(state == 10)
                        strcpy(o, "All received messages:\n");
                    else
                        strcpy(o, "Unread messages:\n");
                    send(client, o, sizeof(o), 0);
                    memset(o, 0, sizeof(o));
                    memset(i, 0, sizeof(i));
                    recv(client, i, sizeof(i), 0);// ack
                    int num = 0;
                    if(state == 10)
                        num = u[n].getAll().size();
                    else
                        num = u[n].getNew().size();
                    string s = to_string(num);
                    strcpy(o, s.c_str());
                    send(client, o, sizeof(o), 0);
                    memset(o, 0, sizeof(o));
                    memset(i, 0, sizeof(i));
                    recv(client, i, sizeof(i), 0);// ack
                    for(int c = 0; c < num; c++)
                    {
                        strcpy(o, (u[n].getAll())[c].c_str());
                        send(client, o, sizeof(o), 0);
                        memset(o, 0, sizeof(o));
                        memset(i, 0, sizeof(i));
                        recv(client, i, sizeof(i), 0);// ack
                    }
                    u[n].readMsg();
                    state = 7;// messageMode
                }
            }
            closesocket(client);
            cout << "Client disconnected." << endl;
        }
    }
}
