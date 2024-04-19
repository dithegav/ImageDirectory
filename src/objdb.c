/*In this file i will be explaining and implementing every function that is needed to make this program and this excersise to work properly
, this functions are going to be used in project3.c main file*/

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include "objdb.h"
#include <string.h>
#include <stdlib.h>

int do_malloc (file_t *file, char h){
    char *help_ptr, c;
    int i=0;

    help_ptr = (char*)malloc(sizeof(char));

    if (help_ptr == NULL){
        /*fail to malloc*/
        return -71;
    }
    file->pathname = help_ptr;
    scanf (" %c", &file->pathname[0]);
    file->length += 1;

    do {
        scanf("%c", &c);
        if ((c == '\n' || c == ' ')){
            c = '\0';
        }
        file->pathname = (char*)realloc (file->pathname, file->length);
        
        file->length+= 1;
        i++;
        file->pathname[i] = c;

    }while(c != '\0' && c != ' ');



    return 0;
}

int do_realloc (file_t *file){
    char *help_ptr;
    int i=0;

    help_ptr = (char*)realloc (file->pathname, file->length+1);

    if (help_ptr == NULL){
        /*fail to malloc*/
        return -71;
    }
    file->pathname = help_ptr;

    for (i=0; i < file->length; i++){
        file->pathname[i] = '0'; 
    }
    file->pathname[i] = '\0';

    /*success*/
    return 0;

}

int write_file (int *fd, void *buf, int size, int length){
    int  curr=0, n=0;

    do {
        if (size == 4){
            curr = write (*fd, &((int*)buf)[n], length-n);
        }
        else if (size == 1){
            curr = write (*fd, &((char*)buf)[n], length-n);
        }

        if (curr < 0){
            return -42;
        }
        else if (curr == 0){
            return 0;
        }

        n += curr;

    }
    while (n != length);
    n=0;

    return *fd;
}

int read_file (int *fd, void *buf, int size, int length){
    int  curr=0, n=0;

    do {
        if (size == 4){
            curr = read (*fd, &((int*)buf)[n], length-n);
        }
        else if (size == 1){
            curr = read (*fd, &((char*)buf)[n], length-n);
        }

        if (curr < 0){
            return -42;
        }
        else if (curr == 0){
            /*no more*/
            return 0;
        }
        n += curr;
    }
    while (n != length);
    n=0;

    return *fd;
}

int close_file (int *fd, int *fd_db){
    int cls=0;
    /*close if opened both or one of the fd from database file*/

    if (*fd < 0){
        /*no open file*/
        return 1;
    }
    cls = close (*fd);
    *fd = -1;

    if (*fd_db < 0){
        return 1;
    }
    cls = close (*fd_db);
    *fd_db = -1;

    return cls;
}

int open_file (const char *path, int *fd, int *fd_db){
    int cls=0, system_fail=0;
    char buf[NAMESIZE+1] = {'\0'};
    
    /*if a file is already open close it*/
    if (*fd > 0){
        /*call close and close the program*/
        cls = close_file (fd, fd_db);

        /*if close returns -1 , systme failed*/
        if (cls == -1){
            return -42;
        }
    }
    if (*fd_db > 0){
        /*call close and close the program*/
        cls = close_file (fd, fd_db);

        /*if close returns -1 , systme failed*/
        if (cls == -1){
            return -42;
        }
    }

    *fd = open (path, O_CREAT|O_EXCL|O_RDWR, 0700);

    /*first see if you can create a file with  that name and do the appropriate changes*/
    if (*fd > 0){
        
        /*new file needs a MAGIC_NUMBER to identify it as a database file*/
        system_fail = write_file (fd, MAGIC_NUMBER, sizeof(char), NAMESIZE);

        if (system_fail == -42){
            /*system fail*/
            return -42;
        }
        
        /*open second fd for function delete*/
        *fd_db = open (path, O_RDWR, 0);

        return *fd;
    }

    *fd = open (path, O_RDWR, 0700);

    if (*fd > 0){

        system_fail = read_file (fd, buf, sizeof(char), NAMESIZE);

        if (system_fail == -42){
            /*system fail*/
            return -42;
        }
        
        if (strcmp (buf, MAGIC_NUMBER) != 0){
            /*not a data base file*/
            return -21;
        }
        else {
            /*open second fd for function delete*/
            *fd_db = open (path, O_RDWR, 0);
            return *fd;
        }
    }
    
    return -1;

}

