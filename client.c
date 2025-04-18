#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PEERIP "127.0.0.1"
#define PORT 8000
#define ECHOSTRING "ImAlive"
#define BUFSIZE 16

// void error_handling(char* message)
// {
//     perror(message);
//     exit(EXIT_FAILURE);
// }

int main()
{
    /*変数宣言*/
    int sock;
    struct sockaddr_in serv_addr;
    int serv_addr_len;
    char message[BUFSIZE]=ECHOSTRING;
    char buffer[BUFSIZE];

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(PEERIP);
    serv_addr.sin_port=htons(PORT);

    if((sock=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP))<0){
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }
    
    if(sendto(sock,message, sizeof(message),0,
        (struct sockaddr*)&serv_addr,sizeof(serv_addr))!=sizeof(message)){
        perror("sendto() failed");
        exit(EXIT_FAILURE);
    }
    serv_addr_len=sizeof(serv_addr);
    if(recvfrom(sock,buffer,BUFSIZE,0,
        (struct sockaddr*)&serv_addr,&serv_addr_len)!=sizeof(message)){
        perror("recvfrom() failed");
        exit(EXIT_FAILURE);
    }
    close(sock);
    return 0;
}