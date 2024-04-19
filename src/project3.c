#include<stdio.h>
#include "objdb.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>

int main (int argc, char* argv[]){
    file_t db_name, file_name, obj_name;
    char letter, objcopy[MAX_BYTES]= {'\n'};
    int return_value=0, fd=-1, first=0, fd_db=-1, close=1, fr=1;

    db_name.length=1;
    file_name.length=1;
    obj_name.length=1;
    
    do {
        scanf (" %c", &letter);

        switch (letter){

            case 'o' : {
                /*function open*/
                /*if dtbase file already open then close and open new*/
                if ((fd != -1 || fd_db != -1)&&(fr == 0)){
                    free(db_name.pathname);
                    fr =1;
                }
                do_malloc (&db_name, 'd');
                fr=0;
                
                return_value = open_file (db_name.pathname, &fd, &fd_db);
                close=0;
                if (return_value == -42){
                    perror ("");
                    return 42;
                }
                else if (return_value == -21){
                    /*dtbase file incorrect*/
                    fprintf (stderr, "\nInvalid db file %s.\n", db_name.pathname);
                    free(db_name.pathname);
                    fr =1;
                    close =1;
                }
                else if (return_value == -1){
                    /*function open return's anomalies*/
                    fprintf (stderr, "\nError opening %s.\n", db_name.pathname);
                    free(db_name.pathname);
                    return 42;
                }
                break;
            }
            
            case 'i' : {
                /*function import*/

                do_malloc (&file_name, 'd');
                do_malloc (&obj_name, 'f');
                return_value = import (&fd, file_name.pathname, obj_name.pathname);

                if (return_value == 0){
                    fprintf(stderr, "\nNo open db file.\n");
                }
                else if (return_value == -1){
                    /*not found a file by that name*/
                    fprintf(stderr, "\nFile %s not found.\n", file_name.pathname);
                }
                else if (return_value == -42){
                    free(file_name.pathname);
                    free(obj_name.pathname);
                    perror ("");
                    return 42;
                }
                else if (return_value == 5){
                    fprintf(stderr, "\nObject %s already in db.\n", obj_name.pathname);
                }
                free(file_name.pathname);
                free(obj_name.pathname);
                break;
            }
            case 'f' : {
                /*function find, if given '*' printf every objname ,if not then print the speciifed if found*/
                do_malloc (&obj_name, 'f');
                first=0;

                printf ("\n##\n");
                if (strcmp (obj_name.pathname, "*") != 0){
                    while(1){
                        return_value = find_obj (obj_name.pathname, &fd, &first, objcopy, 'm');
                        if (return_value == -42){
                            free(obj_name.pathname);
                            free(db_name.pathname);
                            perror ("");
                            return 42;
                        }
                        else if (return_value == -1){
                            free(obj_name.pathname);
                            break;
                        }
                        else if (return_value == 0){
                            fprintf(stderr, "\nNo open db file.\n");
                            free(obj_name.pathname);
                            break;
                        }
                        else if (return_value == 5){
                            printf ("%s\n", objcopy);
                        }
                    }
                    if (return_value == 5){
                        free(obj_name.pathname);
                    }
                }
                else {
                    while (1){
                        return_value = find_obj (obj_name.pathname, &fd, &first, objcopy, '*');
                        if (return_value == -42){
                            free(obj_name.pathname);
                            free(db_name.pathname);
                            perror ("");
                            return 42;
                        }
                        else if (return_value == -1){
                            free(obj_name.pathname);
                            break;
                        }
                        else if (return_value == 0){
                            fprintf(stderr, "\nNo open db file.\n");
                            free(obj_name.pathname);
                            break;
                        }
                        else if (return_value == 5){
                            printf ("%s\n", objcopy);
                        }
                    }
                    if (return_value == 5){
                        free(obj_name.pathname);
                    }
                }

                break;
            }
            case 'e' : {
                /*function export*/
                do_malloc (&obj_name, 'f');
                do_malloc (&file_name, 'd');
                return_value = export (obj_name.pathname, file_name.pathname, &fd);
                if (return_value == -42){
                    free(file_name.pathname);
                    free(obj_name.pathname);
                    free(db_name.pathname);
                    perror ("");
                    return 42;
                }
                else if (return_value == 0){
                    fprintf(stderr, "\nNo open db file.\n");
                }
                else if ((return_value == -12)||(return_value == -25)){
                    fprintf(stderr, "\nCannot open file %s.\n", file_name.pathname);
                }
                else if (return_value == -1){
                    fprintf(stderr, "\nObject %s not in db.\n", obj_name.pathname);
                }
                free(file_name.pathname);
                free(obj_name.pathname);
                break;
            }
            case 'd' : {
                /*function delete*/
                do_malloc(&obj_name, 'f');
                return_value = delete(obj_name.pathname, &fd, &fd_db);
                if (return_value == -42){
                    free(obj_name.pathname);
                    free(db_name.pathname);
                    perror("");
                    return 42;
                }
                else if (return_value == -1){
                    fprintf(stderr, "\nObject %s not in db.\n", obj_name.pathname);
                }
                else if (return_value == -8){
                    fprintf(stderr, "\nNo open db file.\n");;
                }
                free(obj_name.pathname);
                break;
            }
            case 'c' : {
                /*function close*/
                if (fr == 0){
                    free(db_name.pathname);
                    fr=1;
                }
                if (close == 1){
                    fprintf (stderr, "\nNo open db file.\n");
                    break;
                }
                return_value = close_file (&fd, &fd_db);
                close=1;
                if (return_value == 1){
                    fprintf (stderr, "\nNo open db file.\n");
                }
                else if (return_value == -1){
                    perror ("");
                    return 42;
                }
                break;
            }
            case 'q' : {
                if (fr == 0){
                    free(db_name.pathname);
                    fr=1;
                }
                if (close == 1){
                    break;
                }
                return_value = close_file (&fd, &fd_db);
                if (return_value == -1){
                    perror ("");
                    return 42;
                }
                break;
            }
        }

    }
    while(letter != 'q');
    

    return 0;
} 
