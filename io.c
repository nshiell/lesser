char* lesser_io_readnew()
{
    char buffer[10];
    char *input = 0;
    size_t cur_len = 0;
    while (fgets(buffer, sizeof(buffer), stdin) != 0)
    {
        size_t buf_len = strlen(buffer);
        char *extra = realloc(input, buf_len + cur_len + 1);
        if (extra == 0)
            break;
        input = extra;
        strcpy(input + cur_len, buffer);
        cur_len += buf_len;
    }
    printf("%s [%d]", input, (int)strlen(input));
    free(input);
    return 0;
}

/**
 * Will return a pointer to a variable representing the whole string
 * Don't forget to free afterwards
 * @param FILE *filePointer like fopen("/somefile.txt", "r") or stdin
 * @return *text
 */
char* lesser_io_read(FILE *file_pointer)
{
    char buffer[10];
    static char *text = 0;
    size_t cur_len = 0;
    
    //FILE *fp = fopen("/home/pi/Documents/lesser-git/io.c", "r");
    
    while (fgets(buffer, sizeof(buffer), file_pointer) != 0)
    {
        size_t buf_len = strlen(buffer);
        char *extra = realloc(text, buf_len + cur_len + 1);
        if (extra == 0)
            break;
        text = extra;
        strcpy(text + cur_len, buffer);
        cur_len += buf_len;
    }
    
    return text;
}






char* lesser_io_readold()
{
    static char *source = NULL;
    //FILE *fp = fopen("/home/pi/Documents/lesser-git/io.c", "r");
    FILE *fp = fopen("/dev/stdin", "r");
    if (fp != NULL) {
        printf("HERE0");
        /* Go to the end of the file. */
        if (fseek(fp, 0L, SEEK_END) == 0) {
            printf("HERE!");
            /* Get the size of the file. */
            long bufsize = ftell(fp);
            if (bufsize == -1) { /* Error */ }

            /* Allocate our buffer to that size. */
            source = malloc(sizeof(char) * (bufsize + 1));

            /* Go back to the start of the file. */
            if (fseek(fp, 0L, SEEK_SET) != 0) { /* Error */ }

            /* Read the entire file into memory. */
            size_t newLen = fread(source, sizeof(char), bufsize, fp);
            if (newLen == 0) {
                fputs("Error reading file", stderr);
            } else {
                source[newLen++] = '\0'; /* Just to be safe. */
            }
        }
        fclose(fp);
    }
    //static char string[] = "blah!";
    
    return source;
}
