// Authors:  Meir Haimov, Adi Vaizman.
// ID's: 207910787, 212555726.
#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

using namespace std;

const int SIZE= 1024 ;

int main(int argc ,char **argv){
    int port = atoi(argv[2]);
    int start= atoi(argv[3]);
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr={0};
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(port);
    connect(fd,(sockaddr*)&server_addr,sizeof(server_addr));
    cout << "Sending data to server: " << argv[3] << " " << argv[4] << endl;
    string send= string(argv[3]) + " " + string(argv[4]);
      if (send.size() != write(fd, send.c_str(), send.size())) {
        cerr << "Error: Failed to send all data to server\n";
        close(fd);
        return 1;
      }
    char buffer[SIZE];
    int howmuch= recv(fd, buffer, SIZE-1, 0);
    if (howmuch> 0) {
        buffer[howmuch] = '\0';
        cout << " The Shortest path is : " << buffer << endl;
    }
    close(fd);
    
    return 0;
}
