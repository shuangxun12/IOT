#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "xu.h"

#define N 128

int main(int argc, char const *argv[])
{   
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("fail to socket");
        exit(1);
    }



    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("172.27.160.90 ");
    serveraddr.sin_port = htons(9999);


    if(bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
    {
        perror("fail to bind");
        exit(1);
    }
    
    //接受数据
    while(1){
        char buf[N] = "";
        struct sockaddr_in clientaddr;
        socklen_t addrlen = sizeof(struct sockaddr_in);
            if(recvfrom(sockfd, buf, N, 0, (struct sockaddr *)&clientaddr, &addrlen) == -1)
            {
                  perror("faill to recvfrom");
                exit(1);
            }

            printf("ip:%s, port:%d\n", inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
            printf("from client: %s\n",buf);

        char buf2[128]= "";
        fgets(buf2 ,128, stdin);
        buf2[strlen(buf2) - 1] = '\0';
        if(sendto(sockfd, buf2, 128, 0, (struct sockaddr*)&clientaddr, addrlen) == -1)
        {
            perror("fail to sendto");
            exit(1);
        }}
    close(sockfd);
 
    return 0;
}