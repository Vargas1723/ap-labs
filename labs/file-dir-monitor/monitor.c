#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <ftw.h>
#include "logger.h"

#define BUFFER_LENGTH sizeof(struct inotify_event) * 1024

struct inotify_event *event;

char *b;
int fd, wd, rd;

// FALTA
// soportar actualizaciones, eliminar o agregar directorios
// agregar arreglo the paths
// detectar si es normal file or directory
static int display_info(const char *fpath, const struct stat *sb,
             int tflag, struct FTW *ftwbuf)
{
  if(tflag == FTW_D){
    inotify_add_watch(fd, fpath,IN_ALL_EVENTS);
    printf("%-3s %2d %7jd   %-40s %d %s\n",
        (tflag == FTW_D) ?   "d"   : (tflag == FTW_DNR) ? "dnr" :
        (tflag == FTW_DP) ?  "dp"  : (tflag == FTW_F) ?   "f" :
        (tflag == FTW_NS) ?  "ns"  : (tflag == FTW_SL) ?  "sl" :
        (tflag == FTW_SLN) ? "sln" : "???",
        ftwbuf->level, (intmax_t) sb->st_size,
        fpath, ftwbuf->base, fpath + ftwbuf->base);

  }
  return 0;           /* To tell nftw() to continue */
}



int main(int argc, char** argv){
  // Place your magic here
  initLogger("stdout");
  if(argc<2){
    errorf("Please specify the path of the directoy to monitor\n");
    infof("To run this program you have to specify one path\n");
    return -1;
  }
  else if(argc>2){
    warnf("Only one directory can be monitored\n");
    infof("To run this program you have to specify one path\n");
    return -1;
  }
  infof("Starting File/Directory Monitor on %s\n", argv[1]);
  infof("----------------------------------------------------------\n");

  fd = inotify_init1(O_NONBLOCK);
  //wd = inotify_add_watch(fd, argv[1],IN_ALL_EVENTS);
  if (nftw((argc < 2) ? "." : argv[1], display_info, 20, 0)
           == -1) {
       errorf("nftw");
   }

  char *buffer = (char *)malloc(BUFFER_LENGTH);
  while(1){
    rd = read(fd, buffer, BUFFER_LENGTH);
    b = buffer;
    event = (struct inotify_event*)b;

    for(b = buffer; b < buffer + rd; ){
      event = (struct inotify_event*)buffer;

      if(event -> mask & IN_CREATE) warnf("- [File - Create] - %s\n",event -> name);
      else if(event -> mask & IN_OPEN) warnf("- [File - Open] - %s\n",event -> name);
      else if(event -> mask & IN_MODIFY) panicf("- [File - Modify] - %s\n",event -> name);
      else if(event -> mask & IN_ACCESS) infof("- [File - Access] - %s\n",event -> name);
      else if(event -> mask & IN_DELETE) errorf("- [File - Delete] - %s\n",event -> name);

      b += sizeof(struct inotify_event) + event->len;
    }
  }
  close(fd);
  return 0;


}
