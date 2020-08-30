#include <stdio.h>
#include <stdlib.h>

#define IN   1   /* inside a word */
#define OUT  0   /* outside a word */
#define ARRAY_LENGHT 50

int main(){
  int c, i, tmp, state;
  state = OUT;
  char *word = (char *)malloc(sizeof(char)*20);
  int count = 0;

  while ((c = getchar()) != EOF) {
        if (c == ' ' || c == '\n' || c == '\t'){
          state = OUT;
          for (i = 0;  i < count/2; i++) {
      	     tmp = word[i];
      	     word[i] = word[count - i - 1];
      	     word[count - i - 1] = tmp;
          }
          count = 0;
          printf("%s ",word);
        }
        else if (state == OUT) {
          state = IN;
          word = (char *)malloc(sizeof(char)*20);
        }
        if(state == IN){
          *(word+count++) = c;
        }
  }
  printf("\n");
  free(word);
  return 0;
}
