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
    //判断是否输入足够的信息：ip 和 端口号
    if(argc < 3)
    {
        fprintf(stderr, "Usage: %s ip port\n", argv[0]);
        exit(1);
    }
    //创建套接字；
    struct sockaddr_in groupcataddr;
    socklen_t addrlen = sizeof(groupcataddr);
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("fail to socket");
        exit(1);
    }

    groupcataddr.sin_family = AF_INET;
    groupcataddr.sin_addr.s_addr = inet_addr(argv[1]);
    groupcataddr.sin_port = htons(atoi(argv[2]));

    char buf[N] = "";
    while(1)
    {
        fgets(buf, sizeof(buf), stdin);
        if(sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&groupcataddr, addrlen) == -1)
        {
            perror("fail to sendto");
            exit(1);
        }
    }

    return 0;
}