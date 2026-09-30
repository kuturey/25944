#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node
{
    char *text;
    struct Node *next;
};

int main()
{
    char buffer[1024];

    struct Node *head = NULL;
    struct Node *tail = NULL;
    struct Node *node;

    while (1)
    {
        printf("Enter string: ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            break;

        if (buffer[0] == '.')
            break;

        node = malloc(sizeof(struct Node));

        if (node == NULL)
        {
            perror("malloc");
            return 1;
        }

        node->text = malloc(strlen(buffer) + 1);

        if (node->text == NULL)
        {
            perror("malloc");
            free(node);
            return 1;
        }

        strcpy(node->text, buffer);
        node->next = NULL;

        if (head == NULL)
        {
            head = node;
            tail = node;
        }
        else
        {
            tail->next = node;
            tail = node;
        }
    }

    printf("\nStrings:\n");

    node = head;

    while (node != NULL)
    {
        struct Node *next;

        printf("%s", node->text);

        next = node->next;

        free(node->text);
        free(node);

        node = next;
    }

    return 0;
}
