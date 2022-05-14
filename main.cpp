#include <iostream>
#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#if defined _WIN32
    #include <Windows.h>
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

//#define line_Sep "\033[32m ~ \033[0m"
#define line_Sep " ~ "

using std::string;

void load_file(FILE *fp);
void screen_refresh(void);
void moveCursor(int x, int y);
void clear(void);


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

    //make variables with terminal size


    clear();
    // load_file(fp);
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
                    screen_refresh();
                }
                break;
            }
            case arrow_down:
            {
                if (input_count < strlen(input))
                {
                    line_count++;
                    input_count++;
                    screen_refresh();
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
                    }
                    screen_refresh();
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
                    }
                    screen_refresh();
                }
                break;
            }
            // ansi escape sequences
            case '\n':
            {
                line_count++;
                input_count++;

                input[input_count] = c;
                break;
            }
            case '\b':
            {
                if (input_count > 0)
                {
                    input_count--;
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

    for(int i = 0 ; i < sizeof(input) ; i++)
    {
        char previos_char   = input[--i];
        char next_char      = input[++i];

        int line_length;
        if(input[i] == '\n')
        {
            line_length = i;
            break;
        }


        switch (input[i])
        {
            case '\0':
            {
                break;
            }
            case newl: // '\n'
            {
                std::cout << newl;
                /*
                if (next_char == '\n')
                {
                    std::cout << newl;
                }
                else
                {
                    std::cout << newl;
                }
                */
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
                /* syntax highlighting

                if(input[++i] == 'i' && input[++i] == 'n' && input[++i] == 't')
                {
                    std::cout << blue << input[i] << reset;
                }
                else if(input[++i] == 's' && input[++i] == 't' && input[++i] == 'r' && input[++i] == 'i' && input[++i] == 'n' && input[++i] == 'g')
                {
                    std::cout << green << input[i] << reset;
                }
                else if(input[++i] == 'c' && input[++i] == 'h' && input[++i] == 'a' && input[++i] == 'r')
                {
                    std::cout << blue << input[i] << reset;
                }
                else if(input[++i] == 'f' && input[++i] == 'l' && input[++i] == 'o' && input[++i] == 'w' && input[++i] == 'e' && input[++i] == 'r')
                {
                    std::cout << blue << input[i] << reset;
                }
                else if(input[++i] == '(')
                {
                    std::cout << magenta << input[i] << reset;
                }
                else if(input[++i] == ')')
                {
                    std::cout << magenta << input[i] << reset;
                }
                */
                std::cout << input[i];
            }
        }
    }
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
