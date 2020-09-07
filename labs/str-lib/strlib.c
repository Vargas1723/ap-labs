
#include <stdlib.h>

int mystrlen(char *str){
    int count = 0;
    while (str[count] != '\0'){
      count++;
    }
    return count;
}

char *mystradd(char *origin, char *addition){
    int i;
    int count = mystrlen(origin);
    int additionlen = mystrlen(addition);
    for(i = 0;i < additionlen;i++){
      *(origin+count++) = *(addition+i);

    }
    *(origin+count) = '\0';
    return origin;
}

int mystrfind(char *origin, char *substr){
    int limit = mystrlen(substr);
    int originlen = mystrlen(origin);
    int count = 0;
    int i;
    for(i = 0;i < originlen;i++){
      if(*(origin+i) == *(substr+count)){
        count++;
      }
      else {
        i = i - count + 1;
        count = 0;
      }
      if(count==limit) return i - count + 1;
    }
    return -1;
}
