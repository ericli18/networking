#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT "8080"
#define MAXBUFSIZE 100

void* getaddr(struct sockaddr* sa)
{
  if(sa->sa_family == AF_INET)
  {
    return &(((struct sockaddr_in *)sa) -> sin_addr);
  }
  
  return &(((struct sockaddr_in6*)sa) -> sin6_addr);
}

int main() {
  int status;
  struct addrinfo hints, *servinfo, *curr;
  int sockfd;
  int yes = 1;
  char buf[MAXBUFSIZE];
  int numBytes;
  char s[INET6_ADDRSTRLEN];
  
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;

  if ((status = getaddrinfo("localhost", PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    return 1;
  }

  for (curr = servinfo; curr != NULL; curr = curr->ai_next) {
    if ((sockfd = socket(curr->ai_family, curr->ai_socktype,
                         curr->ai_protocol)) == -1) {
      perror("socket: ");
      continue;
    }

    if (connect(sockfd, curr->ai_addr, curr->ai_addrlen) == -1) {
      perror("connect: ");
      close(sockfd);
      continue;
    }

    break;
  }

  if (curr == NULL) {
    fprintf(stderr, "connect: socket failed to connect");
    exit(1);
  }
  
  inet_ntop(curr->ai_family, getaddr(curr->ai_addr), s, sizeof(s));
  
  printf("connected to: %s\n", s);

  freeaddrinfo(servinfo);

  if ((numBytes = recv(sockfd, buf, MAXBUFSIZE - 1, 0)) == -1) {
    perror("recv: ");
    exit(1);
  }
  
  buf[numBytes] = '\0';

  printf("recieved: %s\n", buf);

  close(sockfd);

  return 0;
}
