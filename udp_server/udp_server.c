/*PROGRAM KLIENTA UDP/IPV4*/

#define _POSIX_C_SOURCE 200809L
#include "header.h"     //deklaracje funkcji uzywanych w programie
//implementacje znajduja sie w pliku functions.c

int lst_sock;   // gniazdko nasłuchujące

void INThandler() //obsluga sygnalu SIGINT
{
    printf("\nINThandler: ");
    int rc = close(lst_sock);
    if (rc == -1) {
        perror("close");
    }else{
        printf(" lst_sock closed\n");
    }
    exit(0);
    
}

int main(int argc, char *argv[])
{
    int rc;
    ssize_t cnt;    // wyniki zwracane przez read() i write() są tego typu

    signal(SIGINT, INThandler); //obsluga CTRL-C

    if(argc!=2)
    {
        printf("Invalid number of arguments.\nargv[1] - UDP port number\n");
        return 1;
    }

    //konwersja nr portu na int
    int port;
    char *end = argv[1];
    port = strtol(argv[1], &end, 0);
    if(port==0)
    {
        perror("strtol");
        return 1;
    }
    printf("port: %i\n", port);
    
    //otwarcie socketa UDP serwera
    lst_sock = socket(AF_INET, SOCK_DGRAM, 0); 
    if (lst_sock == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr = { .s_addr = htonl(INADDR_ANY) },
        .sin_port = htons(port)
    };

    rc = bind(lst_sock, (struct sockaddr *) & addr, sizeof(addr));
    if (rc == -1) {
        perror("bind");
        return 1;
    }


    bool keep_on_handling_clients = true;
    while (keep_on_handling_clients) {

        char buf[UDP_MAX];      //bufor na input klienta o rozmiarze maksymalnym datagramu UDP
        int err = 0;            //zmienna informujaca o bledach konwersji otrzymanych danych
        int16_t result;         //wynik powinien byc zapisany do int16_t
        char msg[MSG_SIZE];     //wiadomosc odsylana do klienta 
        memset(msg, '\0', MSG_SIZE); //wyczysc tablice
        socklen_t len = sizeof(addr);
        int message_size;

        cnt = recvfrom(lst_sock, buf, UDP_MAX, 0, (struct sockaddr *) & addr, &len);

        if(cnt==-1){
            perror("recvfrom error");
            return 1;
        }

        size_t length = countBufLen(buf); //obliczanie realnej dlugosci wiadomosci od klienta

        //sprawdzenie poprawnosci daych oraz obliczenie wyniku
        if(validateInput(buf, length)){
        
            result = count(buf, length, &err); //jesli pojawi sie blad to wartosc err zostanie zmieniona na -1
        } 
        else{
            err = -1;
        }

        if(err==0){
            
            if(sprintf(msg, "%d", result)<0){
                perror("sprintf error");
                return 1;
            } //zapisz wynik do msg
            message_size = countBufLen(msg);
        }
        else{
            char error[] = "ERROR";
            memcpy(msg, error, 6);  //zapisz ERROR do msg
            message_size = countBufLen(msg);
        }

        cnt = sendto(lst_sock, msg, message_size, 0, (const struct sockaddr *) &addr, len);

        if(cnt!=message_size){
            perror("cnt not equal message_size");
            return 1;
        }
        
        if(cnt==-1){
            perror("sendto error");
            return 1;
        }

        memset(buf, '\0', length);
    }

    return 0;
}