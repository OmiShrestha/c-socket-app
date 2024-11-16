#include "client-helper.h"
#include "protocol.h"
#include "msg-list.h"
#include "user-list.h"

/**
 * Program name: my-client.c
 * Description:  This client program connects to a server to send messages, receive acknowledgments, 
 *               and exit the connection.
 * Compile:      gcc -o client my-client.c client-helper.c
 * Run:          ./client <hostname> <port>
 */

// Function prototypes
void send_messege(int server_socket, char *message);
void receive_ack(int server_socket);
void send_exit_message(int server_socket);

// Added By: Omi
void send_registration(int server_socket, char *email, char *name);
void request_all_messages(int server_socket);

// Added By: Omi
/**
 * Sends a registration message to the server with email and name
 *
 * param server_socket The socket descriptor for the server connection.
 * param email The user's email address.
 * param name The user's name.
 */
void send_registration(int server_socket, char *email, char *name) {
    c2s_send_message regis_msg;
    regis_msg.type = REGISTRATION_TYPE;
    // Prepare for use with strtok() in server
    snprintf(regis_msg.message, BUFFER_SIZE, "%s %s", email, name);
    regis_msg.length = strlen(regis_msg.message) + 1;
    
    if (send(server_socket, &regis_msg, sizeof(c2s_send_message), 0) == -1) {
        perror("Error sending registration to server\n");
    }

}

// Added By: Omi
/**
 * Requests all messages from the server
 *
 * param server_socket The socket descriptor for the server connection.
 */
void request_all_messages(int server_socket) {
    c2s_send_message req_msg;
    req_msg.type = REQUEST_ALL_MESSAGES_TYPE;
    snprintf(req_msg.message, BUFFER_SIZE, "REQUEST_ALL_MESSAGES");
    req_msg.length = strlen(req_msg.message) + 1;
    
    if (send(server_socket, &req_msg, sizeof(c2s_send_message), 0) == -1) {
        perror("Error requesting messages from server\n");
        return;
    }

    // Added By: Aedan
    // Receives the message list from server then print
    MessageList messageList;
    initMessageList(&messageList);

    while (1) {
        user_message received_msg;
        if (recv(server_socket, &received_msg, sizeof(user_message), 0) == -1) {
            perror("Error receiving message from server\n");
            break;
        }

        if (strcmp(received_msg.message, "END_OF_MESSAGES") == 0) {
            break;
        }

        // Creates a user and message, then appends it to the message list
        User *user = createUser(NULL, strdup(received_msg.name));
        if (user == NULL) {
            perror("Error creating user\n");
            break;
        }
        Message *msg = createMessage(strdup(received_msg.message), user);
        if (msg == NULL) {
            perror("Error creating message\n");
            break;
        }
        appendMessage(&messageList, msg);
    }

    // Prints the received message list
    printMessageList(&messageList);
    freeMessageList(&messageList);
}

/**
 * Sends a message to the server. The message is encapsulated in a c2s_send_message struct.
 *
 * param server_socket The socket descriptor for the server connection. 
 *                     Used to transmit the message to the server.
 * param message       A character pointer to the message that will be sent to the server.
 */
void send_messege(int server_socket, char *message) {
    c2s_send_message client_message;
    client_message.type = MESSAGE_TYPE;
    client_message.length = strlen(message) + 1;
    strncpy(client_message.message, message, BUFFER_SIZE);

    if (send(server_socket, &client_message, sizeof(c2s_send_message), 0) == -1) {
        perror("Error sending message to server\n");
    }
}

/**
 * Receives an acknowledgment from the server and prints a confirmation message 
 * if the acknowledgment is valid. Otherwise, an error message is printed.
 * The acknowledgment is encapsulated in a s2c_send_ok_ack struct.
 *
 * param server_socket The socket descriptor for the server connection.
 */
void receive_ack(int server_socket) {
    s2c_send_ok_ack server_ack;
    if (recv(server_socket, &server_ack, sizeof(s2c_send_ok_ack), 0) == -1) {
        perror("Error receiving ack from server\n");
    } else if (server_ack.type == ACK_TYPE) {
        printf("Acknowledgement received from server\n");
    } else {
        perror("Invalid acknowledgement received from server\n");
    }
}

/**
 * Sends an exit signal to the server to terminate the client session.
 * The exit signal is encapsulated in a c2s_send_exit struct.
 *
 * param server_socket The socket descriptor for the server connection.
 */
void send_exit_message(int server_socket) {
    c2s_send_exit exit_message;
    exit_message.type = EXIT_TYPE;
    if (send(server_socket, &exit_message, sizeof(c2s_send_exit), 0) == -1) {
        perror("Error sending exit message to server\n");
    }
}

// Added By: Omi
/**
 * Main function to start the client and send messages to the server.
 *
 * param argc Number of command-line arguments.
 * param argv Array of command-line arguments. The first argument should be the hostname,
 *            and the second argument should be the port number.
 * return 0 on successful execution.
 */
int main(int argc, char *argv[]) {

    // Initialize the server socket
    int server_socket = get_server_connection(argv[1], argv[2]);
    if (server_socket == -1) {
        printf("Error connecting to server\n");
        exit(1);
    }

    // Registration by email and name
    char email[BUFFER_SIZE];
    char name[BUFFER_SIZE];

    printf("Enter your email: ");
    fgets(email, BUFFER_SIZE, stdin);
    email[strcspn(email, "\n")] = '\0'; // Remove newline character

    printf("Enter your name: ");
    fgets(name, BUFFER_SIZE, stdin);
    name[strcspn(name, "\n")] = '\0'; // Remove newline character

    send_registration(server_socket, email, name);
    receive_ack(server_socket); // Acknowledge registration

    int choice;
    while(1) {
        // Menu options
        printf("\nMenu:\n");
        printf("1. Send a message\n");
        printf("2. Request all messages\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: {
                // Send a message
                char message[BUFFER_SIZE];
                printf("Enter your message: ");
                fgets(message, BUFFER_SIZE, stdin);
                message[strcspn(message, "\n")] = '\0'; // Remove newline character
                send_messege(server_socket, message);
                receive_ack(server_socket);
                break;
            }
            case 2:
                // Request all messages
                request_all_messages(server_socket);
                receive_ack(server_socket); // Wait for acknowledgment
                break;
            case 3:
                // Exit the client
                send_exit_message(server_socket);
                printf("Exiting...\n");
                close(server_socket);
                return 0;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
}

