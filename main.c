#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>



void display_fb(char* framebuffer) {
    char *text_to_display = "Hello World, I'm running in the text editor window";
    int len = strlen(text_to_display);
    for (int i = 0; i < len; i++) {
        framebuffer[i + 5] = text_to_display[i];
    }
}

int main() {
    // initialize the frame buffer
    struct ttysize ts;
    ioctl(0, TIOCGSIZE, &ts);
    int size = (sizeof(char) * (ts.ts_lines) * (ts.ts_cols)) + 1;
    char *framebuffer = malloc(size);

    char* clear_txt = malloc(size);
    memset(clear_txt, '\b', size - 1);
    clear_txt[size - 1] = '\0';

    memset(framebuffer, ' ', size);
    framebuffer[size - 1] = '\0';

    // Get current terminal settings
    struct termios old_settings, new_settings;
    tcgetattr(STDIN_FILENO, &old_settings);
    new_settings = old_settings;
    new_settings.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
    system("clear");
    printf("\x1b[?1000h\x1b[?1006h"); 


    display_fb(framebuffer);
    printf("%s", framebuffer);

    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        printf("%s", clear_txt);

        // Editor logic
        display_fb(framebuffer);

        printf("%s", framebuffer);
    }

    printf("\x1b[?1000l\x1b[?1006l");
    tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);

    free(framebuffer);
    free(clear_txt);
    return 0;
}
