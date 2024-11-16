#ifndef PROTOCOL_H
#define PROTOCOL_H

#define BUFFER_SIZE 256
#define MESSAGE_TYPE 2
#define EXIT_TYPE 99
#define ACK_TYPE 200

// Added By: Omi
#define REGISTRATION_TYPE 1 
#define REQUEST_ALL_MESSAGES_TYPE 3 

/**
 * Struct name: c2s_send_message
 * Description: Represents a message sent from the client to the server.
 *
 * param type    The type identifier for a message. Should be MESSAGE_TYPE.
 * param length  The length of the message.
 * param message A character buffer holding the actual message content.
 */
typedef struct {
    int type;    // type = 2
    int length;  // Length of the message
    char message[BUFFER_SIZE]; // The actual message
} c2s_send_message;


// Added By: Aedan
// Struct used to handle passing user and messages to the client
typedef struct {
    int type;
    char name[BUFFER_SIZE];
    char message[BUFFER_SIZE];
} user_message;

/**
 * Struct name: c2s_send_exit
 * Description: Represents an exit signal sent from the client to the server.
 *
 * param type The type identifier for an exit signal. Should be EXIT_TYPE.
 */
typedef struct {
    int type; // type = 99
} c2s_send_exit;

/**
 * Struct name: s2c_send_ok_ack
 * Description: Represents an acknowledgment sent from the server to the client.
 *
 * param type The type identifier for an acknowledgment. Should be ACK_TYPE.
 */
typedef struct {
    int type; // type = 200
} s2c_send_ok_ack;

// Added By: Omi
typedef struct USER {
char *email;
char *name;
struct USER *next;
} User;

typedef struct USER_LIST {
User *first; // points to first user
User *last; // points to last user
int count; // # of the users
} UserList;

typedef struct MESSAGE {
char *message;
User *sender; // sender of the message
struct MESSAGE *next;
} Message;

typedef struct MESSAGE_LIST {
Message *first; // points to first message
Message *last; // points to last message
int count; // # of the messages
} MessageList;

typedef struct SESSION_DATA {
UserList *userList; // points to UserList
MessageList *messageList; // points to MessageList
User *user; // user of the session
int socketFd; // socket fd of the client
} Session;

#endif // PROTOCOL_H