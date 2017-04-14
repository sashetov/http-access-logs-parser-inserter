#define _GNU_SOURCE
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <ctype.h>
#include <string.h>

#define handle_error(msg) \
  do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct linux_dirent {
  long           d_ino;
  off_t          d_off;
  unsigned short d_reclen;
  char           d_name[];
};

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
  int fd, nread;
  int i;
  char buf[BUF_SIZE];
  struct linux_dirent *d;
  int bpos;
  int debug_mode = 1;
  fd = open(argc > 1 ? argv[1] : ".", O_RDONLY | O_DIRECTORY);
  if (fd == -1) {
    handle_error("open");
  }
  for ( ; ; ) {
    nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);
    if (nread == -1) {
      handle_error("getdents");
    }
    if (nread == 0) {
      break;
    }
    for (bpos = 0; bpos < nread;) {
      d = (struct linux_dirent *) (buf + bpos);
      char * name = d->d_name;
      char letter = ' ';
      if( debug_mode ) {
        printf("char * name = d->d_name : %s\n", name);
        printf("strlen(name): %lu\n", (unsigned long)strlen(name));
      }
      if( strlen(name) >= 14 ) {
        if( name[13] == '.' && name[14]=='w' && name[15] == 'w' && 
            name[16] =='w' && name[17] == '.' ) {
          letter = name[18];
          if( debug_mode ) {
            printf("letter = name[18] : %c\n", letter);
          }
        } else {
          letter = name[14];
          if( debug_mode ) {
            printf("letter = name[14] : %c\n", letter);
          }
        }
      }
      letter = tolower(letter);
      if( debug_mode ) {
        printf("letter = tolower(letter); : \'%c\'\n", letter);
      }
      if( letter != ' ') {
        if( debug_mode ) {
          printf("sizeof(letter): %lu\n", sizeof(letter) );
        }
        char *str_letter = malloc(sizeof(letter));
        if( debug_mode ) {
          printf("char *str_letter= malloc(sizeof(letter);  &str_letter; : %lu\n", (unsigned long)&str_letter);
        }
        *str_letter = letter;
        if( letter == '.' ) {
          // . means this dir in linux, so use something else...
          *str_letter = '_';
        }
        if( debug_mode ) {
          printf("str_letter; : \'%s\'\n", str_letter);
        }
        size_t file_len = strlen(name);
        if( debug_mode ) {
          printf("file_len; : %d\n", (int)file_len);
        }
        if( debug_mode ) {
          printf("argc; : %d\n", argc );
          printf("argv[0]; : %s\n", argv[0]);
          for (i=0, i < (int)argc; i++; ){
            printf( "argv[%d]; : %s\n",i ,argv[i] );
          }
        }
        if( argc < 2 ){
          printf("Error: provide at least 1 argument\n");
          exit(EXIT_FAILURE);
        }
        // / and +1 for ending null char
        size_t old_file_path_len = strlen(argv[1]) + 1 + file_len + 1;
        if( debug_mode ) {
          printf("old_file_path_len; : %d\n", (int)old_file_path_len);
          //printf("new_path_len; : %d\n", old_file_path_len);
        }
        //  +3 for /c/ , file_len, +1 for ending null char
        size_t new_path_len = strlen(argv[1]) + 3 + file_len + 1;
        //  +3 for /c/ , +1 for ending null char
        size_t new_dir_len = strlen(argv[1]) + 4;
        char *old_path = (char*) malloc(old_file_path_len);
        char *new_path= (char*) malloc(new_path_len);
        char *new_dir= (char*) malloc(new_dir_len);

        // concat old file path
        strcpy( old_path, argv[1]);
        strcat( old_path, "/");
        strcat( old_path, name );
        // concat new file path
        strcpy( new_path, argv[1]);
        strcat( new_path, "/");
        strcat( new_path, str_letter);
        strcat( new_path, "/");
        strcat( new_path, name);
        // concat new dir path
        strcpy( new_dir, argv[1]);
        strcat( new_dir, "/");
        strcat( new_dir, str_letter);
        strcat( new_dir, "/");
        if( debug_mode ) {
          printf("old path: %s\n", old_path);
          printf("new path: %s\n", new_path);
          printf("new dir: %s\n",  new_dir);
        }
        if( mkdir( new_dir, 0755 ) < 0) {
          // 17 is file exists
          if( errno != 17 ) {
            printf( "Errno: %d\n", errno);
            printf( "Could not create %s because: %s\n", 
                new_dir, strerror(errno) );
            exit(EXIT_FAILURE);
          }
        }
        // move the file in question
        if( debug_mode ) {
          printf( "rename %s to %s\n", old_path, new_path );
        }
        //rename( old_path, new_path );
      }
      bpos += d->d_reclen;
    }
  }
  exit(EXIT_SUCCESS);
}
