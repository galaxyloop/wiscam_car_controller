#include<stdio.h>  
#include<stdlib.h>  
#include<string.h>  
#include<errno.h>  
#include<sys/types.h>  
#include<sys/socket.h>  
#include<netinet/in.h>  
#include <signal.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define KEYCODE_R 0x43 
#define KEYCODE_L 0x44
#define KEYCODE_U 0x41
#define KEYCODE_D 0x42
#define KEYCODE_Q 0x71

int kfd = 0;
int sfd;
struct termios cooked, raw;

void quit(int sig)
{
	(void)sig;
	tcsetattr(kfd, TCSANOW, &cooked);
	exit(0);
}

void keyLoop()
{
	char c;
	char cmd = 0;
	static char last_key = 0;
	static unsigned int counter = 0;

	tcgetattr(kfd, &cooked); 
	memcpy(&raw, &cooked, sizeof(struct termios));
	raw.c_lflag &=~ (ICANON | ECHO);
	// Setting a new line, then end of file                         
	raw.c_cc[VEOL] = 1;
	raw.c_cc[VEOF] = 2;
	tcsetattr(kfd, TCSANOW, &raw);

	for(;;)
	{
		// get the next event from the keyboard  
		if(read(kfd, &c, 1) < 0)
		{
			perror("read():");
			exit(-1);
		}

		switch(c)
		{
			case KEYCODE_L:
				if (last_key == KEYCODE_L) {
					counter++;
				} else {
					counter = 0;
				}
				fprintf(stderr, "\rLEFT  %.10d", counter);
				cmd = '3';
				write(sfd,&cmd, 1); 
				last_key = c;
				break;
			case KEYCODE_R:
				if (last_key == KEYCODE_R) {
					counter++;
				} else {
					counter = 0;
				}
				fprintf(stderr, "\rRIGHT %.10d", counter);
				cmd = '4';
				write(sfd,&cmd, 1); 
				last_key = c;
				break;
			case KEYCODE_U:
				if (last_key == KEYCODE_U) {
					counter++;
				} else {
					counter = 0;
				}
				fprintf(stderr, "\rUP    %.10d", counter);
				cmd = '2';
				write(sfd,&cmd, 1); 
				last_key = c;
				break;
			case KEYCODE_D:
				if (last_key == KEYCODE_D) {
					counter++;
				} else {
					counter = 0;
				}
				fprintf(stderr, "\rDOWN  %.10d", counter);
				cmd = '1';
				write(sfd,&cmd, 1); 
				last_key = c;
				break;
			default:
				break;
				/* printf("value: %c = 0x%02X = %d\n", c, c, c); */
		}
	}
	return;
}

int tcp_connect(const char *dest, const char *port)
{
	int    sockfd;  
	struct sockaddr_in    servaddr;  

	if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){  
		printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);  
		exit(0);  
	}  

	memset(&servaddr, 0, sizeof(servaddr));  
	servaddr.sin_family = AF_INET;  
	servaddr.sin_port = htons(atoi(port));//端口号  
	if (inet_pton(AF_INET, dest, &servaddr.sin_addr) <= 0){  
		printf("inet_pton error for %s\n",dest);  
		exit(0);  
	}  

	if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){  
		printf("connect error: %s(errno: %d)\n",strerror(errno),errno);  
	}  
	return sockfd;
}

int main(int argc, const char *argv[])
{
	signal(SIGINT,quit);
	if (argc != 3){  
		printf("usage:<ipaddress> <port>\n");  
		exit(0);  
	}  
	system("clear");
	fprintf(stderr, "\n\n\n\n**********************WisCam Car Test***********************\n");
	fprintf(stderr, "***************************Start****************************\n");

	sfd = tcp_connect(argv[1], argv[2]);
	keyLoop();

}
