/*Jakub Wykocki - Serwer TCP/IPv4 */
//przykladowa kompilacja: gcc -Wall -pedantic -std=c99 tcp_server.c
#include "header.h"

//struktura danych dla kazdego klienta, zawiera bufor oraz aktualna liczbe bajtow w nim
typedef struct {
    char * buf;
    ssize_t bytes;
}event_data;

event_data *clients;

/*deskryptor socketa serwera*/
int srv_sock; 

// Pomocnicze funkcje do drukowania na ekranie komunikatów uzupełnianych
// o znacznik czasu oraz identyfikatory procesu/wątku. Będą używane do
// raportowania przebiegu pozostałych operacji we-wy.

void log_printf(const char *fmt, ...)
{
    // bufor na przyrostowo budowany komunikat, len mówi ile już znaków ma
    char buf[1024];
    int len = 0;

    struct timespec date_unix;
    struct tm date_human;
    long int usec;
    if (clock_gettime(CLOCK_REALTIME, &date_unix) == -1) {
        perror("clock_gettime");
        return;
    }
    if (localtime_r(&date_unix.tv_sec, &date_human) == NULL) {
        perror("localtime_r");
        return;
    }
    usec = date_unix.tv_nsec / 1000;

    // getpid() i gettid() zawsze wykonują się bezbłędnie
    pid_t pid = getpid();
    pid_t tid = syscall(SYS_gettid);

    len = snprintf(buf, sizeof(buf), "%02i:%02i:%02i.%06li PID=%ji TID=%ji ",
                date_human.tm_hour, date_human.tm_min, date_human.tm_sec,
                usec, (intmax_t) pid, (intmax_t) tid);
    if (len < 0) {
        return;
    }
    va_list ap;
    va_start(ap, fmt);
    int i = vsnprintf(buf + len, sizeof(buf) - len, fmt, ap);
    va_end(ap);
    if (i < 0) {
        return;
    }
    len += i;

    // zamień \0 kończące łańcuch na \n i wyślij całość na stdout, czyli na
    // deskryptor nr 1; 
    buf[len] = '\n';
    if(write(1, buf, len + 1)==-1){
        perror("write in log_printf ");
        exit(1);
    }
}

void log_perror(const char * msg)
{
    log_printf("%s: %s", msg, strerror(errno));
}

void log_error(const char * msg, int errnum)
{
    log_printf("%s: %s", msg, strerror(errnum));
}

int accept_verbose(int srv_sock)
{
    struct sockaddr_in a;
    socklen_t a_len = sizeof(a);

    log_printf("calling accept() on descriptor %i", srv_sock);
    int rv = accept(srv_sock, (struct sockaddr *) &a, &a_len);
    if (rv == -1) {
        log_perror("accept");
    } else {
        char buf[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &a.sin_addr, buf, sizeof(buf)) == NULL) {
            log_perror("inet_ntop");
            strcpy(buf, "???");
        }
        log_printf("new client %s:%u, new descriptor %i",
                        buf, (unsigned int) ntohs(a.sin_port), rv);
    }
    return rv;
}

int close_verbose(int fd)
{
    log_printf("closing descriptor %i", fd);
    int rv = close(fd);
    if (rv == -1) {
        log_perror("close");
    }
    return rv;
}

ssize_t read_verbose(int fd, void * buf, size_t nbytes)
{
    log_printf("calling read() on descriptor %i", fd);
    ssize_t rv = read(fd, buf, nbytes);
    if (rv == -1) {
        log_perror("read");
    } else {
        log_printf("received %zi bytes on descriptor %i", rv, fd);
    }
    return rv;
}

ssize_t write_verbose(int fd, void * buf, size_t nbytes)
{
    log_printf("calling write() on descriptor %i", fd);
    ssize_t rv = write(fd, buf, nbytes);
    if (rv == -1) {
        log_perror("write");
    } else if (rv < nbytes) {
        log_printf("partial write on %i, wrote only %zi of %zu bytes",
                        fd, rv, nbytes);
    } else {
        log_printf("wrote %zi bytes on descriptor %i", rv, fd);
    }
    return rv;
}

int add_fd_to_epoll(int fd, int epoll_fd)
{
    log_printf("adding descriptor %i to epoll instance %i", fd, epoll_fd);
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;    // rodzaj interesujących nas zdarzeń
    ev.data.fd = fd;        // dodatkowe dane, jądro nie zwraca na nie uwagi
    int rv = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
    if (rv == -1) {
        log_perror("epoll_ctl(ADD)");
    }
    return rv;
}

