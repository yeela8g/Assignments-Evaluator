// Ye'ela Granot 209133107
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#define BUF_SIZE 4096

int compare(char* path1, char* path2){

    int file1 = open(path1, O_RDONLY);
    int file2= open(path2, O_RDONLY);
    if(file1 < 0 || file2 < 0){
         if(write(2,"Error in: open\n",15) < 0){
            exit(-1);
        }
       exit(-1);
    }
    char buf1[BUF_SIZE], buf2[BUF_SIZE];
    ssize_t len1, len2;
    int identity = 1;
    int i, j;

    while ((len1 = read(file1, buf1, BUF_SIZE)) > 0 &&
           (len2 = read(file2, buf2, BUF_SIZE)) > 0) {

        if (len1 != len2) {
            identity = 0;
            break;
        }

        for (int i = 0; i < len1; i++) {
            if (buf1[i] != buf2[i]) {
                identity = 0;
                break;
            }
        }

        memset(buf1, 0, sizeof(buf1));
        memset(buf2, 0, sizeof(buf2));
        
    }

    if (len1 < 0 || len2 < 0) {
        if(write(2,"Error in: read\n",15) < 0){
            exit(-1);
        }
        exit(-1);
    }
    
    if(len2 && !len1){
        len2 = read(file2, buf2, BUF_SIZE);
        if(len2 < 0){
            if(write(2,"Error in: read\n",15) < 0){
                exit(-1);
            }
            exit(-1);
        }
    }
    if (len1 != len2) {
        identity = 0;
    }

    if (identity) {
        return 1;
    }

    if (lseek(file1, 0, SEEK_SET) == -1 || lseek(file2, 0, SEEK_SET) == -1) {
        if(write(2,"Error in: lseek\n",16) < 0){
            exit(-1);
        }
        exit(-1);
    }

    //look for similarity or differency
    len1 = read(file1, buf1, BUF_SIZE);
    len2 = read(file2, buf2, BUF_SIZE);
    if(len1 < 0 || len2 < 0){
            if(write(2,"Error in: read\n",15) < 0){
                exit(-1);
            }
            exit(-1);
    }

    int similarity = 3; //similar 
    int final_i = 0;
    int final_j = 0;
    for (int i = 0, j = 0; i < len1 && j < len2; i++, j++) {
        char c1 = buf1[i];
        char c2 = buf2[j];
        if (isspace(c1) || isspace(c2)) {
            // skip white spaces in both strings
            while (isspace(c1) && i < len1) {
                i++;
                c1 = buf1[i];
            }
            while (isspace(c2) && j < len2) {
                j++;
                c2 = buf2[j];
            }
            // reset indices if we reached the end of a file
            if (i == len1) {
                memset(buf1, 0, sizeof(buf1));
                if((len1 = read(file1, buf1, BUF_SIZE)) < 0){
                    if(write(2,"Error in: read\n",15) < 0){
                        exit(-1);
                    }
                    exit(-1);
                }
                if(!len1){
                    final_i = i;
                    final_j = j;
                    break;

                }
                i = -1;
                j -= 1;
                continue;
            }
            if (j == len2) {
                memset(buf2, 0, sizeof(buf2));
                if((len2 = read(file1, buf2, BUF_SIZE)) < 0){
                    if(write(2,"Error in: read\n",15) < 0){
                        exit(-1);
                    }
                    exit(-1);
                if(!len2){
                    final_i = i;
                    final_j = j;
                    break;
                }                    
                j = -1; //deal with large files 
                i -= 0 ;
                continue;
                }
            }
        }
        if(c1 == c2){
            continue;
        }
        if (tolower(c1) == tolower(c2)) {
            continue;
        }
        
        if (c1 != c2) {
            similarity = 2;
            break;
        }
        if(i == (len1 - 1)){ //the buffer ends -> load it with new text so the loop won't end
            memset(buf1, 0, sizeof(buf1));
            if((len1 = read(file1, buf1, BUF_SIZE)) < 0){
                    if(write(2,"Error in: read\n",15) < 0){
                        exit(-1);
                    }
                    exit(-1);
            }
            if(!len1){
                final_i = i;
                final_j = j;
                break;
            }            
            i = -1;
        }
        if(j == (len2 - 1)){
            memset(buf2, 0, sizeof(buf2));
            if((len2 = read(file2, buf2, BUF_SIZE)) < 0){
                    if(write(2,"Error in: read\n",15) < 0){
                        exit(-1);
                    }
                    exit(-1);
            }
            if(!len2){
                final_i = i;
                final_j = j;
                break;
            }           
            j = -1;
        }
        if(!len1 || !len2){
            final_i = i;
            final_j = j;
            break;
        }
    }
    if(!len1 && len2){
        while(final_j < len2){
            char c2 = buf2[final_j];
            if(!isspace(c2)){
                similarity = 2;
            }
            final_j++;
        }
        memset(buf2, 0, sizeof(buf2));
        while ((len2 = read(file2, buf2, BUF_SIZE)) > 0) {
           while(final_j < len2){
                char c2 = buf2[final_j];
                if(!isspace(c2)){
                    similarity = 2;
                }
                final_j++;
            }
        }
        if (len2 < 0) {
            if(write(2,"Error in: read\n",15) < 0){
                exit(-1);
            }
            exit(-1);
        }
    }
    if(len1 && !len2){
        while (final_i < len1) {
            char c1 = buf1[final_i];
            if(!isspace(c1)){
                similarity = 2;
            }
            final_i++;
        }
        memset(buf1, 0, sizeof(buf1));
        while ((len1 = read(file1, buf1, BUF_SIZE)) > 0) {
            while (final_i < len1) {
                char c1 = buf1[final_i];
                if(!isspace(c1)){
                    similarity = 2;
                }
                final_i++;
            }
        }
        if (len1 < 0) {
            if(write(2,"Error in: read\n",15) < 0){
                exit(-1);
            }
            exit(-1);
        }        
    }
    if(close(file1) < 0 || close(file2) < 0){
            if(write(2,"Error in: close\n",16) < 0){
                exit(-1);
            }
            exit(-1);
    }
    return similarity;
}


int main(int argc, char* argv[]){
    if(argc != 3){
        if(write(2,"wrong arguments number\n",23) < 0){
            exit(-1);
        }
        exit(-1);
    }
    int result = compare(argv[1], argv[2]);
    return result;
}