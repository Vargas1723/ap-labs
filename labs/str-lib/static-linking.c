#include <stdio.h>
#include <stdlib.h>
//#include "strlib.c"

int mystrlen(char *);
char *mystradd(char *,char *);
int mystrfind(char *,char *);


int main(int argc, char *argv[]) {
  char *string = (char *)malloc(sizeof(char *));
  char *string2 = (char *)malloc(sizeof(char *));
  char *r = (char *)malloc(sizeof(char *));
  string = argv[2];
  string2 = argv[3];
  if(mystrfind(argv[1],"-add")==0 && argc == 4){
    printf("Initial Lenght\t: %d\n", mystrlen(string));
    mystradd(string,string2);
    printf("New String\t: %s\n", string);
    printf("New lenght\t: %d\n", mystrlen(string));
    return 0;
  }
  else if(mystrfind(argv[1],"-find")==0 && argc == 4){
    printf("[%s] string was found at [%d] position\n", string2,mystrfind(string,string2));
  }
  else if(mystrfind(argv[1],"-add")==0 && argc != 4){
    printf("The correct way must be: -add \"First string\" \"Second string\"\n");
  }
  else if(mystrfind(argv[1],"-find")==0 && argc != 4){
    printf("The correct way must be: -find \"String\" \"String to search\"\n");
  }
  else {
    printf("The correct way must be: -add \"First string\" \"Second string\"\n");
    printf("The correct way must be: -find \"String\" \"String to search\"\n");
  }
  return 0;
}
