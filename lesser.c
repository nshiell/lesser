#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE    1

struct View {
    char *window;
};

struct View view_create(int width, int height) {
    char *win;
    win = (char*)malloc(10);

    char spaces[200];
    spaces[0] = '\0';

    int i;
    for (i = 0; i < width - 2; i++) {
        strcat(spaces, " ");
    }

    for (i = 0; i < width; i++) {
        strcat(win, "_");
    }

    win = strcat(win, "\n");

    for (i = 0; i < height - 2; i++) {
        strcat(win, "|");
        strcat(win, spaces);
        strcat(win, "|\n");
    }
    
    strcat(win, "|");
    for (i = 0; i < width - 2; i++) {
        strcat(win, "_");
    }
    strcat(win, "|");
    
    struct View view;
    //view.window = (char*)malloc(10);
    view.window = win;
    return view;
}

void view_scrollbars_create(char **win, char **scrollbar, char *text, int line_count, int offset) {
    printf("%d\n", line_count);
    printf("%d\n", line_count);
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

void input_stdin_get_raw_with_line_count(char *text, int *line_count)
{
    unsigned char buffer[BUFFERSIZE];
    FILE *instream;
    int bytes_read = 0;
    int buffer_size = 0;

    buffer_size = sizeof(unsigned char) * BUFFERSIZE;
    /* open stdin for reading */
    instream = fopen("/dev/stdin","r");

    /* did it open? */
    if(instream != NULL) {
        /* read from stdin until it's end */
        while ((bytes_read = fread(&buffer, buffer_size, 1, instream)) == buffer_size) {
            text = strcat(text, &buffer[0]);
            if (buffer[0] == *"\n") {
                *line_count = *line_count + 1;
            }
        }
    }
    /* if any error occured, exit with an error message */
    else {
        fprintf(stderr, "ERROR opening stdin. aborting.\n");
        exit(1);
    }
}
/*
void console_cursor_movo_to(int x, int y) {
    printf("\033[%d;%df", x, y);
}*/

int main(int argc, char **argv) {
    char *win;
    win = (char*)malloc(10);

    //struct View view = view_create(60, 30);
    struct View view = view_create(60, 30);

    //printf("%s", win);
    printf("%s", view.window);

    char *text = (char*)malloc(130000);
    text[0] = '\0';
    
    int line_count = 0;
    
    input_stdin_get_raw_with_line_count(text, &line_count);

    char *scrollbar;
    scrollbar = (char*)malloc(10);
    scrollbar[0] = '\0';
    //view_scrollbars_create(&win, &scrollbar, text, line_count, 0);

    printf("%s", "\n");
    printf("%s", "HERE BELLOW\n");
    printf("%s", "\n");
    printf("%s", "\n");
    printf("%s", scrollbar);

    return(0);
}