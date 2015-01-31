#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <termios.h>

#define BUFFERSIZE    1
#define VIEW_BLANK_CHAR "X"
#define LINE_BREAK "\n"

typedef int bool;
#define true 1
#define false 0

#define KEY_QUIT      0
#define KEY_DOWN      1
#define KEY_UP        2
#define KEY_PAGE_UP   3
#define KEY_PAGE_DOWN 4

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

struct ModelText {
    struct Lines {
        int count;
        int offset;
        int start;
        int end;
    } lines;
    char *raw;
    char *visibile;
};

struct ModelText model_text_create(char *raw) {
    
    struct ModelText text;
    
    int i;
    text.lines.count = 0;
    for (i = 0; i < strlen(raw); i = i + 1) {
        if (raw[i] == *LINE_BREAK) {
            text.lines.count = text.lines.count + 1;
        }
    }

    text.lines.offset = 0;
    text.lines.start = 0;
    text.lines.end = 0;
    text.raw = raw;

    return text;
}

void model_text_set_visibile(struct ModelText *text) {
    int i;
    int line_breaks_seen = 0;
    text->visibile = (char*)malloc(130000);
    text->visibile[0] = '\0';

    char *r = (char*)malloc(130000);
    r[0] = '\0';
    
    for (i = 0; i < strlen(text->raw); i = i + 1) {
        // is the number of newlines == offset?
        if (line_breaks_seen >= text->lines.offset) {
            text->visibile[strlen(text->visibile)] = text->raw[i];
            text->visibile[strlen(text->visibile) + 1] = *"\0";
        }

        if (text->raw[i] == *LINE_BREAK) {
            line_breaks_seen = line_breaks_seen + 1;
        }
    }
    
    
    
    //text.lines.offset;
    //text.lines.start
}

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
    return;
    //printf("%s\n\n", view->window.template);
    printf("%s", view->window.rendered);
    /*printf("%d\n", view->window.geometry.innerWidth);
    printf("%d\n", line_count);
    printf("%d\n", line_count);*/
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
//char * input_stdin_get_raw_with_line_count(int *line_count) {
char * input_stdin_get_raw_with_line_count(void) {
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
            /*if (buffer[0] == *"\n") {
                *line_count = *line_count + 1;
            }*/
        }
    }
    /* if any error occured, exit with an error message */
    else {
        fprintf(stderr, "ERROR opening stdin. aborting.\n");
        exit(1);
    }
//fclose(instream);
    return text;
}



void view_add_text(struct View *view, char *text) {
    // get lines
    // *text

    // get template
    // view.window.template

    // Mark which line of text we are on
    int current_template_line = 0;

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

    bool end_of_line_found = false;
    bool start_of_line_added = false;

    int text_length = strlen(text);

    int i;
    for (i = 0; i < strlen(view->window.template); i = i + 1) {
        *template_char = view->window.template[i];

        if (*template_char == *VIEW_BLANK_CHAR) {
            if (end_of_line_found || text_char_i >= text_length) {
                *text_char = *" ";
                strcat(text_lines, text_char);
            } else {
                *text_char = text[text_char_i];
                if (*text_char == *LINE_BREAK) {
                    end_of_line_found = true;
                    *text_char = *" ";
                } else if ((int) *text_char < 32) {
                    text_char_i = text_char_i + 1;
                    i = i -1;
                    continue;
                } else {
                    start_of_line_added = true;
                }

                strcat(text_lines, text_char);
                text_char_i = text_char_i + 1;
            }
        } else if (*template_char == *LINE_BREAK) {
            if (!end_of_line_found && start_of_line_added) {
                text_lines[strlen(text_lines) - 4] = *".";
                text_lines[strlen(text_lines) - 5] = *".";
                text_lines[strlen(text_lines) - 6] = *".";

                while (true) {
                    if (text[text_char_i] == *LINE_BREAK) {
                        text_char_i = text_char_i + 1;
                        break;
                    }

                    if (text_char_i >= strlen(text)) {
                        break;
                    }

                    text_char_i = text_char_i + 1;
                }
                
            }
            end_of_line_found = false;
            start_of_line_added = false;
            strcat(text_lines, "\n");
        } else {
            strcat(text_lines, template_char);
        }
    }
    view->window.rendered = text_lines;

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

int input_console_get_key(void) {
    freopen("/dev/tty", "rw", stdin);
    int ch;
    struct termios origterm, tmpterm;
    
    tcgetattr(STDIN_FILENO, &origterm);
    tmpterm = origterm;
    tmpterm.c_lflag &= ~(ICANON | ECHO);
    tcsetattr (STDIN_FILENO, TCSANOW, &tmpterm);
    ch = getchar();
    
    int key = 0;
    
    if (ch == 27) { // 27 means cursor key
        getchar();
        ch = getchar();
        if (ch == 66) {
            key = KEY_DOWN;
        } else if (ch == 65) {
            key = KEY_UP;
        }
    } else if (ch == 81 || ch == 113 || ch == 27) {
        key = KEY_QUIT;
    }/* else { // Think of what to do here!
        key = KEY_QUIT;
    }*/
    /*
    
    printf("=%d=", ch); // 27 means cursor key
    // 81 113 mean "q" 27 mean escape
    ch = getchar();
    printf("=%d=", ch);
    ch = getchar();
    printf("=%d= ", ch); // 66 is down // 65 means up*/
    tcsetattr (STDIN_FILENO, TCSANOW, &origterm );
    
    
    freopen("/dev/stdin","r", stdin);
    return key;
  }

int main(int argc, char **argv) {
    struct View view = view_create(60, 30);

    int key;
    int line_count = 0;
    int line_offset = 1;
    int line_start = 0;
    int line_end = 0;

    struct ModelText text = model_text_create(input_stdin_get_raw_with_line_count());
    text.lines.start = 0;

    while (true) {
        model_text_set_visibile(&text);

        view_add_text(&view, text.visibile);
        view_add_scrollbar(&view, line_count, 0);

        printf("%s", view.window.rendered);

        key = input_console_get_key();
        if (key == KEY_QUIT) {
            break;
        }
        if (key == KEY_UP && text.lines.offset) {
            text.lines.offset = text.lines.offset - 1;
        } else if (key == KEY_DOWN) {
            text.lines.offset = text.lines.offset + 1;
        }
        //printf("_%d_", key);
    }
    
    //free(text);

    /*
    printf("%s", "\n");
    printf("%s", "HERE BELLOW\n");
    printf("%s", "\n");
    printf("%s", "\n");

    printf("%s", view.text);
    printf("%s", view.scrollbar);*/

    return 0;
}