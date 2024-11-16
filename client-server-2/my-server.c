#include "server-helper.h"
#include "msg-list.h"
#include "user-list.h"

#define BACKLOG 10 // how many pending connections queue will hold

/**
 * Program name: my-server.c
 * Description:  This server program listens for client connections, processes incoming messages, 
 *               and maintains a list of messages sent by clients. It includes functionality to 
 *               send acknowledgments and handle client disconnections.
 * Compile:      gcc -o server my-server.c server-helper.c list.c
 * Run:          ./server <hostname> <port>
 */

// Function prototypes
void send_ack(int client_socket);
void *start_subserver(void *session_data);
void freeMessages(MessageList *msgList);
void freeSession(Session *session);

/**
 * Sends an acknowledgment to the client. The acknowledgment is encapsulated in a s2c_send_ok_ack struct.
 *
 * param client_socket The socket descriptor for the client connection. 
 *                     Used to send acknowledgment data back to the client.
 */
void send_ack(int client_socket) {
    s2c_send_ok_ack server_ack;
    server_ack.type = ACK_TYPE;

    if (send(client_socket, &server_ack, sizeof(s2c_send_ok_ack), 0) == -1) {
        perror("Error sending acknowledgement to client\n");
    }
}

void freeSession(Session *session) {
    freeUserList(session->userList);
    freeMessageList(session->messageList);
    free(session);
}

// Added By: Daniel & Aedan
/**
 * Manages the client connection in a loop, processing incoming messages and appending them 
 * to the provided list. Sends acknowledgments after processing each message and handles client 
 * disconnections.
 * c2s_send_message struct is used to receive messages from the client.
 *
 * param client_socket The socket descriptor for the client connection.
 * param msgList       A pointer to a list where incoming client messages will be stored.
 */
void *start_subserver (void *session_data) {

    Session *session = (Session *) session_data;
    int client_socket = session->socketFd;
    MessageList *messageList = session->messageList;
    UserList *userList = session->userList;

    // Registration handling: Ensure user is registered before processing messages
    int isRegistered = 0;

    while (1) {

        // Initialize client message
        c2s_send_message client_message;

        // Receive message from client
        int bytes_received = recv(client_socket, &client_message, sizeof(client_message), 0);
        if (bytes_received == -1) {
            perror("Error receiving message from client\n");
            break;
        } else if (bytes_received == 0) {
            printf("Client disconnected. Waiting for a new connection...\n");
            break;
        }
        
        // Process client message
        if (client_message.type == REGISTRATION_TYPE) {
            // Create user and append to userList

            // Not working but would allow us to split the message into email and name
            // strdup does not cause problems with server requesting all messages but does not separate the email and name
            // strtok would separate the email and name, but when it is used session->user->name is changed but I am not sure why
            /*
            char *email = strtok(client_message.message, " ");
            char *name = strtok(NULL, " ");
            */
            char *email = strdup(client_message.message);
            char *name = strdup(client_message.message);
            User *user = createUser(email, name);
            appendUser(userList, user);

            printf("Client registered with email: %s, name: %s\n", user->email, user->name);

            session->user = user; // Set user for session
            // debug
            printf("User0 set for session: %s\n", session->user->name);

            send_ack(client_socket);
            isRegistered = 1;

        } else if (!isRegistered) {
            printf("Client is not registered. Ignoring message.\n");
        }
        else if (client_message.type == MESSAGE_TYPE) {
            printf("Client sent: %s\n", client_message.message);

            // Create message and append to msgList
            char *msgString = strdup(client_message.message);
            Message *msg = createMessage(msgString, session->user);
            // DEBUG
            if (msg == NULL) {
                perror("Error creating message\n");
                break;
            }
            appendMessage(messageList, msg);

            send_ack(client_socket);
        } else if (client_message.type == EXIT_TYPE) {
            printf("Client requested to exit. Closing connection...\n");
            break;
        }
        else if (client_message.type == REQUEST_ALL_MESSAGES_TYPE) {
            printf("Client requested all messages\n");

            // Loop through and send every message in the MessageList to the client
            Message *ptr = messageList->first;
            while (ptr != NULL) {
                // DEBUG
                if (ptr->sender == NULL || ptr->message == NULL) {
                    printf("Error: Null sender or message in message list\n");
                    break;
                }

                user_message msg_to_send;
                msg_to_send.type = MESSAGE_TYPE;
                strncpy(msg_to_send.name, ptr->sender->name, BUFFER_SIZE - 1);
                msg_to_send.name[BUFFER_SIZE - 1] = '\0'; // Ensure null-termination
                strncpy(msg_to_send.message, ptr->message, BUFFER_SIZE - 1);
                msg_to_send.message[BUFFER_SIZE - 1] = '\0'; // Ensure null-termination

                if (send(client_socket, &msg_to_send, sizeof(user_message), 0) == -1) {
                    perror("Error sending message to client\n");
                    break;
                }
                ptr = ptr->next;
            }

            // Send an end-of-messages indicator
            user_message end_msg;
            end_msg.type = MESSAGE_TYPE;
            snprintf(end_msg.message, BUFFER_SIZE, "END_OF_MESSAGES");
            end_msg.name[0] = '\0'; // No user for end of messages
            if (send(client_socket, &end_msg, sizeof(user_message), 0) == -1) {
                perror("Error sending end-of-messages indicator to client\n");
            }

            // Send acknowledgment to client
            send_ack(client_socket);
        } else {
            printf("Client sent invalid message type: %d\n", client_message.type);
        }
    }
    close(client_socket);
    printf("Client disconnected. Waiting for a new connection...\n");
    freeSession(session);
    pthread_exit(NULL);
    return 0;
}

/**
 * Main function to start the server and handle client connections.
 *
 * param argc Number of command-line arguments.
 * param argv Array of command-line arguments. The first argument should be the hostname,
 *            and the second argument should be the port number.
 * return 0 on successful execution.
 */
int main(int argc, char *argv[]) {
    int server_socket;  // http server socket
    int client_socket;  // client connection
    UserList userList;
    MessageList messageList;

    initUserList(&userList);
    initMessageList(&messageList);

    server_socket = start_server(argv[1], argv[2], BACKLOG);
    if (server_socket == -1) {
        printf("Error starting server\n");
        exit(1);
    }

    while (1) {
        // Accept connection from client
        if ((client_socket = accept_client(server_socket)) == -1) {
            continue;
        }

        // Added By: Daniel
        Session *session = (Session *) malloc(sizeof(Session));
        session->socketFd = client_socket;
        session->messageList = &messageList;
        session->userList = &userList;
        session->user = NULL; // Initialize user to NULL, later set by registration

        // Added By: Aedan
        // Creating a new thread for each client connection
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, start_subserver, (void *) session) != 0) {
            perror("Error creating thread\n");
            close(client_socket);
            free(session);
            continue;
        }

        // Detaching the thread allows thread resources to be auto released on termination
        pthread_detach(thread_id);
    }

    close(server_socket);
    freeMessageList(&messageList);
    freeUserList(&userList);
    free(&messageList);
    free(&userList);
    return 0;
}
