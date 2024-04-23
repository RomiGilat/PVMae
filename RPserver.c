#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

int main(int argc, char *argv[]){

int fd, j, sock_server, sock_client, size, yes = 1, nsmpl, navg, rx;
void *cfg, *dat;
char *name = "/dev/mem";
uint32_t naverages=0, nsamples=0, timing=0;
struct sockaddr_in addr;
uint32_t command, value, tmp;
uint32_t buffer[8192];
clock_t time_begin;
double time_spent;
int measuring = 0;

if((fd = open(name, O_RDWR)) < 0)
  {
    perror("open");
    return EXIT_FAILURE;
  }

cfg = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x42000000);

if((sock_server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("socket");
    return EXIT_FAILURE;
  }
setsockopt(sock_server, SOL_SOCKET, SO_REUSEADDR, (void *)&yes , sizeof(yes));

memset(&addr, 0, sizeof(addr));
addr.sin_family = AF_INET;
addr.sin_addr.s_addr = htonl(INADDR_ANY);
addr.sin_port = htons(1001);

if(bind(sock_server, (struct sockaddr *)&addr, sizeof(addr)) < 0){
    perror("bind");
    return EXIT_FAILURE;
  }

listen(sock_server, 1024);
printf("Listening on port 1001 ...\n");

while(1){
    if((sock_client = accept(sock_server, NULL, NULL)) < 0)
    {
      perror("accept");
      return EXIT_FAILURE;
    }
    while (1){
    rx = recv(sock_client, (char *)&command, 4, MSG_DONTWAIT);
      if (rx == 0) {
         break;
         measuring = 0;
      }
      if (rx>0){

        value = command & 0xffffffff
        *((int32_t *)(cfg + 0)) = value;
        //break;
      }
}
}


return 0;
}
