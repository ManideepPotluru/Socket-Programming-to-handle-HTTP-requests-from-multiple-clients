#define MAX_LEN 512

//We use this util.h to call the read_line function
int read_line(int sock, char *buffer, int max) {
    int i = 0;
    while(i < max-1) {
        
        int num = recv(sock, buffer, 1, 0);
        if(num == 0) continue; 
        else if(num < 0) break; 
        if(*buffer == '\r') continue; 
        else if(*buffer == '\n') break; 
        ++buffer;
        ++i;
    }
    *buffer = 0; 
    return i;
}
