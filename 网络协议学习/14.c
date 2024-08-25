#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define N 128

int main(int argc, char const *argv[])
{
    if(argc < 3)
    {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("fail to socket");
        exit(1);
    }
    
    struct sockaddr_in serveraddr;
    socklen_t addrlen = sizeof(serveraddr);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
    serveraddr.sin_port = ntohs(atoi(argv[2]));
    //相较于UDP，TCP的发送端多了一步connect：
    //同时，因为建立了connect，TCP的send和recv是不需要传入服务器段的IP和端口信息的；
    if(connect(sockfd, (struct sockaddr *)&serveraddr, addrlen) == -1)
    {
        perror("fail to connect");
        exit(1);
    }

    while(1)
    {
        char buf[N] = "";
        fgets(buf, N, stdin);
        buf[sizeof(buf) - 1] = '\0';
        if(send(sockfd, buf, N, 0) == -1)
        {
            perror("fail to sendto");
            exit(1);
        }

        char text[N] = "";
        if(recv(sockfd, text, N, 0) == -1)
        {
            perror("fail to recv");
            exit(1);
        }

        printf("from server: %s\n", text);
    }


    return 0;
}