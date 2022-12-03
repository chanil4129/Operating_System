#include "types.h"
#include "param.h"
#include "stat.h"
#include "mmu.h"
#include "proc.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "fcntl.h"
#include "user.h"

#define BUFMAX 1024

void write_to_norm(char* fname);

int main()
{
	int fd;
	int i, size;
	char buf[BUFMAX];
	char* fname = "test_cs";

	if ((fd = open(fname, O_CREATE | O_CS | O_RDWR)) <= 0) {
		printf(2, "ERROR: open failed in cs file\n");
		exit();
	}
	for (i = 0; i < BUFMAX; i++)
		buf[i] = 'a';
	for (i = 0; i < 2; i++) {
		if ((size = write(fd, buf, BUFMAX)) != BUFMAX) {
			if(size==-2){
				printf(2,"ERROR: oversize\n");
			}
			else{
				printf(2, "ERROR: write failed in cs file\n");
			}
			exit();
		}

		if (i == 50)
			write_to_norm("test_norm");
		// if (i == 52)
		// 	write_to_norm("test_norm1");	
		// if (i == 54)
		// 	write_to_norm("test_norm2");	
		// if (i == 56)
		// 	write_to_norm("test_norm3");	
		// if (i == 58)
		// 	write_to_norm("test_norm4");
		// if (i == 60)
		// 	write_to_norm("test_norm5");	
		// if (i == 62)
		// 	write_to_norm("test_norm6");	
		// if (i == 64)
		// 	write_to_norm("test_norm7");	
		// if (i == 66)
		// 	write_to_norm("test_norm8");	
		// if (i == 68)
		// 	write_to_norm("test_norm9");	
		// if (i == 70)
		// 	write_to_norm("test_norm10");	
		// if (i == 72)
		// 	write_to_norm("test_norm11");		
		
	}
	printinfo(fd, fname);
	
	close(fd);
	exit();
}

void write_to_norm(char* fname)
{
	int fd;
	int i, size;
	char buf[BUFMAX];

	if ((fd = open(fname, O_CREATE | O_RDWR)) <= 0) {
		printf(2, "ERROR: open failed in normal file\n");
		exit();
	}

	for (i = 0; i < BUFMAX; i++)
		buf[i] = 'b';

	for (i = 0; i < 2; i++) {
		if ((size = write(fd, buf, BUFMAX)) != BUFMAX) {
			printf(2, "ERROR: write failed in normal file\n");
			exit();
		}
	}

	printinfo(fd, fname);
	
	close(fd);
}
