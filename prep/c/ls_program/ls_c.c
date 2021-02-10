#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

int main(int argc, char *argv[]) {
  char* path = ".";

  bool FLAG_LONG_FORMAT = false;
  bool FLAG_ALL = false;
  bool FLAG_SORT_BY_SIZE = false;

  if (argc > 1) {
    // skip first arg, the program name
    argv++;

    char * arg;
    while ((arg = *argv) != NULL) {
      // Identify flags
      if (arg[0] == '-') {
        switch (arg[1]) {
        case 'l':
          FLAG_LONG_FORMAT = true;
          break;
        case 'a':
          FLAG_ALL = true;
          break;
        case 'S':
          FLAG_SORT_BY_SIZE = true;
          break;
        default:
          printf("ls: illegal option -- %c\n", arg[1]);
          printf("usage: ls [-@ABCFGHLOPRSTUWabcdefghiklmnopqrstuwx1] [file ...]\n");
          exit(1);
        }
      } else {
        path = *argv;
      }

      argv++;
    }
  }

  DIR* directory = opendir(path);
  struct dirent *curdir;

  while ((curdir = readdir(directory)) != NULL) {
    struct stat file_stats;
    lstat(curdir->d_name, &file_stats);

    // If -a flag not on, skip hidden files
    if (curdir->d_name[0] == '.' && !FLAG_ALL)
      continue;

    char* name;

    // if directory
    if (S_ISDIR(file_stats.st_mode))
      name = strcat(curdir->d_name, "/");
    // if regular file
    else if (S_ISREG(file_stats.st_mode))
      name = curdir->d_name;

    if (FLAG_ALL) {
      // number of hardlinks
      int hardlinks = file_stats.st_nlink;

      // owner name
      int owner_id = file_stats.st_uid;
      struct passwd *pwd;
      pwd = getpwuid(owner_id);

      // group name
      int group_id = file_stats.st_gid;
      struct group *grp;
      grp = getgrgid(group_id);

      // file size
      int file_size = file_stats.st_size;

      // last modified time
      char ts_buffer[80];
      struct tm timestamp;
      /* time_t last_modified = file_stats.st_mtime; */
      timestamp = *localtime(&file_stats.st_mtime);
      strftime(ts_buffer, 80, "%m %d %H:%M", &timestamp);

      printf("%d %s %s %d %s %s\n", hardlinks, pwd->pw_name, grp->gr_name, file_size, ts_buffer, curdir->d_name);
    } else
      printf("%s\n", curdir->d_name);
  }

  closedir(directory);

  return EXIT_SUCCESS;
}

// Supported flags:
// -l: long format
// -S: Sort by size
// Add error checking