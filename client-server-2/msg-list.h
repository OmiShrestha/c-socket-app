#ifndef MSG_LIST_H
#define MSG_LIST_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "protocol.h"

// Added By: Daniel
// Edited By: Omi

// Function prototypes
void initMessageList(MessageList *msgList);
void appendMessage(MessageList *msgList, Message *message);
Message *createMessage(char* msgString, User *sender);
void printMessageList(MessageList *msgList);
void freeMessageList(MessageList *msgList);

#endif // MSG_LIST_H
