#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <ftw.h>
#include <string.h>
#include "logger.h"

#define BUFFER_LENGTH sizeof(struct inotify_event) * 1024

struct inotify_event *event;
struct directory {
  int wd;
  int level;
  char *filePath;
  struct directory *nextDirectory;
};
static struct directory *firstDirectory;

char *b;
int fd, rd;

static int monitorDirectories(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf);
static int isDirectory(char *path);
static char *completePath(char *path, char *file);
static int addDirectoryPath(const char *dir, int level);
static struct directory *getDirectory(int wd);

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
  if (nftw((argc < 2) ? "." : argv[1], monitorDirectories, 20, 0)
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

      struct directory *dir;
      dir = getDirectory(event->wd);

      char *path = completePath(dir->filePath,event->name);
      int isDirect = isDirectory(path);
      char *type = isDirect ? "Directory" : "File";

      if(event -> mask & IN_CREATE)
      {
        addDirectoryPath(path,1);
        warnf("- [%s - Create] - %s\n",type,path);
      }
      else if(event -> mask & IN_OPEN) warnf("- [%s - Open] - %s\n",type,path);
      else if(event -> mask & IN_MODIFY) panicf("- [%s - Modify] - %s\n",type,path);
      else if(event -> mask & IN_DELETE) errorf("- [%s - Delete] - %s\n",type,path);

      b += sizeof(struct inotify_event) + event->len;
    }
  }
  close(fd);
  return 0;
}
static int monitorDirectories(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf)
{
  if(tflag == FTW_D){
    //inotify_add_watch(fd, fpath,IN_ALL_EVENTS);
    addDirectoryPath(fpath,ftwbuf->level);
  }
  return 0;           /* To tell nftw() to continue */
}
// Know if the file is a directory
static int isDirectory(char *path)
{
  struct stat path_stat;
  return stat(path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode);
}
// Mix path of current firectory with filename
static char * completePath(char *path,char *file)
{
  char *complete = malloc(strlen(path)+strlen(file)+2);
  strcpy(complete,path);
  strcat(complete,"/");
  strcat(complete,file);
  return complete;
}
// Add the path to the List and add to watch the directory
static int addDirectoryPath(const char *dir, int level)
{
  int wd;
  if((wd = inotify_add_watch(fd,dir,IN_ALL_EVENTS)) == -1) return -1;
  struct directory *newDirectory = malloc(sizeof(struct directory));
  newDirectory->wd = wd;
  newDirectory->level = level;
  newDirectory->filePath = malloc(strlen(dir)+1);
  strcpy(newDirectory->filePath,dir);
  newDirectory->nextDirectory = firstDirectory;
  firstDirectory = newDirectory;
  return 0;
}
// Find the path of the event filename
static struct directory *getDirectory(int wd)
{
  struct directory *current = firstDirectory;
  while(current != NULL)
  {
    if (current->wd == wd) return current;
    current = current->nextDirectory;
  }
  return NULL;
}
