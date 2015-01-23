#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE    1
#define VIEW_BLANK_CHAR "X"
#define LINE_BREAK "\n"

typedef int bool;
#define true 1
#define false 0

struct Geometry {
    int x;
    int y;
    int width;
    int height;
    int innerWidth;
    int innerHeight;
};

/**
 * Output can be used to render to the screen
 * Also stores partial parts of the window
 */
struct View {
    // The main window box
    //char *window;
    struct Window {
        char *rendered;
        char *template;
        struct Geometry geometry;
    } window;
    // The ascii image
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
    for (i = 0; i < width - 2 - 2; i++) {
        strcat(spaces, VIEW_BLANK_CHAR);
    }

    strcat(spaces, "YY");
    
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
    view.window.rendered = (char*)malloc(3000);
    
    view.window.template = (char*)malloc(3000);
    view.window.template = win;
    view.window.geometry.innerWidth = width - 2;
    view.window.geometry.innerHeight = height - 2;
    view.window.geometry.width = width;
    view.window.geometry.height = height;
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

    printf("%s\n\n", view->window.template);
    printf("%s\n\n", view->window.rendered);
    printf("%d\n", view->window.geometry.innerWidth);
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

void view_add_text(struct View *view, char *text) {
    // get lines
    // *text

    // get template
    // view.window.template

    // Mark which line of text we are on
    int current_template_line = 0;

    //char text_lines[view->window.geometry.innerHeight][view->window.geometry.innerWidth];
    //char text_lines[view->window.geometry.height + 1][view->window.geometry.innerWidth + 1];
    //char (char*)malloc((view->window.geometry.height + 1) * (view->window.geometry.innerWidth + 1));
    
    char *text_lines = (char*)malloc(130000);
    char *text_char;
    text_char = (char*)malloc(1);

    char *template_char;
    template_char = (char*)malloc(1);

    int text_char_i;
    text_char_i = 0;

    // Loop over view template line by line
    int row_no;
    int col_no;
    row_no = 0;
    col_no = 0;

    bool new_line_found = false;
    
    //printf("%s", view->window.template);
    int i;
    for (i = 0; i < strlen(view->window.template); i++) {
        //printf("%c\n", view->window.template[i]);
        *template_char = view->window.template[i];
        if (*template_char == *LINE_BREAK) {
            strcat(text_lines, "\n");
            //text_lines[row_no][col_no] = '\0';
            
            //printf("%s\n", text_lines[row_no]);

            row_no = row_no + 1;
            //text_lines[row_no][0] = *"";
            if (row_no == 1 && 0) { // 29
                break;
            }
            //text_lines[row_no][0] = '\0';

            col_no = 0;
            new_line_found = false;
        } else if (*template_char == *VIEW_BLANK_CHAR) {
            text_char_i = text_char_i + 1;
            *text_char = text[text_char_i];
            
            if (*text_char == *LINE_BREAK) {
                new_line_found = true;
                *text_char = *"\n";
                
            } else {
                if (text_char_i > strlen(text)) {
                    *text_char = *" ";
                } else {
                    *text_char = text[text_char_i];
                }
            }

            //text_lines[row_no][col_no] = *text_char;
            strcat(text_lines, text_char);
            col_no = col_no + 1;
        } else {
            //printf("\n\n%d %d\n", row_no, col_no);
            //text_lines[row_no][col_no] = *template_char;
            strcat(text_lines, template_char);
            col_no = col_no + 1;
        }
        
        //text_char
    }

    //printf("%s\n", text_lines);
    
    printf("%s\n", text_lines);
    
    //char strs[NUMBER_OF_STRINGS][STRING_LENGTH+1];
    

    // count number of X in the first line
    //    if count X == 0, no text here
    //        Put each char into the return
    //    else if count X < line length, then line too long == true

    // Loop over chars in template if X
    // replace with char, else leave
    /*
    int i;
    char x[] = "123456";
    
    view.window.template
    
    for (i = 0; i < strlen(x); i++) {
        printf("%c\n", x[i]);
    }*/
}

/*
void console_cursor_movo_to(int x, int y) {
    printf("\033[%d;%df", x, y);
}*/

int main(int argc, char **argv) {
    struct View view = view_create(60, 30);

    int line_count = 0;

    char *text = input_stdin_get_raw_with_line_count(&line_count);

    //char *text_visibile = "sdfhgsdfgsdfgsghdfgh";
    char *text_visibile = text;
    
    view_add_text(&view, text_visibile);
    view_add_scrollbar(&view, line_count, 0);

    /*
    printf("%s", "\n");
    printf("%s", "HERE BELLOW\n");
    printf("%s", "\n");
    printf("%s", "\n");

    printf("%s", view.text);
    printf("%s", view.scrollbar);*/

    return 0;
}