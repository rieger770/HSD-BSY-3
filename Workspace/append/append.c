#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#define BUFF_SIZE 512
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) // zugriffsberechtigungen

static short const EXIT_FAILURE = 1;
static short const EXIT_SUCCESS = 0;

static const char *USAGE = "usage: append source target\n";
static const char *ERR_OPEN_SRC = "error: cannot open source file\n";
static const char *ERR_OPEN_DST = "error: cannot open target file\n";
static const char *ERR_READ = "error in read\n";
static const char *ERR_WRITE = "error in write\n";

static void PrintError(const char *msg)
{
    write(STDERR_FILENO, msg, strlen(msg));
}

static void PrintBytesAmount(unsigned long value)
{
    char buffer[32];                  // reicht für jede 64-Bit-Zahl plus '\n'
    size_t pos = sizeof(buffer);

    buffer[--pos] = '\n';   // 32. Element als Zeilenumbruch
    do {
        buffer[--pos] = (char)('0' + (value % 10)); // Ziffern von hinten nach vorne eintragen (niedrigste Ziffer zuerst)
        value /= 10;
    } while (value != 0);             // do-while: auch 0 wird ausgegeben

    write(STDOUT_FILENO, &buffer[pos], sizeof(buffer) - pos);
}

int main(int argc, char *argv[])
{
    char buffer[BUFF_SIZE];
    long total = 0; // anzahl an geschriebenen bytes

    if (argc != 3) { // falscher aufruf des programms
        PrintError(USAGE);
        return EXIT_FAILURE;
    }
    if(strcmp(argv[1], argv[2]) == 0) // beide argumente die selbe file! sonst -> endlosschleife!
    {
        PrintError(USAGE);
        return EXIT_FAILURE;
    }

int src = open(argv[1], O_RDONLY); // oeffne source file
if (src == -1) { PrintError(ERR_OPEN_SRC); return EXIT_FAILURE; }

int dst = open(argv[2], O_WRONLY | O_CREAT | O_APPEND, FILE_MODE); // oeffne target destination
if (dst == -1) { PrintError(ERR_OPEN_DST); close(src); return EXIT_FAILURE; }

    ssize_t n;
    while ((n = read(src, buffer, BUFF_SIZE)) > 0) { // 0 = EOF, -1 = error
        if (write(dst, buffer, n) != n) {
            PrintError(ERR_WRITE);
            close(src); close(dst);
            return EXIT_FAILURE;
        }
        total += n;
    }
    if (n == -1) { PrintError(ERR_READ); close(src); close(dst); return EXIT_FAILURE; }

    PrintBytesAmount(total); // rufe ausgabe funktion auf fuer anzahl der geschriebenen bytes

    close(src);
    close(dst);
    return EXIT_SUCCESS;
}