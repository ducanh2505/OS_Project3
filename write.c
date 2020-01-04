#include<stdio.h>
#include<errno.h>  
#include<fcntl.h>   
#include <unistd.h> 
#include <string.h>


int main(){
    int fd;
    printf("Starting device test code example...\n");
    fd = open("/dev/myChrDev", O_WRONLY);           
    if (fd < 0) {
        perror("Failed to open the device...");
        return errno;
    }
    char s[256]={0};
    printf("Input message for device: ");
    gets(s);
    write(fd,s,strlen(s));
    
    printf("message sent device: %s\n", s);

    printf("End of the program\n");
    return 0;
}