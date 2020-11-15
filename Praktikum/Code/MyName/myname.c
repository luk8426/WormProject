#include <stdio.h>

int main() {
  
  int day=21, month=7, year=1984;
  char name[] = "Lars Tragl";

  printf("Mein Name ist %s\n",name);
  printf("Mein Geburtsdatum ist der %02d.%02d.%04d\n",day,month,year);
  
  return 0;
}


//Gewünschte Ausgabe:
//
//Mein Name ist Lars Tragl
//Mein Geburtsdatum ist der 21.07.1984
