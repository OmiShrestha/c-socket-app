#ifndef USER_LIST_H
#define USER_LIST_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "protocol.h"

// Added By: Daniel

// Function prototypes
void initUserList(UserList *userList);
void appendUser(UserList *userList, User *user);
User *createUser(char *email, char *name);
void printUserList(UserList *userList);
void freeUserList(UserList *userList);

#endif // USER_LIST_H
