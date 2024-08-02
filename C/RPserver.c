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

int fd, j, sock_server, sock_client, size, yes = 1, rx;
void *cfg;
char *name = "/dev/mem";
struct sockaddr_in addr;
uint32_t command, value, tmp;
uint32_t count;// Hz
uint32_t freq;
uint32_t count_quater_period;
uint32_t delay;
uint32_t intcommand;
uint32_t userInput=0;
if((fd = open(name, O_RDWR)) < 0)
  {
    perror("open");
    return EXIT_FAILURE;
  }

cfg = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x41200000);

while (1){
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
    count = *((uint32_t *)(cfg + 8));
    freq=250000000/count;

    count_quater_period=100000/(4*freq);//Lets say this will be max 16 bits
    if((sock_client = accept(sock_server, NULL, NULL)) < 0)
    {
      perror("accept");
      printf("I am at the if with the return\n");
      break;
    }
    while (1){
    rx = recv(sock_client, (char *)&command, 2, MSG_DONTWAIT);
      if (rx == 0) {
	 printf("no command\n");
	 break;
      }
      if (rx>0){
        char substring[2];
        strncpy(substring, (char *)&command, 2);
        intcommand=atoi(substring)&0xf;
        if (intcommand>0x8){
        userInput=0x1;
        delay=(intcommand-0x8)*count_quater_period;}
        else {
        userInput=0x0;
        delay=100;}//This value does not matter as long as it is not 0

        printf("%d\n", delay);
        *((uint32_t *)(cfg + 0)) = (userInput<<16)+(delay);
        break;
      }
}
close(sock_client);
}

}
return 0;
}

