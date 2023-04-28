/*PROGRAM KLIENTA TCP/IPV4*/

#include "header.h"

int main(int args, char *argv[])
{
    int sock;           //socket klienta            
    ssize_t cnt;        // wyniki zwracane przez read() i write() są tego typu
    uint32_t address;   //skonwertowany adres ip serwera
    int port;           //numer portu serwera
    int w, rc;

    

    address = inet_addr("127.0.0.1");

    if(address==-1){
        perror("inet_addr");
        return 1;
    }

    port = 2020;
    

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr = { .s_addr = address },
        .sin_port = htons(port)
    };

    rc = connect(sock, (struct sockaddr *) & addr, sizeof(addr));
    if (rc == -1) {
        perror("connect");
        return 1;
    }

    char msg[] = "12+3\r\n0-5\r\n6+\r\n2+4\r\n123-"; //dzialanie wysylane do serwera

    size_t size = countBufLen(msg);

    cnt = write(sock, msg, size); //wysylanie do serwera
    if (cnt == -1) {
        perror("write");
        return 1;
    }

    if(write(1, msg, sizeof(msg))==-1){ //wysylanie na std out
        perror("write err");
        return 1;
    }
    
    struct timeval timeout;      
    timeout.tv_sec = 20; //liczba sekund na timeout
    
    if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout) < 0){
        perror("setsockopt");
        return 1;
    }

    if(shutdown( sock, SHUT_WR)==-1){
        perror("shutdown");
        return 1;
    }

    char buf[READ_SIZE];
    memset(buf, '\0', READ_SIZE);
    printf("\ndane odebrane: \n");
    while(1){
    cnt = read(sock, buf, READ_SIZE); //odbieranie danych
    if (cnt == -1) {
        perror("read");
        return 1;
    }

    int len = cnt;

        
    w = write(1, buf, len); 

    if(w == -1){
    perror("write std");
    return 1;
    }
    }

    

    rc = close(sock);
    if (rc == -1) {
        perror("close");
        return 1;
    }

    return 0;
}


