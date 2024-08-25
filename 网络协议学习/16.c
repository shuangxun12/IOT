#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

#define N 128

typedef struct 
{
    struct sockaddr_in addr;
    int acceptfd;
}MSG;

void *pthread_fun(void *arg)
{
    
}

int main(int argc, char const *argv[])
{
    if(argc < 3)
    {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    int sockfd, acceptfd;
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

    if(bind(sockfd, (strcut sockaddr *)&serveraddr, addrlen) == -1)
    {
        perror("fail to bind");
        exit(1);
    }
    
    if(listen(sockfd, 10) == -1)
    {
        perror("fail to listen");
        exit(1);
    }

    while(1)
    {
        struct sockaddr_in clientaddr;
        if((acceptfd = accept(sockfd, (struct sockaddr *)&clientaddr, &addrlen)) == -1)
        {
            perror("fail to accept");
            exit(1);
        }

        //printf("ip: %s, port: %d\n",inet_ntoa(clientaddr.sin_addr), htons(clientaddr.sin_port));
        MSG msg;

        char buf[N] = "";
        if(recv(acceptfd, buf, N, 0) == -1)
        {
            perror("fail to recv");
            exit(1);
        }

        printf("from client: %s\n",buf);

        strcat(buf, "*__-");
        if(send(acceptfd, buf, N, 0) == -1)
        {
            perror("fail to send");
            exit(1);
        }
    }
    close(acceptfd);
    close(sockfd);
    return 0;
}