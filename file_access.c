#include <stdio.h>
#include <unistd.h>

void print_ids()
{
    printf("Real UID: %d\n", (int)getuid());
    printf("Effective UID: %d\n", (int)geteuid());
}

void open_file()
{
    FILE *file;

    file = fopen("data.txt", "r");

    if (file == NULL) {
        perror("fopen");
    }
    else {
        printf("File opened successfully\n");
        fclose(file);
    }
}

int main()
{
    printf("Before setuid:\n");
    print_ids();
    open_file();

    if (setuid(getuid()) == -1) {
        perror("setuid");
        return 1;
    }

    printf("\nAfter setuid:\n");
    print_ids();
    open_file();

    return 0;
}
