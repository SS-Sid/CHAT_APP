
# Client Server Based Instant Messenger


The aim was to implement a Client Server Chat Based system, with added support for File transfer.


##  The approach to the Problem

We approached the problem by starting with the basic knowledge required for the making of the server and the client using TCP protocols.

There are 2 servers online at a given time,

*One for Chat purposes and,

*Second for the purpose of sharing the files

**NOTE : The Server is UNIX based whereas the client is Cross Platform**

## The various files used in the program

### The files on the server side are:

1. connection.hpp
2. connection.cpp
3. linked_list.hpp
4. linked_list.cpp
5. server.hpp
6. server.cpp
7. main.cpp
8. Folder named 'database', to store the shared files.

The connection files are the data structure files for storing the username, socket address and the file descriptors for the clients joining the server.

The linked list files are the linked list data structure implementations to manage the list of all the connections currently present in the server. (They help in sending the messages and managing the client list.)

The server.hpp contains the server class. The server is initiated by specifying the port number and a flag specifying if the server is used for file transfer, or for chat. Different flag gives access to different permissions to the server object initiated. Each server works on Multithreading approach to handle multiple client requests independently.

The file main.cpp initiates the server and calls the management functions while the server is online.
 
### The files used on the client side are:

1. client_kiwy_1.3.1_alpha.py
2. messenger.kv
3.  Folder named 'database', to store the shared files.

The file client_kiwy contains the major portion of the client code, ranging from making of the client and connecting it to the server, to the generation of the GUI for the same.

It also contains the classes to support the file transfer from/to the client to/from the server.

The file messenger.kv contains the layout of the GUI application built for the chat purposes.


## Deployment

To deploy this project run the following command in the terminal.

### To set the Server online

Open the terminal in the same directory as the server files.

```bash
  g++ -o <chat_server_name> main.cpp server.cpp linked_list.cpp connection.cpp
``` 

```bash
./<chat_server_name> <chat_port_number> 0
```

```bash
./<chat_server_name> <file_transfer_port_number> 1
```

### To set the client side 

Open the terminal in the same directory as client file

First, we need to install kivy, you can use the following command for the same:

```bash
pip install kivy
```
You need to open the **client_kivy_1.3.1_alpha.py** in the **IDE/Text Editor** and go to line 11, 12, 13. Enter the IP ADDRESS of the server, chat_port_number and the file_transfer_port_number and save the file.

Then you need to run the following command in the terminal.

```bash
python -u client_kivy_1.3.1_alpha.py
```
