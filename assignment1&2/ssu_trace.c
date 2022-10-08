#include "types.h"
#include "stat.h"
#include "user.h"
// #include "string.h"

int main(int argc, char *argv[]){
    int pid, wpid;
    int mask;

    if(argc<3){
        printf(2,"ssu_trace [mask] [command]\n");
        exit();
    }

    mask=atoi(argv[1]);
    
    for(;;){
      pid = fork();
      if (pid < 0){
        printf(1, "ssu_trace: fork failed\n");
        exit();
      }
      if (pid == 0){
        trace(mask);
        exec(argv[2], argv + 2);
        printf(1, "ssu_trace: exec %s failed\n", argv[2]);
        exit();
      }
      while ((wpid = wait()) >= 0 && wpid != pid)
        printf(1, "zombie!\n");

      exit();
    }
}