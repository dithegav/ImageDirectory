#ifndef O_OBJDB
#define O_OBJDB

#define MAGIC_NUMBER "Prog2"
#define NAMESIZE 5
#define MAX_BYTES 512

typedef struct {
    char *pathname;
    int length;
}file_t;

int do_malloc (file_t *file, char h);

int close_file(int *fd, int *fd_db);

int open_file (const char *path, int *fd, int *fd_db);

int find_obj (char *objname, int *fd, int *first, char obj_copy[], char c);

int import (int *fd, char *fname, char *objname);

int export (char *objname, char *fname, int *fd);

int delete (char *objname, int *fd, int *fd_db);

#endif