int remove_fd_from_epoll(int fd, int epoll_fd)
{
    log_printf("removing descriptor %i from epoll instance %i", fd, epoll_fd);
    int rv = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    if (rv == -1) {
        log_perror("epoll_ctl(DEL)");
    }
    return rv;
}

bool isNumber(char c){
    if((int)c<48 || (int)c>57){ //return true if char is a number
        return false;
    }
    return true;
}

bool isMark(char c){
    return ( (int)c==43 || (int)c==45 ); //return true if char is '+' or '-'
    
}

int16_t count(char * data, size_t data_len, int *err) //funkcja liczaca wyrazenie
{
    
    char * p = data; //poczatek
    char * e = data + data_len; //koniec
    int currentNumber = 0;
    int currentResult = 0;
    char prevMark = '+';
    bool prevCharWasMark = false;

    for (; p<e; ++p){
        if(isNumber(*p)){

            currentNumber *= 10;
            currentNumber += *p - '0';

            if(currentNumber>INT16_MAX){
                *err = -1;
                return -1;
            }

            prevCharWasMark = false;
        
        }else if(isMark(*p)){

            if(prevCharWasMark){
                *err = -1;
                return -1;
            }

            if(prevMark == '+'){ //+
                currentResult+=currentNumber;
            }else if(prevMark == '-'){ //-
                currentResult-=currentNumber;
            }

            if(currentResult>INT16_MAX || currentResult<INT16_MIN){
                *err = -1;
                return -1;
            }

            currentNumber = 0;
            prevMark = *p;
            prevCharWasMark = true;
        }else{
            *err = -1;
            return -1;
        }
    } //koniec petli, nalezy zsumowac z ostatnia liczba

    if(prevCharWasMark){
        *err = -1;
        return -1;
    }

    if(prevMark == '+'){ //+
        currentResult+=currentNumber;
    }else if(prevMark == '-'){ //-
        currentResult-=currentNumber;
    }

    if(currentResult>INT16_MAX || currentResult<INT16_MIN){
        *err = -1;
        return -1;
    }

    int16_t result = (int16_t) currentResult;
    return result;

}

int countBufLen(char *ptr)
{
    int offset = 0;
    int count = 0;

    while (*(ptr + offset) != '\0')
    {
        ++count;
        ++offset;
    }

    return count;
}

int findEnd(char * data, size_t data_len){
    int ix = 0;
    while(ix<data_len){
        if(data[ix]=='\r' && data[ix+1]=='\n'){
            return ix;
        }
        ix++;
    }
    return -1;
}

ssize_t read_count_write(int sock, char* buf, ssize_t *curr_bytes)
{
    ssize_t current_bytes = *curr_bytes;
    
    char msg[MSG_SIZE];
    memset(msg, '\0', MSG_SIZE);
    char goodmsg[10];
    memset(goodmsg, '\0', 10);
    char *data = (char *) malloc(MSG_SIZE);
    memset(data, '\0', MSG_SIZE);
    char *dataf = data;
    char error[] = "ERROR\r\n";

    int16_t result = INT16_MIN; //to ignore unavailable warning
    int err = 0;
    size_t data_len = 0;
    ssize_t bytes_read;

        bytes_read = read_verbose(sock, msg, sizeof(msg));
        if(bytes_read < 0) {
            return -1;
        }
 
        if(bytes_read==0){ //klient nie bedzie juz wysylal danych      
            if(current_bytes>0){
                strcat(data, error);
                data_len += countBufLen(error);
                while (data_len > 0) {
                    ssize_t bytes_written = write_verbose(sock, data, data_len);
                    if (bytes_written < 0) {
                        return -1;
                    }
                    data = data + bytes_written;
                    data_len = data_len - bytes_written;
                }
                (buf)+=current_bytes;
                free(dataf);
                return bytes_read;
            }
        }

        //sprawdza bardzo pesymistyczna sytuacje, gdy musielibysmy wszystkie
        //przeczytane bajty zapisac do bufora, a nie bedzie tam juz miejsca
        if(countBufLen(buf) + bytes_read >= MSG_SIZE){
            log_printf("message size over MSG_SIZE");
            return -1;
        } 
        strcat(buf, msg);
        current_bytes += bytes_read;
        int end; //\r\n

        do{
            err = 0;
            end = findEnd(buf, current_bytes);

            if(end!=-1){

                result = count(buf, end, &err);
                const char *coursor = buf; //to move pointer of \r\n
                coursor += (end + 2);
                strcpy(buf, coursor);
                current_bytes-=(end+2);

                if(err == 0){ 
                    if(sprintf(goodmsg, "%d\r\n", result)<0){ //dwuznak \r\n jest dopisywany do wiadomosci zwrotnej   
                        log_printf("sprintf error"); 
                        return -1;
                    } //zapisz wynik do msg
                    strcat(data, goodmsg);
                    data_len += countBufLen(goodmsg);
                }else{ //wiadomosc byla niepoprawna            
                    strcat(data, error);
                    data_len += countBufLen(error);
                }

                while (data_len > 0) {
                    ssize_t bytes_written = write_verbose(sock, data, data_len);
                    if (bytes_written < 0) {
                        return -1;
                    }
                    data = data + bytes_written;
                    data_len = data_len - bytes_written;
                }
            }
        }while(end!=-1);

    *curr_bytes = current_bytes;
    free(dataf); //zwalnianie pamieci
    return bytes_read;
}


