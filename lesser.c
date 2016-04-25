#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <termios.h>
#include <getopt.h>
#include <sys/ioctl.h>

#include "defines.c"
#include "io.c"

// To compile: gcc -x c lesser.c -o lesser

#define STREAM_PATH_DEFAULT  "/dev/stdin"

typedef int bool;
#define true  1
#define false 0

#define KEY_QUIT      0
#define KEY_DOWN      1
#define KEY_UP        2
#define KEY_PAGE_UP   3
#define KEY_PAGE_DOWN 4

/**
 * Used for representing the dimentions of something
 */
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

/**
 * For modeling text, raw text, number of lines etc
 */
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

/**
 * For creating a ModelText
 */
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

/**
 * Adds a substring of visibile text into ModelText->visibile
 * @todo check that is doesn't add in too much text
 */
void model_text_set_visibile(struct ModelText *text, int innerHright) {
    int i;
    int line_breaks_seen = 0;
    text->visibile = (char*)malloc(130000);
    text->visibile[0] = '\0';

    //char *r = (char*)malloc(130000);
    //r[0] = '\0';

    for (i = 0; i < strlen(text->raw); i = i + 1) {
        // is the number of newlines == offset?
        if (line_breaks_seen >= text->lines.offset) {
            text->visibile[strlen(text->visibile)] = text->raw[i];
            text->visibile[strlen(text->visibile) + 1] = *"\0";
        }

        if (text->raw[i] == *LINE_BREAK) {
            line_breaks_seen = line_breaks_seen + 1;
            // We have enough lines of text to show? Great break out then
            if (line_breaks_seen - text->lines.offset == innerHright) {
                break;
            }
        }
    }
    //text.lines.offset;
    //text.lines.start
}

/**
 * Will create a view
 * Will create a View, with a blank tenplate, the area where text
 * and scrollbars will appear have placeholder chars
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

    win = strcat(win, LINE_BREAK);

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

/**
 * Will read chars from the FILE passed in untill it becomes NULL
 * Will error to stderror if can't open file
 */
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
            // Not sure how accurate this is!
            if (strlen(text) == 130000) {
                break;
            }
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

/**
 * Will add some text to the window, replacing the placeholder chars
 * Will not do scrollbars
 * If too much text given, the extra won't show
 */
void view_add_text(struct View *view, char *text) {
    // get lines
    // *text

    // get template
    // view.window.template

    // Mark which line of text we are on
    int current_template_line = 0;

    //char (char*)malloc((view->window.geometry.height + 1) * (view->window.geometry.innerWidth + 1));
    char *text_lines = (char*)malloc(130000);

    // Hold the current char from the text
    char *text_char;
    text_char = (char*)malloc(1);

    // Hold the current char from the template
    char *template_char;
    template_char = (char*)malloc(1);

    // Offset of the text char we have
    int text_char_i;
    text_char_i = 0;

    // This is important to calculate weather we should truncate the line with "..."
    bool end_of_line_found = false;
    // Have we actually added any text for this line?
    // (this might be false if the innerHeight > line count)
    bool start_of_line_added = false;

    // Usefull for adding in spaces after we have added all text in
    // (this might be false if the innerHeight > line count)
    int text_length = strlen(text);

    // Walk over each char in the template replacing with text or "." or a space
    int i;
    for (i = 0; i < strlen(view->window.template); i++) {
        *template_char = view->window.template[i];

        // This char *should* be replaced
        if (*template_char == *VIEW_BLANK_CHAR) {
            // We have no more text to add in for this line, add in spaces
            if (end_of_line_found || text_char_i >= text_length) {
                *text_char = *" ";
                strcat(text_lines, text_char);
            } else {
                *text_char = text[text_char_i];
                // The char we have in text is a newline,
                // but the template wants a normal printed char use a space
                // and mark that we encountered an EOL in text
                if (*text_char == *LINE_BREAK) {
                    end_of_line_found = true;
                    *text_char = *" ";
                // The char is a control char, ewww, forget it and move the template
                // char back one so we can put a proper char in ths template char next itteration!
                } else if ((int) *text_char < 32) {
                    text_char_i = text_char_i + 1;
                    i = i -1;
                    continue;
                } else {
                    // We are adding an actual char, make sure we
                    // know we have started adding this line, we need this for truncation
                    start_of_line_added = true;
                }

                // Add the text char into the concatinated output,
                // both text chars and tempalate can be in text_char
                // ...and make sure we try and read the next char next time
                strcat(text_lines, text_char);
                text_char_i++;
            }
        // Template EOL
        } else if (*template_char == *LINE_BREAK) {
            // We have an incomplete line, then add in the "." 3, 4 and 5th char BEHIND here
            /** @todo think about this! */
            if (!end_of_line_found && start_of_line_added) {
                text_lines[strlen(text_lines) - 3] = *".";
                text_lines[strlen(text_lines) - 4] = *".";
                text_lines[strlen(text_lines) - 5] = *".";

                // Push text_char_i upto the EOL so that the next line starts at the correct place
                // so truncation occurs
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
            // Reset stuff for next line to work correctly
            end_of_line_found = false;
            start_of_line_added = false;
            // And add in the actual newline into the text
            strcat(text_lines, template_char);
        } else {
            strcat(text_lines, template_char);
        }
    }

    view->window.rendered = text_lines;
}

/**
 * Move the consoles cursor up and left
 * @param int x how many spaces left
 * @param int y how many spaces up
 */
void console_cursor_move_by(int x, int y) {
    //printf("\033[%d;%df", x, y);
    printf("\033[%dA", x);
    printf("\033[%dD", y);
}
struct termios origterm, tmpterm;
/**
 * Returns one of KEY_QUIT, KEY_DOWN, KEY_UP, KEY_PAGE_UP, KEY_PAGE_DOWN
 * This reads /dev/tty directly as we are using /dev/stdin for getting raw text
 */
int input_console_get_key(void) {
    freopen("/dev/tty", "rw", stdin);
    int ch;
    //struct termios origterm, tmpterm;

    ch = getchar();

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

    //tcsetattr (STDIN_FILENO, TCSANOW, &origterm );

    //freopen("/dev/stdin","r", stdin);
    return key;
}

void input_console_disable_char(void) {
    freopen("/dev/tty", "rw", stdin);
    tcgetattr(STDIN_FILENO, &origterm);
    tmpterm = origterm;
    tmpterm.c_lflag &= ~(ICANON | ECHO);
    tcsetattr (STDIN_FILENO, TCSANOW, &tmpterm);
}

void input_console_enable_char(void) {
    freopen("/dev/tty", "rw", stdin);
    tcsetattr (STDIN_FILENO, TCSANOW, &origterm );
}

/**
 * Concatinates PROGRAM_LESSER_DESCRIPTION a new line and PROGRAM_LESSER_HINTS
 * Useful hints on the program
 */
char * program_hints_get() {
    char *hints = (char*)malloc(130000);
    hints[0] = '\0';
    strcat(hints, PROGRAM_LESSER_DESCRIPTION);
    strcat(hints, LINE_BREAK);
    strcat(hints, PROGRAM_LESSER_HINTS);

    return hints;
}

/**
 * Prints out standard unix usage text
 */
void console_usage_print() {
    printf(PROGRAM_LESSER_DESCRIPTION);
}

void console_get_dimensions(int *width, int *height) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    // Default values
    //int width = GEOMETERY_WINDOW_WIDTH_DEFAULT;
    *width = w.ws_col;
    //int height = GEOMETERY_WINDOW_HEIGHT_DEFAULT;
    *height = w.ws_row / 2;
}

