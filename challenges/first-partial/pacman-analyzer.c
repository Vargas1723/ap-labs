#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#define REPORT_FILE "packages_report.txt"

typedef struct s{
  char date[16];
  char manager[14];
  char operation[150];
} Oper;


void analizeLog(char *logFile, char *report);

int main(int argc, char **argv) {

    if (argc < 2) {
	     printf("Usage:./pacman-analizer.o \n");
	      return 1;
    }

    analizeLog(argv[1], REPORT_FILE);

    return 0;
}

void analizeLog(char *logFile, char *report) {
    printf("Generating Report from: [%s] log file\n", logFile);
    // Files variables
    int src;
    int destination;
    char buffer;
    // Report variables
    int installed = 0;
    int removed = 0;
    int upgraded = 0;
    // Reader variables
    int date = 0;
    int manager = 0;
    int operation = 0;
    int brackets = 0;
    Oper *oper1;
    oper1 = (Oper *)malloc(sizeof(Oper));

    // Open read only file
    src = open(logFile, O_RDONLY);
    if(src < 0){
      printf("Can not be open: %s\n",logFile);
      exit(1);
    }
    // Create write file
    if((destination = creat(report,0600)) < 0){
      printf("Error creating destination file: %s\n", report);
      exit(2);
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


        if(oper1->manager[0] == 'A' && oper1->operation[1] == 'i'){
          installed++;
        }
        else if(oper1->manager[0] == 'A' && oper1->operation[1] == 'u'){
          upgraded++;
        }
        else if(oper1->manager[0] == 'A' && oper1->operation[2] == 'e'){
          removed++;
        }
      }

      else if( brackets == 1){
        oper1->date[date++]=buffer;
      }
      else if( brackets == 3){
        oper1->manager[manager++]=buffer;
      }
      else if( brackets == 4){
        oper1->operation[operation++]=buffer;
      }
    }


    // Close files
    free(oper1);
    close(src);
    close(destination);
    printf("Installed elements: [%d]\n", installed);
    printf("Removed elements: [%d]\n", removed);
    printf("Upgraded elements: [%d]\n", upgraded);
    printf("Current elements: [%d]\n", installed-removed);
    printf("Report is generated at: [%s]\n", report);
}
