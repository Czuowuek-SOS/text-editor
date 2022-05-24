#include <iostream>
#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#if defined _WIN32
    #include <Windows.h>
    #include <conio.h>
    /* for eliminate error */
    #define STD_OUTPUT_HANDLE (DWORD)(0xfffffff5)
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__) || defined(__APPLE__)
    #include <cstring>
    #include <unistd.h>
    #include <termios.h>
    #include <sys/ioctl.h>
    char getch()
    {
        struct termios oldt, newt;
        char ch;
        tcgetattr( STDIN_FILENO, &oldt );
        newt = oldt;
        newt.c_lflag &= ~( ICANON | ECHO );
        tcsetattr( STDIN_FILENO, TCSANOW, &newt );
        ch = getchar();
        tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
        return ch;
    }

    void raw_mode(bool set)
    {
        static struct termios oldt, newt;
        if (set)
        {
            tcgetattr(STDIN_FILENO, &oldt);
            newt = oldt;
            newt.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        }
        else
        {
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        }
    }
#endif

#define newl    '\n'
#define tab     '\t'
#define space   ' '

#define red     "\033[31m"
#define green   "\033[32m"
#define yellow  "\033[33m"
#define blue    "\033[34m"
#define magenta "\033[35m"
#define cyan    "\033[36m"
#define white   "\033[37m"
#define fiolet  "\033[38m"

#define reset   "\033[0m"


#define CTRL(k) ((k) & 0x1f)

#define arrow_up    72
#define arrow_down  80
#define arrow_left  75
#define arrow_right 77

#define esc         27

//#define line_Sep "\033[32m ~ \033[0m"
#define line_Sep " ~ "

#define scroll_up(n)     printf("\033[%dS", n);
#define scroll_down(n)   printf("\033[%dT", n);

using std::string;

void load_file(FILE *fp);
void screen_refresh(void);
void moveCursor(int x, int y);
void clear(void);

// cursor position
int cursor_x = 0;
int cursor_y = 0;

int width;
int height;
char input[512];
bool program_started = false;
int main(int argc, char *argv[1])
{
    atexit([]()
    {   
        if(program_started)
        {
            clear();
        }
    });

    if (argc < 1)
    {
        std::cout << red << "Expected arguments" << reset << newl;
    }

    FILE *fp = fopen(argv[1], "r+");
    if(fp == NULL)
    {
        std::cout << red << "File not found" << reset << newl;
        exit(1);
    }

    // get terminal size
    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        int ret;
        ret = GetConsoleScreenBufferInfo(GetStdHandle( STD_OUTPUT_HANDLE ),&csbi);
        if(ret)
        {
            width  = csbi.dwSize.X;
            height = csbi.dwSize.Y;
        }
        else
        {
            std::cout << red << "Error getting terminal size - 0" << reset << newl;
            exit(1);
        }

    #elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__) || defined(__APPLE__)
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        width = w.ws_col;
        height = w.ws_row;
    #endif
    if (width == 0 || height == 0)
    {
        std::cout << red << "Error getting terminal size - 1" << reset << newl;
        exit(1);
    }

    clear();
    // load_file(fp);
    program_started = true;
    int input_count = 0;
    int line_count  = 1;
    char c;
    while (true)
    {
        c = getch();

        switch (c)
        {
            // macros
            case CTRL('Q'):
            {
                exit(0);
                break;
            }
            case CTRL('S'):
            {
                fseek(fp, 0, SEEK_END);
                fprintf(fp, "%s", input);
                break;
            }
            case CTRL('R'):
            {
                FILE *fp2 = fopen("program.exe", "r");
                if (fp2 != NULL)
                {
                    fclose(fp2);
                    remove("program.exe");
                }
                clear();
                system("g++ main.cpp -o program.exe && .\\program.exe");

                std::cout << "Press any key to continue..." << newl;
                getch();
            }
            // arrow keys
            case arrow_up:
            {
                if (input_count > 0)
                {
                    line_count--;
                    input_count--;

                    cursor_y--;
                }
                break;
            }
            case arrow_down:
            {
                if (input_count < strlen(input))
                {
                    line_count++;
                    input_count++;

                    cursor_y++;
                }
                break;
            }
            case arrow_left:
            {
                if (input_count > 0)
                {
                    input_count--;
                    if(input[input_count] == '\n')
                    {
                        line_count--;
                        cursor_y--;
                        cursor_x = 0;
                    }
                    else
                    {
                        cursor_x--;
                    }
                }
                break;
            }
            case arrow_right:
            {
                if (input_count < strlen(input))
                {
                    input_count++;
                    if(input[input_count] == '\n')
                    {
                        line_count++;
                        cursor_y++;

                        cursor_x = 0;
                    }
                    else
                    {
                        cursor_x++;
                    }
                }
                break;
            }
            // ansi escape sequences
            case '\n':
            {
                line_count++;
                input_count++;

                input[input_count] = '\n';
                break;
            }
            case '\b':
            {
                if (input_count > 0)
                {
                    input_count--;
                    if(input[input_count] == '\n')
                    {
                        line_count--;
                        cursor_y--;
                        cursor_x = 0;
                    }
                    else
                    {
                        cursor_x--;
                    }

                    for(int i = input_count ; i < strlen(input) ; i++)
                    {
                        input[i] = input[i + 1];
                    }
                }
                break;
            }
            // normal characters
            default:
            {
                if (iscntrl(c) != 0)
                {
                    break;
                }

                input[input_count] = c;
                input_count++;
                break;
            }
        }
        screen_refresh();
    }
    

    return 0;
}

void screen_refresh(void)
{
    clear();

    int line_length;
    int line_count = 1;
    for(int i = 0 ; i < sizeof(input) ; i++)
    {
        char previos_char   = input[--i];
        char next_char      = input[++i];


        switch (input[i])
        {
            case '\0':
            {
                break;
            }
            case newl: // '\n'
            {
                std::cout << newl;
                line_length = 0;
                line_count++;
                break;
            }
            case tab: // '\t'
            {
                std::cout << "   ";
                break;
            }
            case space: // ' '
            {
                std::cout << reset <<space;
            }

            default:
            {   
                if (line_length == width || line_count == height)
                {
                    exit(-7);
                    std::cout << newl;
                    line_length = 0;
                    line_count++;
                }

                std::cout << input[i];
            }
        }
    }
    moveCursor(cursor_y, cursor_x);
}


void load_file(FILE* fp)
{   
    int i = 0;
    char c;
    while (true)
    {
        c = fgetc(fp);
        if (c != EOF)
        {
            input[i] = c;
            i++;
        }
        else
        {
            input[i] = '\0';
            break;
        }

    }
}

void moveCursor(int x, int y)
{
    printf("\033[%d;%dH", y, x);
}


void clear(void)
{
    #if defined(_WIN32)
        system("cls");
    #else
        system("clear");
    #endif
    //std::cout << "\033[2J\033[1;1H";
}
