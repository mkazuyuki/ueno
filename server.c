#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8000
#define ECHOSTRING "ImAlive"
#define BUFSIZE 16


int main()
{
    int sock;
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
    int client_addr_len;

    char message[BUFSIZE]=ECHOSTRING;
    char buffer[BUFSIZE];


    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=PF_INET;
    serv_addr.sin_addr.s_addr=INADDR_ANY;
    serv_addr.sin_port=htons(PORT);

    if((sock=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP))<0){
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    if(bind(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))){
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }
    client_addr_len=sizeof(client_addr);
    if(recvfrom(sock, buffer, sizeof(buffer), 0,
                (struct sockaddr *) &client_addr, &client_addr_len)<0){
        perror("recvfrom() failed");
        exit(EXIT_FAILURE);
    }
        printf("%d\n",client_addr_len);
    
    if(sendto(sock, message, sizeof(message),0,
        (struct sockaddr*)&client_addr,client_addr_len)!=sizeof(message)){
        perror("sendto() failed");
        exit(EXIT_FAILURE);
    }
    close(sock);
    return 0;
}
