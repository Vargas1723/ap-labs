#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <syslog.h>
#include <stdarg.h>
#include "logger.h"

#define RESET 0
#define BRIGHT 1
#define DIM 2
#define UNDERLINE 3
#define BLINK 4
#define REVERSE 7
#define HIDDEN 8

#define BLACK 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6
#define WHITE 7
#define NORMAL_BLACK 8


bool logMode;
bool initLog;

void textColor(int attr, int fg, int bg)
{
  char command[13];
  sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
  printf("%s", command);
}
void normalColor(){
  textColor(RESET,NORMAL_BLACK,WHITE);
}
int initLogger(char *logType) {
  initLog = true;
    if(strcmp(logType, "stdout")==0 ){
      logMode = false;
      printf("Initializing Logger on: %s\n", logType);
    }
    else if(strcmp(logType,"syslog")==0){
      logMode = true;
      printf("Initializing Logger on: %s\n", logType);
    }
    else{
      logMode = false;
      initLog = false;
      printf("Wrong Parameter, using stdout\n");
    }
    return 0;
}

int infof(const char *format, ...) {
  if(!initLog){
    initLogger("stdout");
  }
  va_list args;
  va_start(args, format);
  textColor(RESET,CYAN,WHITE);
  if(logMode){
    openlog("Logger", LOG_PID | LOG_CONS, LOG_SYSLOG);
    vsyslog(LOG_INFO, format, args);
    closelog();
  }
  else {
    vprintf(format,args);
  }
  va_end(args);
  normalColor();
  return 0;
}

int warnf(const char *format, ...) {
  if(!initLog){
    initLogger("stdout");
  }
  va_list args;
  va_start(args, format);
  textColor(RESET,MAGENTA,WHITE);
  if(logMode){
    openlog("Logger", LOG_PID | LOG_CONS, LOG_SYSLOG);
    vsyslog(LOG_WARNING, format, args);
    closelog();
  }
  else {
    vprintf(format,args);
  }
  va_end(args);
  normalColor();
  return 0;
}

int errorf(const char *format, ...) {
  if(!initLog){
    initLogger("stdout");
  }
  va_list args;
  va_start(args, format);
  textColor(RESET,RED,BLACK);
  if(logMode){
    openlog("Logger", LOG_PID | LOG_CONS, LOG_SYSLOG);
    vsyslog(LOG_ERR, format, args);
    closelog();
  }
  else {
    vprintf(format,args);
  }
  va_end(args);
  normalColor();
  return 0;
}
int panicf(const char *format, ...) {
  if(!initLog){
    initLogger("stdout");
  }
  va_list args;
  va_start(args, format);
  textColor(RESET,YELLOW,BLACK);
  if(logMode){
    openlog("Logger", LOG_PID | LOG_CONS, LOG_SYSLOG);
    vsyslog(LOG_EMERG, format, args);
    closelog();
  }
  else {
    vprintf(format,args);
  }
  va_end(args);
  normalColor();
  return 0;
}
