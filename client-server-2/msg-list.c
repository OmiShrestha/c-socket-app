#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "protocol.h"
#include "msg-list.h"

// Added By: Daniel
// Edited By: Omi

void initMessageList(MessageList *msgList) {
   msgList->first = NULL;
   msgList->last = NULL;
   msgList->count = 0;
}

void appendMessage(MessageList *msgList, Message *message) {
   if (msgList->first == NULL) {
      msgList->first = message;
      msgList->last = message;
   }
   else {
      msgList->last->next = message;
      msgList->last = message;
   }
   msgList->count++;
}

Message *createMessage(char* msgString, User *sender) {
   Message *msg = (Message *) malloc(sizeof(Message));
   // DEBUG
   if (msg == NULL) {
        perror("Error allocating memory for message");
        return NULL;
   }
   msg->message = msgString;
   msg->sender = sender;
   msg->next = NULL;
   return msg;
}

void printMessageList(MessageList *msgList) {
   Message *ptr = msgList->first;
   for (int i = 0; i < msgList->count; i++) {
      printf("Message from user (%s): %s\n", ptr->sender->name, ptr->message);
      ptr = ptr->next;
   }
}

// Added By: Daniel
/**
 * Frees the memory allocated for the messages in the provided list.
 *
 * param msgList A pointer to a list of messages.
 */
void freeMessageList(MessageList *msgList) {
    Message *ptr = msgList->first;
    for (int i = 0; i < msgList->count; i++) {
        Message *temp = ptr;
        ptr = ptr->next;
        free(temp->message);
        free(temp);
    }
    msgList->first = NULL;
    msgList->last = NULL;
    msgList->count = 0;
}