int find_obj (char *objname, int *fd, int *first, char obj_copy[], char c){
    int system_fail=0, i=0, compare=0;
    char buf[MAX_BYTES] = {'\n'}, *result;
    file_t objcopy;
    int num=0, flag=0;
    /*find a objname depending on the input and end the function normally after passing 
    the filename or earlier depending on the function thats calling find_obj*/
    /*return -42 for every fail of a called function and then return the appropriate 
    number depending on the result*/

    if (*fd < 0){
        /*no open data base file*/
        return 0;
    }

    if (*first == 0){
        /*first time*/
        /*start from the start to search*/
        *first+= 1;
        system_fail = lseek (*fd, strlen(MAGIC_NUMBER), SEEK_SET);

        if (system_fail == -1){
            /*system fail*/
            return -42;
        }
    }

    while(1){
        /*read the length of objname and compare it to the given one*/
        system_fail = read_file (fd, &num, sizeof(int), 4);

        if (system_fail == -1){
            return -42;
        }
        else if (system_fail == 0){
            return -1;
        }

        system_fail = read_file (fd, buf, sizeof(char), num);

        if (system_fail == -1){
            return -42;
        }
        else if (system_fail == 0){
            return -1;
        }
        
        /*in other words for every function thats calling find do strcmp and if find_obj is called from main do strstr*/
        if ((c != 'm')&&(c != '*')){
            compare = strcmp (buf, objname);
            if (compare == 0){
                result = buf;
            }
            else {
                result = NULL;
            }
        }
        else {
            result = strstr (buf, objname);
        }

        if ((result != NULL)||(c == '*')){
            
            result = (char*)malloc(sizeof(char));

            if (result == NULL){
                /*fail to malloc*/
                return -71;
            }
            objcopy.pathname = result;
            objcopy.length = strlen (buf);
            system_fail = do_realloc (&objcopy);
            if (system_fail == -71){
                /*no memory*/
                return -42;
            }
            strcpy (objcopy.pathname, buf);
            strcpy (obj_copy, objcopy.pathname);
            free(objcopy.pathname);
            flag += 1;
        }
        
        for (i=0; i < 512; i++){
            buf[i] = '\0';
        }

        /*if all good and called from export or delete then stop now*/
        if (flag == 1 && (c == 'e' || c == 'd')){
            flag =0;
            return 5;
        }
        
        system_fail = read_file (fd, &num, sizeof(int), 4);

        if (system_fail == -1){
            return -42;
        }

        system_fail = lseek (*fd, num, SEEK_CUR);

        if (system_fail == -1){
            return -42;
        }

        if (flag == 1){
            flag = 0;
            return 5;
        }

    }
    
}