/**
 * Reads from argv
 * @param argc count of args
 * @param argv char array of args
 * @param stream_path will be the file path passed in else STREAM_PATH_DEFAULT
 * @param width will be the width(if passed)
 * @param height will be the height (if passed)
 */
void console_parse_geometery_stream_path(int argc, char **argv, int *width, int *height, bool *is_help, char **stream_path) {
    int opt = 0;

    console_get_dimensions(width, height);

    //Specifying the expected options
    static struct option long_options[] = {
        // Ags are actually all optional
        {"help",     no_argument,       0,  'e'},
        {"width",    required_argument, 0,  'w'},
        {"height",   required_argument, 0,  'h'},
        {0,          0,                 0,   0 }
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

    // Sane bounds areound width and height
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

    // Add in file path
    //*stream_path = STREAM_PATH_DEFAULT;

    if (optind < argc) {
        while (optind < argc) {
            *stream_path = argv[optind++];
        }
    }
}

int main(int argc, char **argv) {
    bool is_help = false;

    int width = 0;
    int height = 0;

    char *stream_path;
    // Populate vars with values from args
    console_parse_geometery_stream_path(argc, argv, &width, &height, &is_help, &stream_path);

    // Populate raw_text with standard in, file load, or hints
    //char *text_raw = (char*)malloc(130000);

    FILE *stream;
    if (stream_path) {
        *stream = fopen("/home/pi/Documents/lesser-git/io.c", "r");
    } else {
        *stream = stdin;
    }
    
    char *raw_text = lesser_io_read(stream);
    printf("AAA%sZZZ", raw_text);
    free(raw_text);
    return 0;
    
/*    
    
    
    
    
    // Do help!
    if (is_help) {
        // Get hints as input, andd set the dimentions
        text_raw = program_hints_get();
        width = GEOMETERY_WINDOW_WIDTH_HINTS;
        height = GEOMETERY_WINDOW_HEIGHT_HINTS;
    } else {
        // Take the stream path /dev/stdin or a file and get text from it
        FILE *instream = fopen(stream_path, "r");
        text_raw = input_stdin_get_raw_with_line_count(instream);
    }

    // Create a vew, for holding templates, rendered view etc
    struct View view = view_create(width, height);

    struct ModelText text = model_text_create(text_raw);
    text.lines.start = 0;

    // Main loop, draws the window, then awaits key press
    int key = KEY_QUIT;

    while (true) {
        input_console_disable_char();
        model_text_set_visibile(&text, view.window.geometry.innerHeight);

        view_add_text(&view, text.visibile);
        view_add_scrollbar(&view, &text);

        // Main print of the rendered template
        // Apart from errors this is the only print!
        printf("%s", view.window.rendered);

        key = input_console_get_key();
        input_console_enable_char();

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

        // Move the cursor back up so we overprint the window, giving the illustion of a scroll
        console_cursor_move_by(view.window.geometry.height - 1, view.window.geometry.width);
    }

    return 0;*/
}
