/*PROGRAM KLIENTA-TESTERA TCP/IPV4*/

#include "header.h"

void remove_file(char * filename){
    char remove_command[100];
    char rm[] = "rm ";
    strcat(remove_command, rm);
    strcat(remove_command, filename);
    if(system(remove_command)<0){
        perror("system()");
        exit(1);
    }
}

int main(int args, char *argv[])
{
    int sock;           //socket klienta
    int rc;             // "rc" to skrót słów "result code"
    ssize_t cnt;        // wyniki zwracane przez read() i write() są tego typu
    uint32_t address;   //skonwertowany adres ip serwera
    int port = 2020;           //numer portu serwera
    int w;
    char file_name[100];
    char default_file_name[] = "results.txt";

    if(args>1){
        //memcpy(file_name, argv[1], countBufLen(argv[1]));
        strcpy(file_name, argv[1]);

    }
    else{
        memcpy(file_name, default_file_name, countBufLen(default_file_name));
    }


    FILE *inputfile = fopen("test.txt", "r");
    remove_file(file_name);

    int outfd  = open(file_name, O_WRONLY | O_CREAT, 0644);

    if(outfd==-1){
        perror("open error");
        return 1;
    }

    address = inet_addr("127.0.0.1");

    if(address==-1){
        perror("inet_addr");
        return 1;
    }


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

    int sleeper = rand() % 5;
        sleep(sleeper);
        

    rc = connect(sock, (struct sockaddr *) & addr, sizeof(addr));
    if (rc == -1) {
        perror("connect");
        return 1;
    }

    char buf[READ_SIZE];
    char msg[100];

    while(1){ //czytaj z pliku

        int sleeper = rand() % 5;
        sleep(sleeper);
        memset(msg, '\0', 100);
        

        if(fgets(msg, 100, inputfile)==NULL){
            return 0;
        }

        size_t size = countBufLen(msg);
        msg[size-1] = '\r';
        msg[size] = '\n';
        size++;

        cnt = write(sock, msg, size); //wysylanie do serwera
        if (cnt == -1) {
            perror("write");
            return 1;
        }

        struct timeval timeout;      
        timeout.tv_sec = 10; //liczba sekund na timeout
        
        if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout) < 0){
            perror("setsockopt");
            return 1;
        }

        
        memset(buf, '\0', READ_SIZE);
        
        cnt = read(sock, buf, READ_SIZE); //odbieranie danych
        if (cnt == -1) {
            perror("read");
            return 1;
        }

        int len = cnt;
        w = write(outfd, buf, len); 
        
        if(w == -1){
        perror("write");
        return 1;
        }
        
    }

    rc = close(sock);
    if (rc == -1) {
        perror("close");
        return 1;
    }

    rc = close(outfd);
    if (rc == -1) {
        perror("close");
        return 1;
    }

    return 0;
}


