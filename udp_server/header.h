#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>

#define UDP_MAX 65535   //maksymalny rozmiar datagramu UDP
#define MSG_SIZE  7    //rozmiar wiadomosci odsylanej do klienta: ERROR lub liczba int16_t - 7 znakow wystarczy
#define READ_SIZE 7    //rozmiar bufora do jakiego zapisywana jest odpowiedz serwera

int count(char *input, int len, int *err);
bool validateInput(char *input, size_t len);
bool isMark(char c);
bool isNumber(char c);
int countBufLen(char *ptr);
bool printable_buf(const void * buf, int len);