int import (int *fd, char *fname, char *objname){
    struct stat info;
    int f_fail=0, system_fail=0, length=0, fd1=0, first =0;
    char buf[MAX_BYTES] = {'\n'}, obj_copy[MAX_BYTES] = {'\n'};
    /*add the input of a filename to an open dtbase file but first add the length of the objname given 
    then the objname, again add the length of the input of the file and then the actual data*/
    /*if the situation does not act normal return -42 and the filename or dtaabase file are 
    not open or non existent return accordingly*/

    if (*fd < 0){
        /*no open data base file*/
        return 0;
    }

    f_fail = access (fname, F_OK);

    if (f_fail == -1){
        /*no file name detected return fail*/
        return -1;
    }

    system_fail = find_obj(objname, fd, &first, obj_copy, 'i');
    if (system_fail == 5){
        if (strcmp(obj_copy, objname) == 0){
            /*already excistent objname*/
            return 5;
        }
        
    }
    else if (system_fail == -42){
        /*systme_fail*/
        return -42;
    }

    system_fail = lseek (*fd, 0, SEEK_END);

    if (system_fail == -1){
        /*systme_fail*/
        return -42;
    }

    fd1 = open (fname, O_RDWR);

    if (fd1 < 0){
        return -1;
    }

    if (system_fail == -1){
        return -42;
    }

    length = strlen (objname);
        
    system_fail = write_file (fd, &length, sizeof(int), 4);

    if (system_fail == -42){
        /*system failed*/
        return -42;
    }
    
    while(1){

        if (length == 0){
            /*out of input*/
            break;
        }

        if (length > MAX_BYTES){
            /*write 512 bytes exactly*/
            system_fail = write_file (fd, objname, sizeof(char), MAX_BYTES);

            if (system_fail == -42){
                /*system failed*/
                return -42;
            }

            length = length - MAX_BYTES;
        }
        else if (length <= MAX_BYTES){
            system_fail = write_file (fd, objname, sizeof(char), length);

            if (system_fail == -42){
                /*system failed*/
                return -42;
            }
            break;
        } 
            
    }

    system_fail = fstat (fd1, &info);

    if (system_fail == -1){
        /*system failed*/
        return -42;
    }

    length = info.st_size;

    system_fail = write_file (fd, &length, sizeof(int), 4);

    if (system_fail == -42){
        return -42;
    }

    while (1){
        /*read and write the given bytes each time 512 or less but first write infront the length in binary*/
        if (length == 0){
            break;
        }

        if (length > MAX_BYTES){
            system_fail = read_file (&fd1, buf, sizeof(char), MAX_BYTES);

            if (system_fail == -42){
                return -42;
            }

            system_fail = write_file (fd, buf, sizeof(char), MAX_BYTES);

            if (system_fail == -42){
                return -42;
            }

            length = length - MAX_BYTES;
        }
        else if (length <= MAX_BYTES){
            system_fail = read_file (&fd1, buf, sizeof(char), length);

            if (system_fail == -42){
                return -42;
            }

            system_fail = write_file (fd, buf, sizeof(char), length);

            if (system_fail == -42){
                return -42;
            }
            break;
        }
    }
   
    /*success*/
    return 1;
}

int export (char *objname, char *fname, int *fd){
    char obj_copy[MAX_BYTES]={'\0'}, buf[MAX_BYTES] = {'\0'};
    int result=0, first=0, fd2=0, num=0;
    /*find the objname requested and then the filename attached to it so 
    it can be transfered safely and correctly to another fresh opened file*/

    if (*fd < 0){
        /*no open data base file*/
        return 0;
    }

    result = access (fname, F_OK);

    if (result == -1){
        fd2 = open(fname, O_CREAT|O_RDWR, 0700);
        if (fd2 == -1){
            /*fail open file so printf in main message cannot open file and then perror*/
            return -12;
        }
    }
    else if (result == 0){
        /*fname already exists*/
        /*cannot open file fname*/
        return -25;
    }

    first=0;
    result = find_obj (objname, fd, &first, obj_copy, 'e');
    if (result == -42){
        /*system fail*/
        return -42;
    }
    else if (result == -1){
        /*fail to find objname*/
        unlink (fname);
        return -1;
    }
    else if (result == 5){
        /*success*/
    }

    if (strcmp(objname, obj_copy) != 0){
        /*fail to find objname*/
        unlink (fname);
        return -1;
    }

    result = read_file (fd, &num, sizeof(int), 4);

    if (result == -1){
        return -42;
    }
    else if (result == 0){
        return -8;
    }

    while (1){
        /*do the same as function import without the objname*/
        if (num == 0){
            /*out of input*/
            break;
        }

        if (num > MAX_BYTES){

            result = read_file (fd, buf, sizeof(char), MAX_BYTES);

            if (result == -1){
                return -42;
            }
            else if (result == 0){
                return -8;
            }

            result = write_file (&fd2, buf, sizeof(char), MAX_BYTES);

            if (result == -42){
                return -42;
            }

            num = num - MAX_BYTES;
        }
        else if (num <= MAX_BYTES){

            result = read_file (fd, buf, sizeof(char), num);

            if (result == -1){
                return -42;
            }
            else if (result == 0){
                return -8;
            }

            result = write_file (&fd2, buf, sizeof(char), num);

            if (result == -42){
                return -42;
            }
            break;
        }
    }

    /*succes*/
    return 1;
}

