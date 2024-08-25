#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ether.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main (int argc, char const *argv[])
{
    int sockfd;
    if((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1)
    {
        perror("fail to sockfd");
    }
    //printf("sockfd = %d\n",sockfd);
    unsigned char msg[1600] = "";
    while(1)
    {
        if(recvfrom(sockfd, msg, sizeof(msg), 0, NULL, NULL) == -1)
        {
            perror("fail to recvfrom\n");
            exit(1);
        }

        unsigned char dst_mac[18] = "";
        unsigned char src_mac[18] = "";
        unsigned short type;
        sprintf(dst_mac, "%x:%x:%x:%x:%x:%x", msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
        sprintf(src_mac, "%x:%x:%x:%x:%x:%x", msg[6], msg[7], msg[8], msg[9], msg[10], msg[11]);
        type = ntohs(*(unsigned short*)(msg + 12));

        printf("源mac: %s --> 目的mac: %s\n", src_mac, dst_mac);
        printf("type = %#x\n",type);
    }
    
    
    close(sockfd);
    return 0;
}
//原始套接字的运行需要管理员权限，在运行时候要加sudo；