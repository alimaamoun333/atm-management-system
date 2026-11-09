#ifndef INPUT_H
#define INPUT_H

int ReadInt(const char prompt[], int *result);
int ReadDouble(const char prompt[], double *result);
int ReadString(
    const char prompt[],
    char result[],
    int resultSize
);



#endif