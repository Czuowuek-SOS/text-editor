#include <iostream>
#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#if defined _WIN32
    #include <windows.h>
    #include <conio.h>
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__) || defined(__APPLE__)
    #include <unistd.h>
    #include <termios.h>
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
#endif

#define newl    '\n'

#define red     "\033[31m"
#define green   "\033[32m"
#define blue    "\033[34m"
#define yellow  "\033[33m"
#define magenta "\033[35m"
#define cyan    "\033[36m"
#define white   "\033[37m"

#define reset   "\033[0m"


#define CTRL(k) ((k) & 0x1f)

//#define line_Sep "\033[32m ~ \033[0m"
#define line_Sep " ~ "

using std::string;

void screen_refresh(void);
void clear(void);



extern char input[512];
bool program_started = false;
int main(int argc, char *argv[2])
{
    atexit([]()
    {   
        if(program_started) {clear();}
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

    clear();
    program_started = true;
    int input_count = 0;
    int line_count = 1;
    char c;
    while (true)
    {
        c = getch();

        switch (c)
        {
            // macros
            case CTRL('q'):
            {
                exit(0);
                break;
            }
            case CTRL('s'):
            {
                fseek(fp, 0, SEEK_END);
                fprintf(fp, "%s", input);
                break;
            }
            // special characters
            case '\n':
            {
                std::cout << "~" << '\r' << newl << newl;
                line_count++;
                break;
            }
            case '\b':
            {
                if (input_count > 0)
                {
                    input_count--;
                    input[input_count] = '\0';
                    std::cout << "\b \b";
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
                std::cout << c;
                break;
            }
        }
    }
    

    return 0;
}

void screen_refresh(void)
{
    clear();

    for(int i = 0 ; i < sizeof(input) / sizeof(input[0]) ; i++)
    {
        if(input[i] == '\0')
        {
            break;
        }
        else if(input[i] == '\n')
        {
            std::cout << line_Sep << newl;
        }
        else
        {
            std::cout << input[i];
        }
    }
}


void clear(void)
{
    std::cout << "\033[2J\033[1;1H";
}
