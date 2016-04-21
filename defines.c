#define BUFFERSIZE       1
#define VIEW_BLANK_CHAR  "X"
#define VIEW_SCROLL_CHAR "Y"
#define LINE_BREAK       "\n"

#define GEOMETERY_WINDOW_WIDTH_MIN      10
#define GEOMETERY_WINDOW_WIDTH_MAX      150
#define GEOMETERY_WINDOW_WIDTH_HINTS    99
#define GEOMETERY_WINDOW_WIDTH_DEFAULT  60

#define GEOMETERY_WINDOW_HEIGHT_MIN     4
#define GEOMETERY_WINDOW_HEIGHT_MAX     60
#define GEOMETERY_WINDOW_HEIGHT_HINTS   24
#define GEOMETERY_WINDOW_HEIGHT_DEFAULT 20

#define PROGRAM_LESSER_DESCRIPTION "Usage: [PIPE FROM STDIN |] lesser [FILE] [OPTION] \n\
\n\
A program that allows you to view files and other streams (from stdin) in a scrollable manner.\n\
You can use this program in a similar way to less although this program is *far* less advanced,\n\
and for now is just an experimant into writing programs in c. \n\
\n\
Options:\n\
  -w, --width  <number>    Width of the window\n\
  -h, --height <number>    Height of the window\n\
  -e, --help               Show some help\n"

#define PROGRAM_LESSER_HINTS "When running the program you can see a scrollbar on the right\n\
to show how far down the page you are.\n\
press space, or down arrow or enter to see the next line\n\
press up to go up one line\n\
press enter or page down to jump down one screen (window's worth of text)\n\
press page up to go up one screen\n\
press q or any other key to quit.\n\
Written by Nicholas Shiell (NShiell), please feel free to hack the sourcecode yourself,\n\
but if you do please let me have your work.\n\
\n\
This is licenced under the terms of the GNU GPLv3."
