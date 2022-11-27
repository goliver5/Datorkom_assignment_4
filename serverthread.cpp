#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <cstring>

/* You will to add includes here */

//test

//returns the amount of chars in the char*[]
int checkForChar(char p[], const char searchingFor = '/')
{
  int size = strlen(p);
  int counter = 0;
  for (int i = 0; i < size; i++)
  {
      if(p[i] == searchingFor)
      {
        counter++;
      }
  }
  return counter;
}

int checkForCharPointerAddition( char looking[], const char searchingFor = '/')
{
  int counter = 0;
  char* p =nullptr;
  printf("searchingFor: %c\n",searchingFor);
  p = (char*)malloc(strlen(looking) * sizeof(char));
  memcpy(p,looking,strlen(looking));
  char* startPointer = p;

  for(int i = 0; i< strlen(looking); i++)
  {
    if(*p == searchingFor)
    {
      counter++;
    }
    p++;
  }
  printf("free ");
  free(startPointer);
  return counter;
}

using namespace std;

int main(int argc, char *argv[]){
  
  /* Do more magic */


  printf("strlen of argv[1]: %d\n",strlen(argv[1]));


  int nrOfSlashes = checkForChar(argv[1], '/');
  //int nrOfSlashes = checkForCharPointerAddition(argv[1]);
  printf("nrofSlashes: %d.\n", nrOfSlashes);
  return(0);


  
}
