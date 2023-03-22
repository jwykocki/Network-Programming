/*PROGRAM KLIENTA UDP/IPV4*/

#define _POSIX_C_SOURCE 200809L

#include "header.h"

int main(int args, char *argv[])
{
    int sock;           //socket klienta            
    ssize_t cnt;        // wyniki zwracane przez read() i write() są tego typu
    uint32_t address;   //skonwertowany adres ip serwera
    int port;           //numer portu serwera
    int w, rc;

    if(args!=3)
    {
        printf("Invalid number of arguments.\nargv[1] - IPv4 address\nargv[2] - TCP port number\n");
        return 1;
    }

    address = inet_addr(argv[1]);

    if(address==-1){
        perror("inet_addr");
        return 1;
    }

    char *end = argv[2];
    port = strtol(argv[2], &end, 0);
    if(port==0)
    {
        perror("strtol");
        return 1;
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
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

    char msg[] = "2+3-5"; //dzialanie wysylane do serwera

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
    printf("\n");


    struct timeval timeout;      
    timeout.tv_sec = 10; //liczba sekund na timeout
    
    if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout) < 0){
        perror("setsockopt");
        return 1;
    }

    char buf[READ_SIZE];
    
    cnt = read(sock, buf, READ_SIZE); //odbieranie danych
    if (cnt == -1) {
        perror("read");
        return 1;
    }

    int len = countBufLen(buf);

    if(printable_buf(buf, len)){ //jesli tekst drukowalny to wypisz
        
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


