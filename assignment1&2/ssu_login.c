#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char userID[16][32];
char pwdID[16][32];

//"list.txt" 파일 정보를 받아와 userID, pwdID에 저장
void get_user_list(){
	int fd;
	int n;
	char buf[1024];
	const char *t=buf;

	fd=open("list.txt",O_RDONLY);
	n=read(fd,buf,sizeof(buf));

	for(int i=0;i<16;i++){
		char *s=userID[i];
		if(n<=0) break;
		while(n--){
			//nul
			if(*t=='\0'){
				*s=0;
				break;
			}
			//SpaceBar
			if(*t==' ') {
				*s=0;
				t++;
				s=pwdID[i];
			}
			if(*t=='\n'){
				*s=0;
				t++;
				break;
			}
			*s++=*t++;
		}
	}
}

//ID, Password 확인
int check_idpw(char *user, char *pwd){
	for(int i=0;i<16;i++)
		if(!strcmp(userID[i],user)&&!strcmp(pwdID[i],pwd)) return 1;
	return 0;
}

int main(int argc,char *argv[]){
	char user[512]; //Username 입력
	char pwd[512]; //Password 입력
	int pid,wpid;
	get_user_list();
	while(1){
		printf(2,"Username: ");
		gets(user,sizeof(user));
		user[strlen(user)-1]=0;
		printf(2,"Password: ");	
		gets(pwd,sizeof(pwd));
		pwd[strlen(pwd)-1]=0;
		if(check_idpw(user,pwd)) break;
		printf(2,"login failed. try again\n");
	}
	pid=fork();
	if(pid<0){
		printf(1,"ssu_login: fork failed\n");
		exit();
	}
	if(pid==0){
		exec("sh",argv);
		printf(1,"ssu_login: exec sh failed\n");
		exit();
	}
	while((wpid=wait()) >= 0 && wpid != pid)
      printf(1, "zombie!\n");
	
	return 0;
}
