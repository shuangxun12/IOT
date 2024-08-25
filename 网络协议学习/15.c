#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define N 128
#define ERR_LOG(errmsg) do{\
                            perror(errmsg);\
                            exit(1);\
                        }while(0)

void handler(int sig)
{
    wait(NULL);
}

int main(int argc, char const *argv[])
{
    if(argc < 3)
    {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    int sockfd,acceptfd;
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

    if(bind(sockfd, (struct sockaddr *)&serveraddr, addrlen) == -1)
    {
        perror("fail to bind");
        exit(1);
    }
    
    if(listen(sockfd, 10) == -1)
    {
        perror("fail to listen");
        exit(1);
    }
    //处理僵尸进程：
    signal(SIGCHLD, handler);
    while(1)
    {
        struct sockaddr_in clientaddr;
        if((acceptfd = accept(sockfd, (struct sockaddr *)&clientaddr, &addrlen)) == -1)
        {
            ERR_LOG("fail to accept");
        }

        printf("ip: %s, port: %d\n",inet_ntoa(clientaddr.sin_addr), htons(clientaddr.sin_port));
        pid_t pid;
        if((pid = fork()) < 0)
        {
            ERR_LOG("fail to fork");
        }
        else if(pid > 0)
        {
        }
        else
        {
            char buf[N] = "";
            ssize_t bytes;
            while(1)
            {
                if((bytes = recv(acceptfd, buf, N, 0)) == -1)
                {
                    ERR_LOG("fail to recv");
                }
                //判断是否离线（close套接字）
                else if(bytes == 0)
                {
                    printf("ip: %s, port: %d\n",inet_ntoa(clientaddr.sin_addr), htons(clientaddr.sin_port));
                    printf("The client quited\n");
                    exit(0);
                }
                
                printf("ip: %s, port: %d\n",inet_ntoa(clientaddr.sin_addr), htons(clientaddr.sin_port));
                printf("from client: %s\n",buf);

                strcat(buf, "*__-");
                if(send(acceptfd, buf, N, 0) == -1)
                {
                    ERR_LOG("fail to send");
                }
                
            }
        }
    }
    close(acceptfd);
    close(sockfd);
    return 0;
}