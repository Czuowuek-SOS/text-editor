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
char getch();
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


bool program_started = false;
int main(int argc, char *argv[2])
{
    atexit([]()
    {   
        if(program_started) {cls();}
    });

    if (argc != 1)
    {
        std::cout << red << "Expected arguments" << reset << newl;
    }

    FILE *fp = fopen(argv[1], "r+");
    if(fp == NULL)
    {
        std::cout << red << "File not found" << reset << newl;
        exit(1);
    }

    program_started = true;

    int input_count = 0;
    int line_count = 1;
    char c;
    char input[512];
    while (true)
    {
        c = getch();

        switch (c)
        {
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

            case '\n':
            {
                std::cout << newl << line_Sep;
                line_count++;
                break;
            }
            case '\b':
            {
                if (input_count > 0)
                {
                    input[--input_count] = '\0';
                    std::cout << "\b \b";
                }
            }


            default:
            {

                input[input_count] = c;
                input_count++;
                std::cout << c;
                break;
            }
        }
    }
    

    return 0;
}




void cls()
{
    std::cout << "\033[2J\033[1;1H";
}