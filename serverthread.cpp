#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <pthread.h>

/* You will to add includes here */

// returns the amount of chars in the char*[]
int checkForChar(char p[], const char searchingFor = '/')
{
  int size = strlen(p);
  int counter = 0;
  for (int i = 0; i < size; i++)
  {
    if (p[i] == searchingFor)
    {
      counter++;
    }
  }
  return counter;
};

int checkForCharPointerAddition(char looking[], const char searchingFor = '/')
{
  int counter = 0;
  char *p = nullptr;
  printf("searchingFor: %c\n", searchingFor);
  p = (char *)malloc(strlen(looking) * sizeof(char));
  memcpy(p, looking, strlen(looking));
  char *startPointer = p;

  for (int i = 0; i < strlen(looking); i++)
  {
    if (*p == searchingFor)
    {
      counter++;
    }
    p++;
  }
  free(startPointer);
  return counter;
};

void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET)
  {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
};

void *threadTest(void *arg)
{
  // typecast arg till den struct jag villl använda
  printf("thread test\n");
};

using namespace std;

int main(int argc, char *argv[])
{

  /* Do more magic */

  // if(argc < 0) FUNKAR INTE
  // {
  //   printf("not enough arguments\n");
  //   return(0);
  // }

  char *hoststring, *portstring, *rest, *org;
  org = strdup(argv[1]);
  rest = argv[1];
  hoststring = strtok_r(rest, ":", &rest);
  portstring = strtok_r(rest, ":", &rest);
  printf("Got %s \nsplit into %s and %s \n", org, hoststring, portstring);

  char remoteIP[INET6_ADDRSTRLEN];
  int yes = 1;                        // for setsockopt() SO_REUSEADDR, below
  struct sockaddr_storage remoteaddr; // client address
  int listener;                       // listener socket
  struct addrinfo hints, *ai, *p;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int rv;
  if ((rv = getaddrinfo(hoststring, portstring, &hints, &ai)) != 0)
  {
    fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
    exit(1);
  }

  for (p = ai; p != NULL; p = p->ai_next)
  {
    listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (listener < 0)
    {
      continue;
    }

    // lose the pesky "address already in use" error message
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
    {
      close(listener);
      continue;
    }

    break;
  }
  if (listen(listener, 10) == -1)
  {
    perror("Listener error\n");
    exit(1);
  }

  // thread test
  pthread_t testThread;
  void *ret_join;

  // pthread_create(thread,its attribute, the function to run, args to that function)
  pthread_create(&testThread, NULL, threadTest, NULL);
  // pthread_join(testThread, NULL);
  while (1)
  {
    // handle new connections
    socklen_t addrlen = sizeof remoteaddr;
    int newfd = accept(listener,
                       (struct sockaddr *)&remoteaddr,
                       &addrlen);

    if (newfd == -1)
    {
      perror("accept");
    }
    else
    {
      printf("new connection from %s on "
             "socket %d\n",
             inet_ntop(remoteaddr.ss_family,
                       get_in_addr((struct sockaddr *)&remoteaddr),
                       remoteIP, INET6_ADDRSTRLEN),
             newfd);
      pthread_create(&testThread, NULL, threadTest, NULL);
    }

  }



  return (0);
}
