# Simple Client-Server Project

## Overview

This project implements a basic client-server communication system using C. The client and server programs communicate over a network using a predefined protocol to exchange messages.

## Structure

The project contains several key files that contribute to both the client and server sides of the application:

- `my-client.c`: Implements the client-side functionalities.
- `my-server.c`: Implements the server-side functionalities.
- `client-helper.c`, `client-helper.h`: Helper functions for the client.
- `server-helper.c`, `server-helper.h`: Helper functions for the server.
- `protocol.h`: Defines the communication protocol and message structure.
- `msg-list.c`, `msg-list.h`, `user-list.c`, `user-list.h`: Contains additional utility functions used by the server.

## Features

- **Socket Communication**: The client and server use sockets for network communication.
- **Protocol-based Message Handling**: Communication is structured based on a custom protocol defined in `protocol.h`.

## Compilation

1. **Compile the Server**:
   ```bash
   gcc -o server my-server.c server-helper.c user-list.c msg-list.c
   ```

2. **Compile the Client**:
   ```bash
   gcc -o client my-client.c client-helper.c msg-list.c user-list.c
   ```

## Usage

1. **Start the Server**:
   ```bash
   ./server <hostname> <port>
   ```

2. **Run the Client**:
   ```bash
   ./client <hostname> <port> server-helper.h
   ```

## Running the Remote Server at AWS

### Connect to the AWS VPN
Run the AWS VPN client and connect to the VPN 

### Test your connection by logging into the remote machine (a FreeBSD machine)

In a Terminal/PowerShell, enter the following. Replace the names with your real name. 
```
ssh yourfirstname-yourlastname@<hostname>
exit
```
### Secure-copying files to the remote machine

This step is to copy all the C code files for the app server to the remote FreeBSD machine.

In a Terminal/PowerShell, enter the following. Replace the names with your real name. 

```
scp client-server/server* yourfirstname-yourlastname@<hostname>:/usr/home/yourfirstname-yourlastname/.
```
### Compiling and starting the app server on the remote FreeBSD machine

In a Terminal/PowerShell, enter the following. Replace the names with your real name.

```
ssh yourfirstname-yourlastname@<hostname>
```
After logged into the FreeBSD machine, enter the following to compile and run the app server:
```
gcc -o server my-server.c server-helper.c user-list.c msg-list.c -lpthread
./server <hostname> <port>
```

### Run the app client on a local Ubuntu 

Start another Terminal/PowerShell on your local machine, enter:

```
docker compose run --rm --name remoteclient1 remoteclient
```

In the Ubuntu machine, enter the following to start the client:
```
gcc -o client my-client.c client-helper.c msg-list.c user-list.c
./client <hostname> <port> server-helper.h
```

### exit from the remote machine
Kill the app server by pressing CTRL + c, and then enter the following to exit the FreeBSD machine. 
```
exit
```