int delete (char *objname, int *fd, int *fd_db){
    int system_fail=0, length=0, first=0, num=0, size=0, n=0;
    char obj_copy[MAX_BYTES]= {'\0'}, buf[MAX_BYTES] = {'\0'};
    struct stat info;
    /*is a function that will delete both objname and filename and their length's by using 2 files descriptors 
    and by transfering each time the bytes (fd)infront of the thing going to be deleted,
    (fd_db)onto them and then ftruncate the end*/

    if (*fd < 0 || *fd_db < 0){
        /*No open db file*/
        return -8;
    }

    system_fail = find_obj (objname, fd_db, &first, obj_copy, 'd');
    first=0;

    if (system_fail == -42){
        /*problem system cant handle*/
        return -42;
    }
    else if (system_fail != 5){
        /*fail to find objname*/
        return -1;
    }

    if (strcmp(objname, obj_copy) != 0){
        /*fail to find objname*/
        return -1;
    }

    system_fail = strlen (obj_copy);
    size = system_fail + sizeof(int);
    system_fail = read_file(fd_db, &num, sizeof(int), 4);
    if (system_fail == -42){
        /*problem system cant handle*/
        return -42;
    }
    n = num;
    while (n > MAX_BYTES){
        system_fail = read_file(fd_db, buf, sizeof(char), MAX_BYTES);
        if (system_fail == -42){
            /*problem system cant handle*/
            return -42;
        }
        n -= MAX_BYTES;
    }
    system_fail = read_file(fd_db, buf, sizeof(char), n);
    if (system_fail == -42){
        /*problem system cant handle*/
        return -42;
    }
    size += num + sizeof(int);

    system_fail = lseek (*fd_db, (off_t)-(size), SEEK_CUR);

    if (system_fail == -1){
        /*problem system cant handle*/
        return -42;
    }
    
    /*changed char to 'g' so i can start fd from the next objname 
    but to keep the fd_db to the posiiton that i will transfer them back too*/

    system_fail = find_obj (objname, fd, &first, obj_copy, 'g');

    if (system_fail == -42){
        /*problem system cant handle*/
        return -42;
    }
    else if (system_fail != 5){
        /*fail to find objname*/
        return -1;
    }

    while(1){    
        /*first while reads thelength of the objname/filename and the second one transfers to the back*/

        system_fail = read_file(fd, &num, sizeof(int), 4);
        if (system_fail == -42){
            /*problem system cant handle*/
            return -42;
        }
        else if (system_fail == 0){
            /*out of input*/
            break;
        }
        system_fail = write_file (fd_db, &num, sizeof(int), 4);
        if (system_fail == -42){
            /*problem system cant handle*/
            return -42;
        }
        else if (system_fail == 0){
            /*out of input*/
            break;
        }

        while (1){

            if (num == 0){
                break;
            }

            if (num > MAX_BYTES){
                system_fail = read_file (fd, buf, sizeof(char), MAX_BYTES);

                if (system_fail == -42){
                    return -42;
                }
                else if (system_fail == 0){
                    /*out of input*/
                    break;
                }

                system_fail = write_file (fd_db, buf, sizeof(char), MAX_BYTES);

                if (system_fail == -42){
                    return -42;
                }
                else if (system_fail == 0){
                    /*out of input*/
                    break;
                }

                num = num - MAX_BYTES;
            }
            else if (num <= MAX_BYTES){
                system_fail = read_file (fd, buf, sizeof(char), num);

                if (system_fail == -42){
                    return -42;
                }
                else if (system_fail == 0){
                    /*out of input*/
                    break;
                }

                system_fail = write_file (fd_db, buf, sizeof(char), num);

                if (system_fail == -42){
                    return -42;
                }
                else if (system_fail == 0){
                    /*out of input*/
                    break;
                }
                
                break;
            }
        }
    }

    system_fail = fstat (*fd, &info);

    if (system_fail == -1){
        /*system failed*/
        return -42;
    }

    length = info.st_size;

    ftruncate(*fd_db, length-size);

    if (system_fail == -1){
        /*system failed*/
        return -42;
    }

    /*all went smooth*/
    return 1;
}

