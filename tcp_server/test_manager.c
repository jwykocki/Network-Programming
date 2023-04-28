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
    memset(buff, '\0', 100);
    
    int pids[number_of_programs];
 
    for(int i=0; i<number_of_programs; i++){

        switch(pids[i] = fork()){

        case -1:
        perror("fork ");
        exit(1);

        case 0:
        sprintf(buff, "%i%s", i, result_file);
        printf("Uruchamiam %i program. output file = %s\n", i, buff);
     
        if(execlp(exec_program, program_name, buff, NULL)!=0){
            exit(1);
        }
        memset(buff, '\0', 100);   
        }
    }

    int status = 0;
    for(int i=0;i<number_of_programs;i++)
    {   
        if(waitpid(pids[i], &status, 0) == -1)
            {
                perror( "Wait error" ); /*Jesli blad funkcji wait*/
                exit(EXIT_FAILURE);
            }
    }
    char outfile[100];
    for(int i=0; i<number_of_programs; i++){
        memset(outfile, '\0', 100);
        char diff_command[100] = "diff -s expected_results.txt    ";

        sprintf(outfile, "%i%s", i, result_file);
        strcat(diff_command, outfile);

        printf("Program %i finished with result: \n", i);
    
        if(system(diff_command)==-1){
            perror("system() failed\n");
            exit(1);
        }
    }
    
}