void epoll_loop(int srv_sock)
{
    int clients_max = CLIENTS_MAX;
    clients = malloc(clients_max * sizeof(event_data));

    int epoll_fd = epoll_create(10);
    if (epoll_fd == -1) {
        log_perror("epoll_create");
        return;
    }
    log_printf("epoll instance created, descriptor %i", epoll_fd);

    if (add_fd_to_epoll(srv_sock, epoll_fd) == -1) {
        goto cleanup_epoll;
    }

    while (true) {

        log_printf("calling epoll()");
        struct epoll_event events[MAX_EVENTS];
        // timeout równy -1 oznacza czekanie w nieskończoność
        int num = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num == -1) {
            log_perror("epoll");
            break;
        }
        // epoll wypełniła num początkowych elementów tablicy events
        log_printf("number of events = %i", num);

        for (int i = 0; i < num; ++i) {
            int fd = events[i].data.fd;
            // typ zgłoszonego zdarzenia powinien zawierać "gotów do odczytu"
            if ((events[i].events & EPOLLIN) == 0) {
                // niby nigdy nie powinno się zdarzyć, ale...
                log_printf("descriptor %i isn't ready to read", fd);
                continue;
            }
            if (fd == srv_sock) { 
                int s = accept_verbose(srv_sock);
                // s - deskryptor klienta !
                if (s == -1) {
                    goto cleanup_epoll;
                }
                if (add_fd_to_epoll(s, epoll_fd) == -1) {
                    goto cleanup_epoll;
                }else{ //przyszedl nowy klient, nie bylo erroru
    
                    char *buf = malloc(MSG_SIZE);
                    event_data ed = {buf, 0};         
                    if(s>clients_max-6){ //deskryptory kleintow zaczynaja sie od 5 indeksu
                        clients_max*=2;
                        clients = realloc(clients, clients_max * sizeof(event_data));
                        log_printf("clients_max size resized to %i", clients_max);
                    }
                    else if(s<clients_max/2){ //deskryptory kleintow zaczynaja sie od 5 indeksu
                        clients_max/=2;
                        clients = realloc(clients, clients_max * sizeof(event_data));
                        log_printf("clients_max size resized to %i", clients_max);
                    }

                    clients[s] = ed;
                }
            } else {    // fd != srv_sock
                //fd = deskryptor klienta
                if (read_count_write(fd, clients[fd].buf, &clients[fd].bytes) <= 0) {
                    // druga strona zamknęła połączenie lub wystąpił błąd
                    remove_fd_from_epoll(fd, epoll_fd);
                    free(clients[fd].buf);
                    close_verbose(fd);
                }
            }
        }
    }

cleanup_epoll:
    
    close_verbose(epoll_fd);

    // W tym miejscu należałoby zamknąć otwarte połączenia z klientami, ale
    // nie dysponujemy żadną listą ani zbiorem z numerami ich deskryptorów.
}
// Standardowa procedura tworząca nasłuchujące gniazdko TCP.

int listening_socket_tcp_ipv4(in_port_t port)
{
    int s;

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in a = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(port)
    };

    if (bind(s, (struct sockaddr *) &a, sizeof(a)) == -1) {
        perror("bind");
        goto close_and_fail;
    }

    if (listen(s, 10) == -1) {
        perror("listen");
        goto close_and_fail;
    }

    return s;

close_and_fail:
    close(s);
    return -1;
}



void INThandler() //obsluga sygnalu SIGINT
{
    free(clients);
    log_printf("INThandler");
    log_printf("main loop done");
    int rc = close(srv_sock);
    if (rc == -1) {
        log_perror("close");
        exit(1);
    }else{
        log_printf("srv_sock: closed");
    }
    exit(0);
}
