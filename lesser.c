#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <termios.h>
#include <getopt.h>

#define BUFFERSIZE       1
#define VIEW_BLANK_CHAR  "X"
#define VIEW_SCROLL_CHAR "Y"
#define LINE_BREAK       "\n"

#define GEOMETERY_WINDOW_WIDTH_MIN  10
#define GEOMETERY_WINDOW_WIDTH_MAX  150
#define GEOMETERY_WINDOW_WIDTH_HINTS  99

#define GEOMETERY_WINDOW_HEIGHT_MIN 4
#define GEOMETERY_WINDOW_HEIGHT_MAX 60
#define GEOMETERY_WINDOW_HEIGHT_HINTS 24

#define STREAM_PATH_DEFAULT  "/dev/stdin"

#define PROGRAM_LESSER_DESCRIPTION "Usage: [PIPE FROM STDIN |] lesser [FILE] [OPTION] \n\
\n\
A program that allows you to view files and other streams (from stdin) in a scrollable manner.\n\
You can use this program in a similar way to less although this program is *far* less advanced,\n\
and for now is just an experimant into writing programs in c. \n\
\n\
Options:\n\
  -w, --width <number>     Width of the window\n\
  -h, --height <number>    Height of the window\n\
  -e, --help               Show some help\n"

#define PROGRAM_LESSER_HINTS "When running the program you can see a scrollbar on the right\n\
to show how far down the page you are.\n\
press space, or down arrow or enter to see the next line\n\
press up to go up one line\n\
press enter or page down to jump down one screen (window's worth of text)\n\
press page up ti go up one screen\n\
press q or any other quy to quit.\n\
Written by Nicholas Shiell (NShiell), please feel free to hack the sourcecode yourself,\n\
but if you do please let me have your work.\n\
\n\
This is licenced under the terms of the GNU GPLv3." 

typedef int bool;
#define true  1
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
    struct Window {
        char *rendered;
        char *template;
        struct Geometry geometry;
    } window;
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
    for (i = 0; i < width - 1 - 2; i++) {
        strcat(spaces, VIEW_BLANK_CHAR);
    }

    strcat(spaces, VIEW_SCROLL_CHAR);
    
    strcat(win, " ");
    for (i = 0; i < width - 2; i++) {
        strcat(win, "_");
    }

    win = strcat(win, "\n");

    for (i = 0; i < height - 2; i++) {
        strcat(win, "|");
        strcat(win, spaces);
        strcat(win, "|\n");
    }
    
    strcat(win, "'");
    for (i = 0; i < width - 2; i++) {
        strcat(win, "-");
    }
    strcat(win, "'");
    
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
 */
void view_add_scrollbar(struct View *view, struct ModelText *text) {
    /** @todo fix this maths */
    int thumb_size = ((view->window.geometry.innerHeight) / (float) text->lines.count) * (view->window.geometry.innerHeight - 2);
    if (thumb_size > (view->window.geometry.innerHeight - 2)) {
        thumb_size = (view->window.geometry.innerHeight - 2);
    } else if (!thumb_size) {
        thumb_size = 1;
    }

    int thumb_offset = (text->lines.offset / (float) text->lines.count) * (view->window.geometry.innerHeight);
    if (thumb_offset > view->window.geometry.innerHeight - thumb_size) {
        thumb_offset = view->window.geometry.innerHeight - thumb_size;
    }
    
    char *template_char;
    //template_char = (char*)malloc(1);
    template_char[1];
    int i;
    int last_scroll_Char = -1;
    int thumb_added = 0;
    
    for (i = 0; i < strlen(view->window.template); i = i + 1) {
        *template_char = view->window.template[i];
        if (*template_char == *VIEW_SCROLL_CHAR) {
            if (last_scroll_Char < 0) {
                view->window.rendered[i] = *"A";
            } else if (thumb_offset) {
                view->window.rendered[i] = *" ";
                thumb_offset--;
            } else if (thumb_added > thumb_size) {
                view->window.rendered[i] = *" ";
            } else {
                view->window.rendered[i] = *"*";
                thumb_added++;
            }
            
            last_scroll_Char = i;
        }
    }
    view->window.rendered[last_scroll_Char] = *"V";
}

char * input_stdin_get_raw_with_line_count(FILE *instream) {
    char *text = (char*)malloc(130000);
    text[0] = '\0';

    unsigned char buffer[BUFFERSIZE];
    //FILE *instream;
    int bytes_read = 0;
    int buffer_size = 0;

    buffer_size = sizeof(unsigned char) * BUFFERSIZE;
    /* open stdin for reading */
    //instream = fopen("/dev/stdin","r");

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
                text_lines[strlen(text_lines) - 3] = *".";
                text_lines[strlen(text_lines) - 4] = *".";
                text_lines[strlen(text_lines) - 5] = *".";

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


void console_cursor_move_by(int x, int y) {
    //printf("\033[%d;%df", x, y);
    printf("\033[%dA", x);
    printf("\033[%dD", y);
}

int input_console_get_key(void) {
    freopen("/dev/tty", "rw", stdin);
    int ch;
    struct termios origterm, tmpterm;
    
    tcgetattr(STDIN_FILENO, &origterm);
    tmpterm = origterm;
    tmpterm.c_lflag &= ~(ICANON | ECHO);
    tcsetattr (STDIN_FILENO, TCSANOW, &tmpterm);
    ch = getchar();
    //printf("%d ", ch);
    int key = 0;
    
    if (ch == 27) { // 27 means cursor key
        getchar();
        ch = getchar();
        //printf("%d ", ch);
        if (ch == 66) {
            key = KEY_DOWN;
        } else if (ch == 65) {
            key = KEY_UP;
        } else if (ch == 53) {
            key = KEY_PAGE_UP;
        } else if (ch == 54) {
            key = KEY_PAGE_DOWN;
        }
//    } else if (ch == 81 || ch == 113 || ch == 27) {
//        key = KEY_QUIT;
    } else if (ch == 32) { // space
        key = KEY_PAGE_DOWN;
    } else if (ch == 10) { // enter
        key = KEY_DOWN;
    }/* else { // Think of what to do here!
        key = KEY_QUIT;
    }*/

    tcsetattr (STDIN_FILENO, TCSANOW, &origterm );
    
    
    freopen("/dev/stdin","r", stdin);
    return key;
}

char * program_hints_get() {
    char *hints = (char*)malloc(130000);
    hints[0] = '\0';
    strcat(hints, PROGRAM_LESSER_DESCRIPTION);
    strcat(hints, "\n");
    strcat(hints, PROGRAM_LESSER_HINTS);

    return hints;
}

void console_usage_print() {
    printf(PROGRAM_LESSER_DESCRIPTION);
}

void console_parse_geometery_stream_path(int argc, char **argv, int *width, int *height, bool *is_help, char **stream_path) {
    int opt = 0;

    //Specifying the expected options
    static struct option long_options[] = {
        // Ags are actually optional
        {"help",     no_argument,       0,  'e' },
        {"width",    required_argument, 0,  'w' },
        {"height",   required_argument, 0,  'h' },
        {0,           0,                 0,  0   }
    };

    int long_index = 0;
    while ((opt = getopt_long(argc, argv,"w:h:", 
                  long_options, &long_index )) != -1) {
        switch (opt) {
            case 'e': *is_help = true;
                break;
            case 'h' : *height = atoi(optarg);
                break;
            case 'w' : *width = atoi(optarg);
                break;
            default: console_usage_print();
                exit(EXIT_FAILURE);
        }
    }

    if (*width < GEOMETERY_WINDOW_WIDTH_MIN) {
        *width = GEOMETERY_WINDOW_WIDTH_MIN;
    } else if (*width > GEOMETERY_WINDOW_WIDTH_MAX) {
        *width = GEOMETERY_WINDOW_WIDTH_MAX;
    }

    if (*height < GEOMETERY_WINDOW_HEIGHT_MIN) {
        *height = GEOMETERY_WINDOW_HEIGHT_MIN;
    } else if (*height > GEOMETERY_WINDOW_HEIGHT_MAX) {
        *height = GEOMETERY_WINDOW_HEIGHT_MAX;
    }
    
    *stream_path = STREAM_PATH_DEFAULT;
    
    if (optind < argc) {
        while (optind < argc) {
            *stream_path = argv[optind++];
        }
    }
}

int main(int argc, char **argv) {
    int width = 60;
    int height = 20;
    bool is_help = false;

    char *stream_path;
    stream_path = (char*)malloc(100);
    console_parse_geometery_stream_path(argc, argv, &width, &height, &is_help, &stream_path);

    char *raw = (char*)malloc(130000);
    
    if (is_help) {
        raw = program_hints_get();
        width = GEOMETERY_WINDOW_WIDTH_HINTS;
        height = GEOMETERY_WINDOW_HEIGHT_HINTS;
    } else {
        FILE *instream = fopen(stream_path,"r");
        raw = input_stdin_get_raw_with_line_count(instream);
    }

    
    struct View view = view_create(width, height);

    int key;
    int line_count = 0;

    struct ModelText text = model_text_create(raw);
    text.lines.start = 0;

    while (true) {
        model_text_set_visibile(&text);

        view_add_text(&view, text.visibile);
        view_add_scrollbar(&view, &text);

        printf("%s", view.window.rendered);

        key = input_console_get_key();
        if (key == KEY_QUIT) {
            break;
        }

        if (key == KEY_PAGE_UP) {
            text.lines.offset = text.lines.offset - view.window.geometry.innerHeight;
            if (text.lines.offset < 0) {
                text.lines.offset = 0;
            }
        } else if (key == KEY_PAGE_DOWN) {
            text.lines.offset = text.lines.offset + view.window.geometry.innerHeight;
            if (text.lines.offset > text.lines.count - view.window.geometry.innerHeight) {
                text.lines.offset = text.lines.count - view.window.geometry.innerHeight;
            }
        } else if (key == KEY_UP && text.lines.offset) {
            text.lines.offset = text.lines.offset - 1;
        } else if (key == KEY_DOWN && text.lines.offset < text.lines.count - view.window.geometry.innerHeight) {
            text.lines.offset = text.lines.offset + 1;
        }

        console_cursor_move_by(view.window.geometry.height - 1, view.window.geometry.width);
    }
    return 0;
}