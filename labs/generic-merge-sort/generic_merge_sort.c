#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_LENGTH 32
#define OUTPUT_FILE "mergeSortOutput.txt"

typedef struct {
  char **array;
  size_t used;
  size_t size;
} Array;

void initArray(Array *a,size_t initialSize){
  a->array = malloc(initialSize * LINE_LENGTH * sizeof(char));
  a->used = 0;
  a->size = initialSize;
}
void insertArray(Array *a, char *element){
  if(a->used == a->size){
    a->size *= 2;
    a->array = realloc(a->array,a->size * LINE_LENGTH * sizeof(char));
  }
  a->array[a->used] = malloc(LINE_LENGTH * sizeof(char));
  strcpy(a->array[a->used++],element);
}
void freeArray(Array *a){
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
}
void merge(void *a[], int start, int mid, int end,int (*comp)(void *, void *)){
    int start2 = mid + 1;
    if((*comp)(a[mid],a[start2]) <= 0){
      return;
    }
    while (start <= mid && start2 <= end){
      if((*comp)(a[start],a[start2]) <= 0){
        start++;
      }
      else{
        void *value;
        value = a[start2];
        int index = start2;

        while(index != start){
          a[index] =a[index-1];
          index--;
        }
        a[start] = value;
        start++;
        mid++;
        start2++;
      }
    }
}
void mergeSort(void *a[], int l, int r,int (*comp)(void *, void *)){
  if(l < r){
    int m = l +(r - l) / 2;

    mergeSort(a,l,m,comp);
    mergeSort(a,m+1,r,comp);

    merge(a,l,m,r,comp);
  }
}
int numcmp(char *, char *);

int main(int argc, char **argv){
  FILE *fp;
  char *line;
  size_t bufsize = 32;
  line = (char *)malloc(bufsize * sizeof(char));

  if(argc == 2 || argc == 3){
    int file = 1;
    int numeric = 0;
    if(strcmp(argv[1],"-n") == 0){
      if(argc != 3){
        printf("Incomplete parameters\n");
        return 0;
      }
      file = 2;
      numeric = 1;
    }
    fp = fopen(argv[file],"r+");
    if(fp != NULL){
      Array array;
      initArray(&array, 50);
      while(getline(&line,&bufsize,fp) >= 0){
        insertArray(&array,line);
      }
      fclose(fp);
      int i;
      mergeSort((void **)array.array,0,array.used-1,(int (*)(void*,void*))(numeric ? numcmp : strcmp));
      fp = fopen(OUTPUT_FILE,"w");
      for(i = 0;i < array.used;i++){
        fprintf(fp,"%s",array.array[i] );
      }
      freeArray(&array);
      fclose(fp);
    }
    else {
      printf("The file can not be open\n");
    }
  }
  else{
    printf("Missing parameters\n");
    return 0;
  }
}
int numcmp(char *s1,char *s2){
  double v1, v2;
  v1 = atof(s1);
  v2 = atof(s2);
  if(v1 < v2)
    return -1;
  else if (v1 > v2)
    return 1;
  else
    return 0;
}
