#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
using namespace std;

#include "xu.h"
using namespace yazi::serialize;

#define N 128

const char* change(std::vector<char> m_buf);

int main(int argc, char const *argv[])
{   
    DataStream ds;
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("fail to socket");
        exit(1);
    }


    struct sockaddr_in serveraddr2;
    socklen_t addrlen2 = sizeof(serveraddr2);
    serveraddr2.sin_family = AF_INET;
    serveraddr2.sin_addr.s_addr = inet_addr("210.41.99.72");
    serveraddr2.sin_port = htons(9999);


    char buf2[128]= "";
    if(sendto(sockfd,buf2, 128, 0, (struct sockaddr*)&serveraddr2, addrlen2) == -1)
    {
        perror("fail to sendto");
            exit(1);
    }
    close(sockfd);
    L3:return 0;
}

const char* change(std::vector<char> m_buf)
{
    const char* buf3;
	m_buf.emplace_back('\0');
	string str2(m_buf.data()); 
    buf3 = str2.c_str();
    return buf3;
}