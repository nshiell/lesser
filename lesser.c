#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE    1

/**
 * Output can be used to render to the screen
 * Also stores partial parts of the window
 */
struct View {
    char *window;
    char *scrollbar;
};

/**
 * Will create a view
 * Basically the window geometery empty (with spaces)
 */
struct View view_create(int width, int height) {
    char *win;
    win = (char*)malloc(10000);

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
    //view.window = (char*)malloc(3000);
    view.window = win;
    return view;
}

/**
 * Will append a scrollbar to the right of the window
 * @param view       Will append the scrollbar
 * @param line_count How many lines are there?
 * @param offset     Which line are we scrolled too?
 */
void view_add_scrollbar(struct View *view, int line_count, int offset) {
    char *scrollbar;
    scrollbar = (char*)malloc(10);
    scrollbar[0] = '\0';

    printf("%s", view->window);
    printf("%d\n", line_count);
    printf("%d\n", line_count);
    strcat(scrollbar, "/\\\n");
    strcat(scrollbar, "##\n");
    strcat(scrollbar, "##\n");
    strcat(scrollbar, "##\n");
    strcat(scrollbar, "##\n");
    strcat(scrollbar, "\n");
    strcat(scrollbar, "\n");
    strcat(scrollbar, "\n");
    strcat(scrollbar, "\n");
    strcat(scrollbar, "\n");
    strcat(scrollbar, "\n");
    strcat(scrollbar, "\n");
    strcat(scrollbar, "\\/\n");

    //view->scrollbar = (char*)malloc(10);
    //view->scrollbar[0] = '\0';
    view->scrollbar = scrollbar;
}

/**
 * @refactor?
 */
char * input_stdin_get_raw_with_line_count(int *line_count)
{
    char *text = (char*)malloc(130000);
    text[0] = '\0';

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

    return text;
}
/*
void console_cursor_movo_to(int x, int y) {
    printf("\033[%d;%df", x, y);
}*/

int main(int argc, char **argv) {
    struct View view = view_create(60, 30);

    int line_count = 0;
    
    char *text = input_stdin_get_raw_with_line_count(&line_count);
    
    view_add_scrollbar(&view, line_count, 0);

    printf("%s", "\n");
    printf("%s", "HERE BELLOW\n");
    printf("%s", "\n");
    printf("%s", "\n");
    printf("%s", text);
    printf("%s", view.scrollbar);

    return(0);
}