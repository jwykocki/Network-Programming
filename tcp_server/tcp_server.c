#include "header.h"

int main()
{

    long int srv_port = 2020;
    signal(SIGINT, INThandler); //obsluga CTRL-C
    errno = 0;

    

    if ((srv_sock = listening_socket_tcp_ipv4(srv_port)) == -1) {
        return 1;
    }

    struct timeval timeout;      
    timeout.tv_sec = 60; //liczba sekund na timeout
    timeout.tv_usec = 0; //trzeba zainicjalizowac zeby nie bylo smieci

    
    if (setsockopt (srv_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout) < 0){ 
        perror("setsockopt");
        return 1;
    }

    log_printf("starting main loop");
    epoll_loop(srv_sock);
    log_printf("main loop done");

    if (close(srv_sock) == -1) {
        log_perror("close");
        return 1;
    }

    return 0;
}
