#include "header.h"


int main(int args, char *argv[]){
    int number_of_programs;

    if(args>1){
        number_of_programs = atoi(argv[1]); //niebezpieczne ale to chwilowe
    }
    else{
        number_of_programs = 1;
    }

    char exec_program[] = "./tcp_test.x";
    char program_name[] = "tcp_test.x";
    char result_file[] = "results.txt";
    char buff[100];
    char snum[] = " ";
    memset(buff, '\0', 100);
    
    int pids[number_of_programs];
    //char *files[number_of_programs];
    

    for(int i=0; i<number_of_programs; i++){

        switch(pids[i] = fork()){

        case -1:
        perror("fork ");
        exit(1);

        case 0:
        
        snum[0] = i + '0';
        strcat(buff, snum);
        strcat(buff, result_file);
        //files[i] = buff;
        //memcpy(files[i], buff, countBufLen(buff));
        
        printf("Uruchamiam %i program. output file = %s\n", i, buff);
        if(execlp(exec_program, program_name, buff, NULL)!=0){
            printf("zjebalo sie\n");
            exit(1);
        }
        
        

        memset(buff, '\0', 100);

        
        }

    }

    int status = 0;
    for(int i=0;i<number_of_programs;i++)
    {   
        //printf("czekam\n");
        if(waitpid(pids[i], &status, 0) == -1)
            {
                perror( "Wait error" ); /*Jesli blad funkcji wait*/
                exit(EXIT_FAILURE);
            }
    }
    char outfile[100];
    for(int i=0; i<number_of_programs; i++){
        memset(outfile, '\0', 100);
        char diff_command[100] = "diff -s results.txt    ";
        snum[0] = i + '0';
        strcat(outfile, snum);
        strcat(outfile, result_file);
        printf("%s\n", outfile);
        strcat(diff_command, outfile);

        printf("Program %i finished with result: \n", i);

        if(system(diff_command)==-1){
            perror("system() failed\n");
            exit(1);
        }
    }
    
}