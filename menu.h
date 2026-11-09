#ifndef MENU_H
#define MENU_H

#include "user.h"


void ShowMainMenu(void);
int ReadMainChoice(void);
void HandleMainChoice(int choice, struct User *user);

void ShowATMMenu(void);
int ReadATMChoice(void);
int HandleATMChoice(int choice, struct User *user);
void RunATMMenu(struct User *user);

#endif