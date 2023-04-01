#include "header.h"

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

bool isNumber(char c){
    if((int)c<48 || (int)c>57){ //return true if char is a number
        return false;
    }
    return true;
}

bool isMark(char c){
    return ( (int)c==43 || (int)c==45 ); //return true if char is '+' or '-'
    
}

bool validateInput(char *input, size_t len){ //funkcja sprawdza poprawnosc wprowadzonego ciagu znakow
    size_t startIx = 0;
    size_t endIx = 1;

    if(len==0){
        return false;
    }

    if(!isNumber(input[0]) || !isNumber(input[len-1])){ //jesli pierwszy i ostatni znak nie sa cyframi - false
        return false;
    }

    for(size_t i=1; i<len-1; i++){
        
        if(isNumber(input[i])){
            endIx++;
            continue;
        }
        else if(isMark(input[i])){

            int diff = endIx-startIx;

            if(diff==0){ //jesli dlugosc liczby wynosi 0 to false (case '+-' na przyklad)
                return false;
            }

            startIx=i+1;
            endIx = startIx;

            continue;

        }
        else{
            return false; //kazdy inny znak skutkuje false
        }
    }

    return true;
}


bool parseInt(const char *str, int *n) //funkcja zamienia ciag znakow na liczbe typu int uzywajac funkcji strtol
{
    char *temp;
    errno = 0;
    long val = strtol(str, &temp, 10); //10 means decimal format;

    if (temp == str || *temp != '\0' || ((val == LONG_MIN || val == LONG_MAX) && errno == ERANGE)){
        return false;
    }
    else if(val > INT_MAX || val < INT_MIN){
        return false;
    }
    
    *n = (int) val; //rzutuj long na int i zapisz w n

    return true;
}

//funkcja dzieli lancuch na kawalki, sprawdza czy moze bezpiecznie zapisac liczby w int16_t
//oraz oblicza wynik, jesli wystapil blad zmienia wartosc err na -1
int count(char *input, int len, int *err){ 

    size_t startIx = 0, endIx = 0;
    //liczba liczb w wiadomosci nie bedzie wieksza niz polowa dlugosci +1, poniewaz sa rozdzielone operatorami
    int nums_max = (len/2)+1; 
    int marks_max = (len/2)+1;
    int16_t nums[nums_max]; //tablica w ktorej zapisywane beda liczby w typie int16_t
    char marks[marks_max]; //tablica w ktorej zapisywane beda znaki operatory
    size_t nums_size = 0, marks_size = 0;
    char *snum = malloc(len); //tablica w w ktorej zapisywane beda liczby w formie znakow
    memset(snum, '\0', len); //wyczysc tablice

    for(size_t i = 0; i<len; i++){
         
        if(isMark(input[i])){
            
            int diff = endIx-startIx;

            if(diff>9){ 
                *err = -1;
                return -1;
            }
            //jesli liczba ma wiecej niz 9 cyfr to moze i zmiescila by sie przy konwersji na int
            //ale na pewno nie zmiescilaby sie w int16_t, wiec nie ma sensu dalej liczyc

            int m = 0;

            for(size_t j = startIx; j!=endIx; j++){
                snum[m] = input[j];
                m++;
            }

            int n = 0;
            if(!parseInt(snum, &n)){
                fprintf(stderr, "%s", "parseInt error");
                *err = -1;
                return -1;
            }

            if(n>INT16_MAX || n<INT16_MIN){
                *err = -1;
                return -1;
            }
            //liczba nie zmiesci sie w typie int16_t, zwroc blad

            nums[nums_size] = (int16_t)n;
            nums_size++;
            marks[marks_size] = input[i];
            marks_size++;

            startIx=i+1;
            endIx = startIx;
            memset(snum, '\0', diff); //wyczysc uzywanana tablice

            continue;
        }

        endIx++;
    }

    int diff = endIx-startIx;

        if(diff>9){ 
            *err = -1;
            return -1;
        }
        //jesli liczba ma wiecej niz 9 cyfr to moze i zmiescila by sie przy konwersji na int
        //ale na pewno nie zmiescilaby sie w int16_t, wiec nie ma sensu dalej liczyc

        memset(snum, '\0', diff);

        int m = 0;

        for(size_t j = startIx; j!=endIx; j++){
            snum[m] = input[j];
            m++;
        }

        int n = 0;
        if(!parseInt(snum, &n)){
            fprintf(stderr, "%s", "parseInt error");
            *err = -1;
            return -1;
        }

        if(n>INT16_MAX || n<INT16_MIN){
            *err = -1;
            return -1;
        }
        //liczba nie zmiesci sie w typie int16_t, zwroc blad

        nums[nums_size] = (int16_t)n;
        nums_size++;

    size_t marksIx = 0;
    int result = nums[0];

    for(size_t i = 1; i<nums_size; i++){
        if(marks[marksIx]=='+'){
            result+=nums[i];
        }
        else{
            result-=nums[i];
        }
        marksIx++;
    }

    if(result>INT16_MAX || result<INT16_MIN){
        *err = -1;
        return -1;
    }
    //wynik nie zmiesci sie w typie int16_t, zwroc blad

    int16_t finalResult = (int16_t) result;

    return finalResult;
}

bool printable_buf(const void * buf, int len){

    char *s = (char*)buf;
    int i=0;

    while(i<len)
    {
        if((int)s[i]<32 || (int)s[i]>123){ //is not printable ASCII
            if((int)s[i]!=9 && (int)s[i]!=10 && (int)s[i]!=13){ //is not \r, \t, \n
            return false;
            }
        }
        i++;
    }

    return true;

}


