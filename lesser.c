#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE    1

void get_screen_contents() {
    //system ("tput smcup; clear; sleep 5; tput rmcup");
    //system ("tput smcup; clear");
    system ("tput smcup; tput rmcup > ~/.lesser-console");
}

void draw_screen_contents() {
    //system ("tput smcup; clear; sleep 5; tput rmcup");
    system ("tput rmcup");
    //system ("tput smcup; tput rmcup > ~/.lesser-console");
}

void view_create(char **win, int width, int height) {
    char spaces[200];
    //spaces = (char*)malloc(10);
    spaces[0] = '\0';

    int i;
    for (i = 0; i < width - 2; i++) {
        strcat(spaces, " ");
    }

    for (i = 0; i < width; i++) {
        strcat(*win, "_");
    }
    *win = strcat(*win, "\n");

    for (i = 0; i < height - 2; i++) {
        strcat(*win, "|");
        strcat(*win, spaces);
        strcat(*win, "|\n");
    }
    
    strcat(*win, "|");
    for (i = 0; i < width - 2; i++) {
        strcat(*win, "_");
    }
    strcat(*win, "|");
}

void view_scrollbars_create(char **win, char **scrollbar, char *text, int offset) {
    /*char scrollbar[300];
    scrollbar[0] = '\0';*/  

    strcat(*scrollbar, "/\\\n");
    strcat(*scrollbar, "##\n");
    strcat(*scrollbar, "##\n");
    strcat(*scrollbar, "##\n");
    strcat(*scrollbar, "##\n");
    strcat(*scrollbar, "\n");
    strcat(*scrollbar, "\n");
    strcat(*scrollbar, "\n");
    strcat(*scrollbar, "\n");
    strcat(*scrollbar, "\n");
    strcat(*scrollbar, "\n");
    strcat(*scrollbar, "\n");
    strcat(*scrollbar, "\\/\n");
}

const char * get_data()
{
    unsigned char     buffer[BUFFERSIZE];
    FILE                         *instream;
    int                            bytes_read=0;
    int                            buffer_size=0;

    char *x;
    x = (char*)malloc(130000);
    x[0] = '\0';

    buffer_size=sizeof(unsigned char)*BUFFERSIZE;
    /* open stdin for reading */
    instream=fopen("/dev/stdin","r");

    /* did it open? */
    if(instream!=NULL){
        /* read from stdin until it's end */
        while((bytes_read=fread(&buffer, buffer_size, 1, instream))==buffer_size){
            //fprintf(stdout, "%c", buffer[0]);
            x = strcat(x, &buffer[0]);
        }
    }
    /* if any error occured, exit with an error message */
    else{
        fprintf(stderr, "ERROR opening stdin. aborting.\n");
        exit(1);
    }
    
    
    const char *r = x;

    return r;
}

void console_cursor_movo_to(int x, int y) {
    //printf("%s", "\033[5;2f");
    printf("\033[%d;%df", x, y);
}

int main(int argc, char **argv) {
    unsigned char     buffer[BUFFERSIZE];
    FILE                         *instream;
    int                            bytes_read=0;
    int                            buffer_size=0;

    //get_screen_contents();
    
    char *win;
    win = (char*)malloc(10);

    view_create(&win, 60, 30);
// printf("\033[6;3HHello\n");
    //printf("%s", "\033[5;2f");
    //draw_screen_contents();
    //console_cursor_movo_to(5, 20);
    printf("%s", win);
    printf("%s", get_data());
    

    char *scrollbar;
    scrollbar = (char*)malloc(10);
    scrollbar[0] = '\0';
    view_scrollbars_create(&win, &scrollbar, "sdfsdf", 0);
    
    printf("%s", "\n");
    printf("%s", "HERE BELLOW\n");
    printf("%s", "\n");
    printf("%s", "\n");
    printf("%s", scrollbar);
    
    
    //sleep(5);
    //draw_screen_contents();
    return(0);
    
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