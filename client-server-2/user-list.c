#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "protocol.h"

// Added By: Daniel

void initUserList(UserList *userList) {
    userList->first = NULL;
    userList->last = NULL;
    userList->count = 0;
}

void appendUser(UserList *userList, User *user) {
    if (userList->first == NULL) {
        userList->first = user;
        userList->last = user;
    } else {
        userList->last->next = user;
        userList->last = user;
    }
    user->next = NULL;
    userList->count++;
}

User *createUser(char *email, char *name) {
    User *user = (User *) malloc(sizeof(User));
    // DEBUG
    if (user == NULL) {
        perror("Error allocating memory for user");
        return NULL;
    }
    user->email = email;
    user->name = name;
    user->next = NULL;
    return user;
}

void freeUserList(UserList *userList) {
    User *ptr = userList->first;
    for (int i = 0; i < userList->count; i++) {
        User *temp = ptr;
        ptr = ptr->next;
        free(temp->email);
        free(temp->name);
        free(temp);
    }
    userList->first = NULL;
    userList->last = NULL;
    userList->count = 0;
}
