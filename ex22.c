// Ye'ela Granot 209133107
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#define BUF_SIZE 150

int has_c_extension(const char* filename) {
    const char* dot = strrchr(filename, '.');
    if (dot && strcmp(dot, ".c") == 0) {
        return 1;
    }
    return 0;
}


int is_directory(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    return 0;
}

int main(int argc, char* argv[]){
    if(argc != 2){
        if(write(2,"wrong arguments number\n",23) < 0){
            exit(-1);
        };
        exit(-1);
    }
    int config = open(argv[1], O_RDONLY);
    if(config < 0){
        if(write(2,"Error in: open\n",15) < 0){
            exit(-1);
        };
        exit(-1);
    }
    int res = open("results.csv", O_WRONLY | O_CREAT | O_TRUNC, 0777);
        if (res < 0) {
            if (write(2, "Error in: open\n", 15) < 0) {
                exit(-1);
            }
            exit(-1);
        }
    char *filename = "errors.txt";
    int fd_errors = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if(fd_errors < 0){
        if(write(2,"Error in: open\n",15) < 0){
            exit(-1);
        } 
        exit(-1);
    }


    char students_path[BUF_SIZE];
    char input_file_path[BUF_SIZE];
    char output_file_path[BUF_SIZE];
    char c;
    int i = 0;
    int j = 0;
    while (read(config, &c, 1) > 0) {
        if (c == '\n') {
            if(j == 0){ // reading the students directory path
                students_path[i] = '\0';
                j++;
                i = 0;
            }
            else if(j == 1){ // reading the input file path
                input_file_path[i] = '\0';
                j++;
                i = 0;
            }
            else{ // reading the output file path
                output_file_path[i] = '\0';
            }
        }
        else {
            if(j == 0){
                students_path[i++] = c;
            }
            else if(j == 1){
                input_file_path[i++] = c;
            }
            else{
                output_file_path[i++] = c;
            }
        }
    }

    
    int correct_output = open(output_file_path, O_RDONLY);
    if(correct_output < 0){
        if(write(2,"Output file not exist\n",22) < 0){
            exit(-1);
        }
        exit(-1);
    }
    if(close(correct_output) < 0){
        if(write(2,"Error in: close\n",16) < 0){
            exit(-1);
        }
        exit(-1);       
    }

    DIR* dir;
    struct dirent* ent;
    if((dir = opendir(students_path)) == NULL) {
        if(write(2,"Not a valid directory\n",22) < 0){
            exit(-1);
        };
        exit(-1);
    }
    char sub_path[500];
    while ((ent = readdir(dir)) != NULL) { //main loop
        int fd_in = open(input_file_path, O_RDONLY);
        if(fd_in < 0){
        if(write(2,"Input file not exist\n",21) < 0){
            exit(-1);
        }
        exit(-1);
        }
        memset(sub_path, 0, BUF_SIZE*2);
        sprintf(sub_path, "%s/%s", students_path, ent->d_name); //get the sub-student-folder path
        if (!is_directory(sub_path) || strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }//now we are only with students folders.
        //first:choose the name for the student's output file
        char output_name[] = ":student_output.txt";
        char output_student[BUF_SIZE];
        strcpy(output_student, ent->d_name);
        strcat(output_student,output_name);
        DIR* sub_dir;
        struct dirent* for_count;
        struct dirent* sub_ent;
        if((sub_dir = opendir(sub_path)) == NULL){
            if(write(2,"Error in: opendir\n",18) < 0){
                exit(-1);
            }     
            exit(-1);      
        }
        int filesCount = 0;
        while ((for_count = readdir(sub_dir)) != NULL) { //check how many files/dirs exists
            filesCount++;
        }
        closedir(sub_dir);
        if((sub_dir = opendir(sub_path)) == NULL){ //open again but now you know the number of files
            if(write(2,"Error in: opendir\n",18) < 0){
                exit(-1);
            }     
            exit(-1);      
        }
        int not_Cfiles = 0;
        while ((sub_ent = readdir(sub_dir)) != NULL) { //sub_ent is the first element inside the students directory
            if(!has_c_extension(sub_ent->d_name)){
                not_Cfiles++;
                if(not_Cfiles == filesCount){
                    char str[BUF_SIZE*2];
                    sprintf(str, "%s,%s",ent->d_name,"0,NO_C_FILE\n");
                    if (write(res, str, strlen(str)) < 0) { //write to the results.csv that there is no c file
                        if (write(2, "Error in: write\n", 16) < 0) {
                            exit(-1);
                        }
                        exit(-1);
                    }
                }
                continue;
            } //found the c file!
            char c_file_path[800];
            sprintf(c_file_path, "%s/%s", sub_path, sub_ent->d_name); //create the c file path
            pid_t pid1 = fork();
            if (pid1 < 0) {
                if(write(2,"Error in: fork\n",15) < 0){
                    exit(-1);
                }
                exit(-1);
            }
            if(pid1 == 0) { // child process for compiling the c file into c_file.out
                if (dup2(fd_errors, 2) < 0) { //redirecting the stderr to the errors.txt
                    if (write(2, "Error in: dup2\n", 15) < 0) {
                        exit(-1);
                    } 
                    exit(-1);
                }    
                char* argv[] = {"gcc", "-o", "c_file.out", "-w", c_file_path, NULL};
                execvp(argv[0], argv);
                if(write(2,"Error in: execvp\n",17) < 0){ //gets here only if there's an error 
                    exit(-1);
                }
                exit(-1);
            } //from now only father run
            int status1;
            if(waitpid(pid1, &status1, 0) < 0){
                if(write(2,"Error in: waitpi1\n",18) < 0){
                exit(-1);
                }
                exit(-1);
            } //write in results if the compilation failed and !!continue!! 
            if(WIFEXITED(status1)){
                if(WEXITSTATUS(status1) != 0){ //the file could not be compiled
                    char str2[BUF_SIZE*2];
                    sprintf(str2, "%s,%s",ent->d_name,"10,COMPILATION_ERROR\n");
                    if (write(res, str2, strlen(str2)) < 0) { //write to the results.csv that there is no c file
                        if (write(2, "Error in: write\n", 16) < 0) {
                            exit(-1);
                        }
                        exit(-1);
                    }
                    continue;
                }
            }
            pid_t pid2 = fork(); //fork for running the c_file.out
            if (pid2 < 0) {
                if(write(2,"Error in: fork\n",15) < 0){
                    exit(-1);
                }
                exit(-1);
            }
            if(pid2 == 0) { //child     
                if (dup2(fd_errors, 2) < 0) { //redirecting the stderr to the errors.txt
                    if (write(2, "Error in: dup2\n", 15) < 0) {
                        exit(-1);
                    } 
                    exit(-1);
                }
                if (dup2(fd_in, 0) < 0) { //redirecting the stdin to the io/input.txt
                    if (write(2, "Error in: dup2\n", 15) < 0) {
                        exit(-1);
                    } 
                    exit(-1);
                }
                int fd_out = open(output_student, O_RDWR | O_CREAT | O_TRUNC, 0777);
                if(fd_out < 0){
                    if(write(2,"Error in: open\n",15) < 0){
                        exit(-1);
                    }
                    exit(-1);
                }
                if (dup2(fd_out, 1) < 0) { //redirecting the stdout to the student_output.txt
                    if (write(2, "Error in: dup2\n", 15) < 0) {
                        exit(-1);
                    } 
                    exit(-1);
                }
                alarm(5);
                char* argv[] = {"./c_file.out", NULL}; 
                execvp(argv[0], argv); // execute the c_file.out program    
                if(write(2,"Error in: execvp\n",17) < 0){//gets here only if there's an error
                    exit(-1);
                }
                exit(-1);
            }  //father
            int status2;
            if(waitpid(pid2, &status2, 0) < 0){
                if(write(2,"Error in: waitpid\n",18) < 0){
                    exit(-1);
                }
                exit(-1);
            }
            // Check if the child process was terminated by the alarm signal
            if (WIFSIGNALED(status2) && WTERMSIG(status2) == SIGALRM) {
                    char str3[BUF_SIZE*2];
                    sprintf(str3, "%s,%s",ent->d_name,"20,TIMEOUT\n");
                    if (write(res, str3, strlen(str3)) < 0) { //write to the results.csv that there is no c file
                        if (write(2, "Error in: write\n", 16) < 0) {
                            exit(-1);
                        }
                        exit(-1);
                    }                
                    if(unlink("c_file.out") < 0 || unlink(output_student) < 0){
                        if(write(2,"Error in: unlink\n",17) < 0){
                            exit(-1);
                        }
                        exit(-1);   
                    }
                    continue;
            }
            pid_t pid3 = fork();//fork for comparing the two files
            if (pid3 < 0) {
                if(write(2,"Error in: fork\n",15) < 0){
                    exit(-1);
                }
                exit(-1);
            }
            if(pid3 == 0){
                if(dup2(fd_errors, 2) < 0) { //redirecting the stderr to the errors.txt
                    if (write(2, "Error in: dup2\n", 15) < 0) {
                        exit(-1);
                    } 
                    exit(-1);
                }
                char* args[] = {"./comp.out", output_file_path, output_student, NULL};
                execvp(args[0],args);// execute the comp.out program
                if(write(2,"Error in: execvp\n",17) < 0){//gets here only if there's an error
                    exit(-1);
                }
                exit(-1);
            } //father
            int status3;
            if(waitpid(pid3, &status3, 0) < 0){
                if(write(2,"Error in: waitpid\n",18) < 0){
                    exit(-1);
                }
                exit(-1);
            }
            int result = WEXITSTATUS(status3);
            char* reason;
            int grade;
            if (result == 1) {
                reason = "EXCELLENT";
                grade = 100;
            } else if (result == 2) {
                reason = "WRONG";
                grade = 50;
            } else if (result == 3) {
                reason = "SIMILAR";
                grade = 75;
            }
            char line[300];
            memset(line, 0, BUF_SIZE);
            sprintf(line, "%s,%d,%s\n", ent->d_name, grade, reason);
            if (write(res, line, strlen(line)) < 0) {
                if (write(2, "Error in: write\n", 16) < 0) {
                    exit(-1);
                }
                exit(-1);
            }
            if(close(fd_in) < 0){
                if(write(2,"Error in: close\n",16) < 0){
                    exit(-1);
                }
                exit(-1);       
            }
            if(unlink("c_file.out") < 0|| unlink(output_student) < 0){
                if(write(2,"Error in: unlink\n",17) < 0){
                    exit(-1);
                }
                exit(-1);   
            }
        }
    } 

}