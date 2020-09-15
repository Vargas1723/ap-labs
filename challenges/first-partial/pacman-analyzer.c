#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define REPORT_FILE "packages_report.txt"

typedef struct s{
  char date[24];
  int datesize;
  char manager[14];
  char operation[150];
  char name[20];
} Oper;
typedef struct {
  char name[20];
  char installDate[24];
  char lastUpdate[24];
  int updates;
  int datesize;
  bool removed;
  char removalDate[24];
} Package;

void analizeLog(char *logFile, char *report);
void analizeLog2(char *logFile, int lines,char *report);

int main(int argc, char **argv) {
    if (argc < 2) {
	     printf("Usage:./pacman-analizer.o \n");
	      return 0;
    }
    if(strcmp(argv[1],"-input")==0){
      analizeLog(argv[2], argv[3]);
    }
    else {
      printf("Usage:./pacman-analizer.o \n");
      printf("Wrong parameters\n");
      return 0;
    }
    return 0;
}
int numberElements(char array[]){
  int i;
  int count = 0;
  for (i = 0; i < 35;i++){
    if( array[i] != NULL) count++;
    else return count;
  }
}
int compareArray(char array[],char array2[],int size){
  int i;
  for(i = 0;i < size;i++){
    //printf("%c - %c\n",array[i],array2[i] );
    if(array[i]!=array2[i]) return 0;
  }
  return 1;
}
void analizeLog2(char *logFile, int lines,char *report){
  Package packages[lines];
  int count = 0;
  // Files variables
  int src;
  char buffer;
  // Reader variables
  int date = 0;
  int manager = 0;
  int operation = 0;
  int brackets = 0;
  int space = 0;
  int name = 0;
  Oper *oper1;
  oper1 = (Oper *)malloc(sizeof(Oper));
  // Open read only file
  src = open(logFile, O_RDONLY);
  if(src < 0){
    printf("Can not be open: %s\n",logFile);
    exit(1);
  }
  //
  while(read(src,&buffer,1)){
    if(buffer == '[') brackets++;
    else if(buffer == ']') brackets++;
    else if(buffer == '\n') {
      date = 0;
      manager = 0;
      operation = 0;
      brackets = 0;
      space = 0;
      name = 0;

      if(strstr(oper1->manager, "ALPM") != NULL && strstr(oper1->operation,"reinstalled") != NULL){

      }
      else if(strstr(oper1->manager, "ALPM") != NULL && strstr(oper1->operation,"installed") != NULL){
        int i;
        for(int i=0;i<20;i++) packages[count].name[i]=oper1->name[i];
        for(int i=0;i<24;i++) packages[count].installDate[i]=oper1->date[i];
        packages[count].datesize=oper1->datesize;
        packages[count].updates=0;
        count++;
      }
      else if(strstr(oper1->manager, "ALPM") != NULL && strstr(oper1->operation,"upgraded") != NULL){
        int i;
        for(i = 0;i < count;i++){
          if(compareArray(packages[i].name,oper1->name,20) == 1){
            int j;

            packages[i].updates += 1;
            for(int j=0;j<24;j++) packages[i].lastUpdate[j]=oper1->date[j];
          }
        }
      }
      else if(strstr(oper1->manager, "ALPM") != NULL && strstr(oper1->operation,"removed") != NULL){
        int i;
        for(i = 0;i < count;i++){
          if(compareArray(packages[i].name,oper1->name,20) == 1){
            int j;
            for(int j=0;j<24;j++) packages[i].removalDate[j]=oper1->date[j];
            packages[i].removed=true;
          }
        }
      }
      memset(oper1->name,0,20);
      memset(oper1->manager,0,14);
      memset(oper1->operation,0,150);
    }
    else if( brackets == 1){
      oper1->date[date++]=buffer;
    }
    else if( brackets == 3){
      oper1->manager[manager++]=buffer;
    }
    else if( brackets == 4){
      oper1->operation[operation++]=buffer;
      if(buffer==' ') space++;
      else if(space == 2){
        if(name<20)oper1->name[name++]=buffer;
      }
    }
  }
  close(src);
  int dest;
  if((dest=open(report,O_RDWR))<0){
    printf("Can not be open: %s\n",report);
  }
  lseek(dest,0,SEEK_END);
  write(dest,"List of Packages\n",17);
  write(dest,"----------------------\n",23);
  int i;
  char text[70];
  char text1[70];
  char text2[90];
  char text3[70];
  char text4[70];
  for(int i = 0;i<lines;i++){
    sprintf(text,"\n- Package Name : %.20s",packages[i].name);
    if(packages[0].datesize==16){
      sprintf(text1,"\n - Install date \t: %.16s",packages[i].installDate);
    }
    else{
      sprintf(text1,"\n - Install date \t: %.24s",packages[i].installDate);
    }

    if(packages[i].updates == 0){
      sprintf(text2,"\n - Last update date \t : -");
    }
    else {
      if(packages[0].datesize==16){
        sprintf(text2,"\n - Last update date \t : %.16s",packages[i].lastUpdate);
      }
      else{
        sprintf(text2,"\n - Last update date \t : %.24s",packages[i].lastUpdate);
      }
    }
    sprintf(text3,"\n - How many updates \t : %d",packages[i].updates);
    if(packages[i].removed == true){
      if(packages[0].datesize==16){
        sprintf(text4,"\n - Removal date \t : %.16s",packages[i].removalDate);
      }
      else {
        sprintf(text4,"\n - Removal date \t : %.24s",packages[i].removalDate);
      }
    }
    else {
      sprintf(text4,"\n - Removal date \t : -");
    }
    write(dest,text,numberElements(text));
    write(dest,text1,numberElements(text1));
    write(dest,text2,numberElements(text2));
    write(dest,text3,numberElements(text3));
    write(dest,text4,numberElements(text4));
    memset(text,0,70);
    memset(text1,0,70);
    memset(text2,0,70);
    memset(text3,0,70);
    memset(text4,0,70);
  }
  close(dest);
  free(oper1);
}
void analizeLog(char *logFile, char *report) {
    printf("Generating Report from: [%s] log file\n", logFile);
    // Files variables
    int src;
    int destination;
    char buffer;
    // Report variables
    int installed = 0;
    int reinstalled = 0;
    int removed = 0;
    int upgraded = 0;
    // Reader variables
    int manager = 0;
    int operation = 0;
    int brackets = 0;
    int space = 0;
    int name = 0;
    Oper *oper1;
    oper1 = (Oper *)malloc(sizeof(Oper));
    // Open read only file
    src = open(logFile, O_RDONLY);
    if(src < 0){
      printf("Can not be open: %s\n",logFile);
      exit(0);
    }
    // Create write file
    if((destination = creat(report,0600)) < 0){
      printf("Error creating destination file: %s\n", report);
      exit(0);
    }
    //
    while(read(src,&buffer,1)){
      if(buffer == '[') brackets++;
      else if(buffer == ']') brackets++;
      else if(buffer == '\n') {
        manager = 0;
        operation = 0;
        brackets = 0;
        space = 0;
        name = 0;
        if(strstr(oper1->manager, "ALPM") != NULL && strstr(oper1->operation,"reinstalled") != NULL){
          reinstalled++;
        }
        else if(strstr(oper1->manager, "ALPM") != NULL && strstr(oper1->operation,"installed") != NULL){
          installed++;
        }
        else if(strstr(oper1->manager, "ALPM") != NULL && strstr(oper1->operation,"upgraded") != NULL){
          upgraded++;
        }
        else if(strstr(oper1->manager, "ALPM") != NULL && strstr(oper1->operation,"removed") != NULL){
          removed++;
        }
        memset(oper1->name,0,20);
        memset(oper1->date,0,24);
        memset(oper1->manager,0,14);
        memset(oper1->operation,0,150);
      }
      else if( brackets == 3){
        oper1->manager[manager++]=buffer;
      }
      else if( brackets == 4){
        oper1->operation[operation++]=buffer;
        if(buffer==' ') space++;
        else if(space == 2){
          if(name<20)oper1->name[name++]=buffer;
        }
      }
    }
    // Write in the file first results
    char text1[33];
    sprintf(text1,"- Installed packages \t : %d\n",installed);
    char text2[33];
    sprintf(text2,"- Removed packages \t : %d\n",removed);
    char text3[33];
    sprintf(text3,"- Upgraded packages \t : %d\n",upgraded);
    char text4[33];
    sprintf(text4,"- Current packages \t : %d\n",installed-removed);
    write(destination,"Pacman Packages Report\n",23);
    write(destination,"----------------------\n",23);
    write(destination,text1,numberElements(text1));
    write(destination,text2,numberElements(text2));
    write(destination,text3,numberElements(text3));
    write(destination,text4,numberElements(text4));
    printf("Report is generated at: [%s]\n", report);
    // Close files
    free(oper1);
    close(src);
    close(destination);
    analizeLog2(logFile,installed,report);
}
