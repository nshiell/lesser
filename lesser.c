#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE    1

void get_screen_contents() {
    //system ("tput smcup; clear; sleep 5; tput rmcup");
    system ("tput smcup; clear");
    //system ("tput smcup; tput rmcup > ~/.lesser-console");
}

void draw_screen_contents() {
    //system ("tput smcup; clear; sleep 5; tput rmcup");
    system ("tput rmcup");
    //system ("tput smcup; tput rmcup > ~/.lesser-console");
}

void create_view(char **win) {
    int width = 60;
    int height = 30;
    
    int i;

    for (i = 0; i < width; i++) {
        *win = strcat(*win, "_");
    }
}



int main(int argc, char **argv) {
    unsigned char     buffer[BUFFERSIZE];
    FILE                         *instream;
    int                            bytes_read=0;
    int                            buffer_size=0;

    get_screen_contents();
    draw_screen_contents();
    
    char *win;
    win  = (char*)malloc(10);

    create_view(&win);
    printf("%s", win);

    buffer_size=sizeof(unsigned char)*BUFFERSIZE;
    /* open stdin for reading */
    instream=fopen("/dev/stdin","r");

    /* did it open? */
    if(instream!=NULL){
        /* read from stdin until it's end */
        while((bytes_read=fread(&buffer, buffer_size, 1, instream))==buffer_size){
            fprintf(stdout, "%c", buffer[0]);
        }
    }
    /* if any error occured, exit with an error message */
    else{
        fprintf(stderr, "ERROR opening stdin. aborting.\n");
        exit(1);
    }

    return(0);
}