#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char userID[16][32];
char pwdID[16][32];

void get_user_list(){
	int fd;
	int n;
	char buf[1024];

	fd=open("list.txt",O_RDONLY);
	n=read(fd,buf,sizeof(buf));

	printf(2,"%d  %d  %s\n",fd,n,buf);
	for(int i=0;i<16;i++){
		
	}
}

int check_idpw(char *user, char *pwd){
	for(int i=0;i<16;i++)
		if(!strcmp(userID[i],user)&&!strcmp(pwdID[i],pwd)) return 1;
//	return 0;
	return 1;
}

int main(int argc,char *argv[]){
	char user[512];
	char pwd[512];
	get_user_list();
	while(1){
		printf(2,"Username: ");
		gets(user,sizeof(user));
		printf(2,"Password: ");	
		gets(pwd,sizeof(pwd));
		if(check_idpw(user,pwd)) break;
		printf(2,"login failed. try again\n");
	}
	exec("sh",argv);
	
	return 0;
}
