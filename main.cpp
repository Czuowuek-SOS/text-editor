#include <iostream>
#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#if defined _WIN32
    #include <Windows.h>
    #include <conio.h>
    /* for eliminate fake error */
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

#define bg_red     "\033[41m"
#define bg_green   "\033[42m"
#define bg_yellow  "\033[43m"
#define bg_blue    "\033[44m"
#define bg_magenta "\033[45m"
#define bg_cyan    "\033[46m"
#define bg_white   "\033[47m"
#define bg_fiolet  "\033[48m"


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
void get_terminal_size(void);
bool isCntrlKey(char c);
void moveCursor(int x, int y);
void clear(void);

// cursor position
int cursor_x = 1;
int cursor_y = 1;

string file_name;

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

    file_name = argv[1];

    FILE *fp = fopen(argv[1], "r+");
    if (fp == NULL)
    {
        std::cout << red << "File not found" << reset << newl;
        exit(1);
    }



    get_terminal_size();
    clear();
    // load_file(fp);
    program_started = true;

    int lines_size[height];
    int input_count = 0;
    int line_count  = 1;
    int line_length = 0;
    if (atoi(argv[2]) == 1)
    {
        load_file(fp);
        // get line count in input
        for (int i = 0; i < strlen(input); i++)
        {
            if (input[i] == newl)
            {
                line_count++;
            }
        }
        // get lines size in input
        for (int i = 0; i < strlen(input); i++)
        {
            if (input[i] == newl)
            {
                lines_size[line_count - 1] = line_length;
                line_length = 0;
            }
            else
            {
                line_length++;
            }
        }
        input_count = strlen(input);

        cursor_x = lines_size[--line_count] + 1;
        cursor_y = line_count;
    }
    screen_refresh();

    char c;
    while (true)
    {
        c = getch();

        switch (c)
        {
            // macros
            case esc:
            {
                exit(0);
                break;
            }
            case CTRL('Q'):
            {
                exit(0);
                break;
            }
            case CTRL('S'):
            {
                if (sizeof(input) > 512)
                {
                    std::cout << red << "File too big" << reset << newl;
                    exit(1);
                }
                fseek(fp, 0, SEEK_END);
                fprintf(fp, "%s", input);
                break;
            }
            case CTRL('R'):
            {
                get_terminal_size();
            }
            // arrow keys

            
            case arrow_up:
            {
                // if (input_count > 0)
                // {
                //     line_count--;
                //     input_count--;

                //     cursor_y--;
                // }
                break;
            }
            case arrow_down:
            {
                // if (input_count < strlen(input))
                // {
                //     line_count++;
                //     input_count++;

                //     cursor_y++;
                // }
                break;
            }
            

            case arrow_left:
            {
                if (input_count > 0)
                {
                    if(input[input_count] == '\n')
                    {
                        line_count--;

                        cursor_y--;
                        cursor_x = lines_size[line_count];
                    }
                    else
                    {
                        cursor_x--;
                    }
                }
                input_count--;
                break;
            }
            case arrow_right:
            {
                if (input_count < strlen(input))
                {

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
                input_count++;
                break;
            }
            // scroll screen
            /*
            case CTRL(arrow_up):
            {
                scroll_up(1);
                break;
            }

            case CTRL(arrow_down):
            {
                scroll_down(1);
                break;
            }
            */
            // ansi escape sequences
            case 13:
            {
                input[input_count] = '\n';
                line_count++;
                input_count++;

                cursor_y++;
                cursor_x = 1;
                break;  
            }
            case '\t':
            {
                input[input_count] = '\t';
                input_count++;

                cursor_x += 3;
                break;
            }
            case '\b':
            {
                if (input_count > 0)
                {
                    if(input_count < strlen(input))
                    {
                        for(int i = input_count; i < strlen(input); i++)
                        {
                            input[input_count + i] = input[i + 1];
                        }
                    }

                    if(input[input_count] == '\n')
                    {
                        line_count--;
                        cursor_y--;
                        cursor_x = lines_size[line_count];
                    }
                    else
                    {
                        cursor_x--;
                        lines_size[line_count]--;
                    }

                    input_count--;
                    input[input_count] = 0;
                    // for(int i = input_count ; i < strlen(input) ; i++)
                    // {
                    //     input[i] = input[i + 1];
                    // }
                }
                break;
            }
            // normal characters
            default:
            {
                if (input_count > sizeof(input) - 1)
                {
                    std::cout << red << "File too big" << reset << newl;
                    exit(1);
                }
                // if (iscntrl(c) != 0 && c != '\n' && c != 13 && c != '\b' && c != '\r' && c != '\t')
                // {
                //     // std::cout << yellow << (int)c << reset << newl;
                //     break;
                // }

                //    if (isCntrlKey(c))
                //    {
                //        break;
                //    }
                if (!(c > 31 && c < 127))
                {
                    break;
                }

                if (input_count < strlen(input))
                {
                    for(int i = 0 ; i < strlen(input) - input_count ; i++)
                    {
                        input[input_count + i] = input[input_count + i + 1];
                    }
                }
           

                cursor_x++;

                input[input_count] = c;
                input_count++;
                line_length++;
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

    int line_length = 0;
    int line_count  = 1;
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
            case '\n': // '\n' - newl
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
                std::cout << space;
                line_length++;
                break;
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
    std::cout << '\n';
    int chars_count = 0;
    for(int i = 0 ; i < sizeof(input) ; i++)
    {
        if(input[i] != '\0')
        {
            chars_count++;
        }
    }

    for(int i = 0 ; i < (height - line_count) - 1 ; i++)
    {
        std::cout << green << "~" << reset << newl;
    }
    string info = "lines: " + std::to_string(line_count) + " | " + "chars: " + std::to_string(chars_count) + ' ' + '[' + file_name + ']';
    std::cout << bg_blue << info;
    for(int i = 0 ; i < (width - info.length()) ; i++)
    {
        std::cout << space;
    }
    std::cout << reset;
    moveCursor(cursor_x, cursor_y);
}

void get_terminal_size()
{
    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        int ret;
        ret = GetConsoleScreenBufferInfo(GetStdHandle( STD_OUTPUT_HANDLE ), &csbi);
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
        clear();
        std::cout << red << "Error getting terminal size - 1" << reset << newl;
        exit(1);
    }

}

void load_file(FILE* fp)
{   
    int i = 0;
    char c;
    // load file content to input
    while((c = fgetc(fp)) != EOF)
    {
        input[i] = c;
        i++;
    }
}

bool isCtrlKey(char c)
{
    return c == CTRL(c);
}

void moveCursor(int x, int y)
{
    printf("\033[%d;%dH", y, x);
}

void clear(void)
{
    // #if defined(_WIN32)
    //     system("cls");
    // #else
    //     system("clear");
    // #endif
    std::cout << "\033[H\033[2J\033[3J";
    // std::cout << "\033[2J\033[1;1H";
}
