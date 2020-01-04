#include<stdio.h>
#include<errno.h>  
#include<fcntl.h>   
#include <unistd.h> 


int main(){
    int fd;
    printf("Starting device test code example...\n");
    fd = open("/dev/myChrDev", O_RDONLY);           
    if (fd < 0) {
        perror("Failed to open the device...");
        return errno;
    }
    char s[256]={0};
    int n;
    printf("Input numbers of bytes will be recived from device: ");
    scanf("%d",&n);
    read(fd,s,n);
    
    printf("message from device: %s\n", s);

    printf("End of the program\n");
    return 0;
}