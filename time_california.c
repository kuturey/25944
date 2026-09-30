#include <stdio.h>
#include <time.h>

int main()
{
    time_t current_time;
    struct tm *california_time;

    current_time = time(NULL);

    /* California PST = UTC - 8 hours */
    current_time = current_time - 8 * 60 * 60;

    california_time = gmtime(&current_time);

    printf("California time: %s", asctime(california_time));

    return 0;
}
