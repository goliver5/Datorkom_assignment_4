#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <cstring>

/* You will to add includes here */

using namespace std;

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

void handleRequest(int sockfd, char *fileName)
{
  // printf("Opening File: {%s} \n", fileName);

  int length;
  // std::ifstream file(fileName, ios::binary);
  FILE *file = fopen(fileName, "rb");

  int size = 0;
  int n = 0;
  int count = 0;

  // nr of bytes

  if (file != NULL)
  {
    printf("Opened file %s\n", fileName);
    //char ok[] = "HTTP/1.1 200 OK \r\n\r\n";
    // sending msg back to client
    // if (send(sockfd, ok, sizeof(ok), 0) == -1)
    // {
    //   //printf("sending message error\n");
    // }
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    // printf("size: %d", size);
    fseek(file, 0, SEEK_SET);

    char buffer[size + 1];
    memset(buffer, 0, sizeof(buffer));
    while (!feof(file))
    {
      n = fread(buffer, 1, sizeof(buffer), file);
      count += n;
    }

    printf("closing file\n");
    fclose(file);

    // printf("count: %d", count);
    // printf("Buffer: {%s}\n", buffer);

    char buf[20000];
    memset(buf, 0, sizeof(buf));

    sprintf(buf, "HTTP/1.1 200 OK\r\n\r\n%s", buffer);

    // sending msg back to client
    if (send(sockfd, buf, strlen(buf), 0) == -1)
    {
      // printf("sending message error\n");
    }
    else
    {
      //printf("Sent buffer size of buffer: %s\n", buf);
    }
  }
  else
  {
    // Couldnt open requested file
    // printf("Couldnt open requested file\n");
  }
};

void handleTheConnection(int &sockfd)
{

  // typecasting the argument to the data thats in the argument

  char buf[100];
  memset(buf, 0, sizeof(buf));

  if (recv(sockfd, buf, sizeof(buf), 0) == -1)
  {
    //printf("recv error\n");
  }

  //printf("recv buf: {%s}\n", buf);
  char *token = buf;
  char *method = strtok_r(token, "/", &token);

  // if the filename token returns null the given char is invalid
  if (method == NULL)
  {
    //printf("method token returned NULL, the given char is invalid\n");
    return;
  }

  // printf("after strtok_r token: {%s}\n", token);
  // printf("data1: {%s}\n", method);

  if (strcmp(method, "GET ") == 0)
  {
    char *fileName = strtok_r(token, " ", &token);
    // if the filename token returns null the given char is invalid
    if (fileName == NULL)
    {
      //printf("Filename token returned NULL, the given char is invalid\n");
      return;
    }
    //printf("Token: {%s}\n", token);
    char *httpProtocol = strtok_r(token, "\n", &token);

    if (httpProtocol == NULL)
    {
      //printf("httpProtocol token returned NULL, the given char is invalid\n");
      return;
    }


    int nrOfSlashes = checkForChar(fileName, '/');
    // printf("nrOfSlashes: %d\n", nrOfSlashes);
    if (nrOfSlashes > 3)
    {
     // printf("Given char contains more than 3 '/'\n");
      return;
    }

      handleRequest(sockfd, fileName);

  }
  else
  {
    //printf("Wrong method\n");
  }

};

void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET)
  {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
};

int main(int argc, char *argv[])
{

  /* Do more magic */

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
  pid_t pid;

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

  signal(SIGPIPE, SIG_IGN);

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

      pid = fork();
      if (pid == 0)
      {
        handleTheConnection(newfd);
        //printf("Done with the fork\n");
        close(newfd);
        return 0;
      }
      pid = wait(NULL);
      close(newfd);
    }
  }

  //printf("done.\n");
  return (0);
}
