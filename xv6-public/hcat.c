#include "types.h"
#include "stat.h"
#include "user.h"

char buf[512];
int row; //to print row num

void
cat(int fd)
{
  int n;
  int cnt=0;

  while((n = read(fd, buf, 1)) > 0) {
    if (write(1, buf, n) != n) {
      printf(1, "cat: write error\n");
      exit();
    }
	  if(buf[0]=='\n') cnt++;
	  if(cnt==row) break;
  }
  if(n < 0){
    printf(1, "cat: read error\n");
    exit();
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc <= 1){
    cat(0);
    exit();
  }
  row=atoi(argv[1]);

  for(i = 2; i < argc; i++){
    if((fd = open(argv[i], 0)) < 0){
      printf(1, "cat: cannot open %s\n", argv[i]);
      exit();
    }
    cat(fd);
    close(fd);
  }
  exit();
}