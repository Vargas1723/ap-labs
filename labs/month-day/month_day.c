#include <stdio.h>
#include <stdlib.h>

static char daytab[2][13] = { {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31} };
/* month_name:  return name of n-th month */
char *month_name(int n) {

   static char *name[] = {
       "Illegal month",
       "January", "February", "March",
       "April", "May", "June",
       "July", "August", "September",
       "October", "November", "December"
   };

   return (n < 1 || n > 12) ? name[0] : name[n];
}
/* month_day function's prototype*/
void month_day(int year, int yearday, int *pmonth, int *pday){
  int i, leap;
  leap = year%4 == 0 && year%100 != 0 || year%400 == 0;

  int dayscount = 0;
  int month = 0;
  for (i = 1; i < 13; i++){
    if(dayscount+daytab[leap][i] >= yearday){
      month = i;
      yearday -= dayscount;
      break;
    }
    else {
      dayscount+=daytab[leap][i];
    }
  }
  *pmonth = month;
  *pday = yearday;
}
int main(int argc, char** argv) {
    int year = atoi(argv[1]);
    int yearday = atoi(argv[2]);
    if(yearday <= 0 || yearday > 366){
      printf("Illegal number of days");
      return 0;
    }
    int *pmonth = malloc(sizeof(int));
    int *pday = malloc(sizeof(int));
    month_day(year,yearday,pmonth,pday);
    printf("%s %d, %d", month_name(*pmonth),*pday,year);
    return 0;
}
