#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ether.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netpacket/packet.h>
#include <net/if.h>
#include <strings.h>
#include <arpa/inet.h>

int main(int argc, char const* argv[])
{
    int sockfd;
    if((sockfd= socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1)
    {
        perror("fail to socket");
        exit(1);
    }

    unsigned char msg[1600] = {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // 目的mac地址，这里是arp，所以是用广播地址；
        0x5e, 0xbb, 0xf6, 0x9e, 0xee, 0xfa, // 源mac地址，可以通过ifconfig查询；
        0x08, 0x06, //帧类型，arp报文： 0x0806；

        //arp组报文；
        0x00, 0x01, //硬件类型 以太网：1；
        0x08, 0x00, //协议类型 IP地址：0x0800；
        6, //硬件地址长度；
        4, //协议地址长度；
        0x00, 0x01, //op arp请求：1；
        0x5e, 0xbb, 0xf6, 0x9e, 0xee, 0xfa, //源mac地址；
        192, 168, 1, 104, //源IP地址；
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //目的mac地址
        192, 168, 1, 103, //目的IP地址；


    };
    struct ifreq ethrep;
    strncpy(ethrep.ifr_name, "eth0", IFNAMSIZ);
    if(ioctl(sockfd, SIOCGIFINDEX, &ethrep) == -1)
    {
        perror("fail to icotl");
        exit(1);
    }

    struct sockaddr_ll sll;
    bzero(&sll, sizeof(sll));
    sll.sll_ifindex = ethrep.ifr_ifindex;

    if(sendto(sockfd, msg, 14 + 28, 0, (struct sockaddr*)&sll, sizeof(sll)) < 0)
    {
        perror("fail to sendto");
        exit(1);
    }

    unsigned char recv_msg[1600] = "";
    unsigned char mac[18] = "";
    while(1)
    {
        if(recvfrom(sockfd, recv_msg, sizeof(recv_msg), 0, NULL, NULL) < 0)
        {
            perror("fail to recvfrom");
            exit(1);
        }

        if(ntohs(*(unsigned short*)(recv_msg + 12)) == 0x0806)
        {
            if(ntohs(*(unsigned short*)(recv_msg + 20)) == 2)
            {
                sprintf(mac, "%x:%x:%x:%x:%x:%x", recv_msg[6], recv_msg[7], recv_msg[8], recv_msg[9], recv_msg[10], recv_msg[11]);
                printf("192.168.1.%d --> %s\n", msg[41], mac);
            }
        }
    }

}