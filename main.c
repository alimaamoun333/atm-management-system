#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include  "user.h"
#include "auth.h"
#include "menu.h"
#include "user_store.h"

int main(void) {
    if (!InitializeUserStore())
    {
        printf("Failed to initialize user storage.\n");
        return 1;
    }

    struct User currentUser = {0};
    int choice = 0;
    do {
        ShowMainMenu();
        choice = ReadMainChoice();
        HandleMainChoice(choice, &currentUser);
    } while (choice != 4);
    return 0; 
}




 
