#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <signal.h>

int width, height;
char framebuffer[1000][1000];

void display_fb(char* text_to_display) {
    int len = strlen(text_to_display);
    int a = 0, b = 0;
    for (int i = 0; i < len; i++) {
        framebuffer[b][a] = text_to_display[i];
        
        a++;
        if (a >= width) {
            a = 0;
            b++;
        }
    }
}

void clear_buffer() {
    for (int i = 0; i < 1000; i++) {
        memset(framebuffer[i], '\0', 1000);
    }
}

void print_screen() {
    int i = 0;
    char *to_print = framebuffer[i];
    while(strlen(to_print) > 0) {
        printf("%s\n", to_print);
        i++;
        to_print = framebuffer[i];
    }
}

void handle_width(int sig) {
    // This function is called when SIGWINCH is received
    // You can put your screen redrawing logic here

    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        struct ttysize ts;
        ioctl(0, TIOCGSIZE, &ts);
        height = ts.ts_lines;
        width = ts.ts_cols;

        clear_buffer();

        char to_display[100];
        memset(to_display, '\0', 100);
        sprintf(to_display, "Terminal resized to: %d rows, %d columns\n", height, width);
        display_fb(to_display);
        
        system("clear");
        print_screen();
    }
}

void hide_cursor() {
    printf("\e[?25l"); // ANSI escape code to hide the cursor
    fflush(stdout);    // Ensure the output is immediately sent to the terminal
}

void show_cursor() {
    printf("\e[?25h"); // ANSI escape code to show the cursor
    fflush(stdout);    // Ensure the output is immediately sent to the terminal
}

int main() {
    // initialize the frame buffer
    struct ttysize ts;
    ioctl(0, TIOCGSIZE, &ts);
    height = ts.ts_lines;
    width = ts.ts_cols;

    clear_buffer();

    // Register the signal handler for SIGWINCH
    signal(SIGWINCH, handle_width);

    // Get current terminal settings
    struct termios old_settings, new_settings;
    tcgetattr(STDIN_FILENO, &old_settings);
    new_settings = old_settings;
    new_settings.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);

    hide_cursor();

    system("clear");
    printf("\x1b[?1000h\x1b[?1006h");

    char to_display[100];
    memset(to_display, '\0', 100);
    sprintf(to_display, "Terminal resized to: %d rows, %d columns\n", height, width);
    display_fb(to_display);
    print_screen();

    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        system("clear");

        char to_display[100];
        memset(to_display, '\0', 100);
        sprintf(to_display, "Terminal resized to: %d rows, %d columns\n", height, width);
        display_fb(to_display);

        print_screen();
    }

    system("clear");
    show_cursor();
    printf("\x1b[?1000l\x1b[?1006l");
    tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);

    return 0;
}
