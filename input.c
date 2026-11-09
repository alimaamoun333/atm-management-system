#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include "input.h"

int ReadDouble(const char prompt[], double *result)
{
    char buffer[100];
    char *endPointer;
    double value;
    int character;

    if (prompt == NULL || result == NULL)
    {
        return 0;
    }

    printf("%s", prompt);

    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
    {
        return 0;
    }

    if (strchr(buffer, '\n') == NULL)
    {
        while (
            (character = getchar()) != '\n' &&
            character != EOF
        )
        {
        }

        return 0;
    }

    errno = 0;
    value = strtod(buffer, &endPointer);

    if (endPointer == buffer || errno == ERANGE || !isfinite(value))
    {
        return 0;
    }

    while (isspace((unsigned char)*endPointer))
    {
        endPointer++;
    }

    if (*endPointer != '\0')
    {
        return 0;
    }

    *result = value;
    return 1;
}

int ReadInt(const char prompt[], int *result)
{
    char buffer[100];
    char *endPointer;
    long value;
    int character;

    if (prompt == NULL || result == NULL)
    {
        return 0;
    }

    printf("%s", prompt);

    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
    {
        return 0;
    }

    if (strchr(buffer, '\n') == NULL)
    {
        while (
            (character = getchar()) != '\n' &&
            character != EOF
        )
        {
        }

        return 0;
    }

    errno = 0;
    value = strtol(buffer, &endPointer, 10);

    if (endPointer == buffer || errno == ERANGE)
    {
        return 0;
    }

    while (isspace((unsigned char)*endPointer))
    {
        endPointer++;
    }

    if (*endPointer != '\0')
    {
        return 0;
    }

    if (value < INT_MIN || value > INT_MAX)
    {
        return 0;
    }

    *result = (int)value;
    return 1;
}

int ReadString(
    const char prompt[],
    char result[],
    int resultSize
)
{
    int character;
    char *newline;

    if (prompt == NULL || result == NULL || resultSize <= 1)
    {
        return 0;
    }

    printf("%s", prompt);

    result[0] = '\0';

    if (fgets(result, resultSize, stdin) == NULL)
    {
        return 0;
    }

    newline = strchr(result, '\n');

    if (newline == NULL)
    {
        while (
            (character = getchar()) != '\n' &&
            character != EOF
        )
        {
        }

        result[0] = '\0';
        return 0;
    }

    *newline = '\0';

    if (result[0] == '\0')
    {
        return 0;
    }

    return 1;
}
