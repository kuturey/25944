#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define MAX_LINES 100

int main()
{
    int fd;
    struct stat st;
    char *data;

    long offsets[MAX_LINES];
    long lengths[MAX_LINES];

    int line_count = 0;
    long start = 0;
    long i;
    int number;

    fd = open("text.txt", O_RDONLY);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    if (fstat(fd, &st) == -1) {
        perror("fstat");
        close(fd);
        return 1;
    }

    if (st.st_size == 0) {
        printf("File is empty\n");
        close(fd);
        return 0;
    }

    data = mmap(NULL,
                st.st_size,
                PROT_READ,
                MAP_PRIVATE,
                fd,
                0);

    if (data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    offsets[0] = 0;

    for (i = 0; i < st.st_size; i++) {

        if (data[i] == '\n') {

            lengths[line_count] = i - start + 1;

            line_count++;

            if (line_count >= MAX_LINES) {
                break;
            }

            offsets[line_count] = i + 1;
            start = i + 1;
        }
    }

    if (start < st.st_size && line_count < MAX_LINES) {
        lengths[line_count] = st.st_size - start;
        line_count++;
    }

    printf("Line table:\n");

    for (number = 0; number < line_count; number++) {
        printf("Line %d: offset = %ld, length = %ld\n",
               number + 1,
               offsets[number],
               lengths[number]);
    }

    while (1) {

        printf("\nEnter line number (0 to exit): ");

        if (scanf("%d", &number) != 1) {
            printf("Invalid input\n");
            break;
        }

        if (number == 0) {
            break;
        }

        if (number < 1 || number > line_count) {
            printf("No such line\n");
            continue;
        }

        for (i = 0; i < lengths[number - 1]; i++) {
            putchar(data[offsets[number - 1] + i]);
        }
    }

    if (munmap(data, st.st_size) == -1) {
        perror("munmap");
    }

    close(fd);

    return 0;
}
