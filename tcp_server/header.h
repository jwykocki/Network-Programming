#ifndef HEADER_H
#define HEADER_H
// your code

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700

#define _GNU_SOURCE
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>


// Te dwa pliki nagłówkowe są specyficzne dla Linuksa z biblioteką glibc:
#include <sys/epoll.h>
#include <sys/syscall.h>

#define MAX_EVENTS 100
#define MSG_SIZE 4096
#define READ_SIZE 100

//extern aby uniknac multiple definition

extern char **clients_buf;  //tablica wskaznikow do buforow klientow

/*deskryptor socketa serwera*/
extern int srv_sock; 


void log_printf(const char *fmt, ...);
void log_perror(const char * msg);
void log_error(const char * msg, int errnum);
int accept_verbose(int srv_sock);
int close_verbose(int fd);
ssize_t read_verbose(int fd, void * buf, size_t nbytes);
ssize_t write_verbose(int fd, void * buf, size_t nbytes);
int add_fd_to_epoll(int fd, int epoll_fd);
int remove_fd_from_epoll(int fd, int epoll_fd);
bool isNumber(char c);
bool isMark(char c);
int16_t count(char * data, size_t data_len, int *err);
int countBufLen(char *ptr);
int findEnd(char * data, size_t data_len);
ssize_t read_count_write(int sock, char** buf, ssize_t *curr_bytes);
void epoll_loop(int srv_sock);
int listening_socket_tcp_ipv4(in_port_t port);
void INThandler();

#endif
