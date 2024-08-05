# Implementing Concurrency in the Concurrent FTP UDP Server

The concurrent FTP UDP server is designed to handle multiple client connections simultaneously by using a thread pool and a channel-based communication system. Here’s a detailed explanation of how concurrency is implemented in the codebase for the client:

## Overview of Concurrency Implementation

The server uses a thread pool to manage `FTPFileWriter` instances, each of which owns a channel for processing file data. When a connection request is received from a client, a new `FTPFileWriter` is started in the thread pool, and the instance is saved in a map with the sender's address as the key. Subsequent data packets from the same client are directed to the corresponding `FTPFileWriter` for processing.

## Key Components and Workflow

### Connection Handling

1. **Connection Request**:
   - When the server receives a connection request from a client, it starts a new `FTPFileWriter` instance using the thread pool.
   - This `FTPFileWriter` instance owns a channel for processing incoming data.
   - The `FTPFileWriter` is saved in a map, with the sender's address as the key.

2. **Thread Pool**:
   - The thread pool manages the creation and execution of `FTPFileWriter` instances.
   - Each `FTPFileWriter` runs concurrently, allowing multiple file writes to be processed in parallel.

### Data Processing

1. **Send Request**:
   - When a new send request is received, the server processes the message at the UDP layer.
   - The relevant data buffer is extracted and pushed to the `FTPFileWriter`'s channel.

2. **Channel-Based Communication**:
   - The channel owned by each `FTPFileWriter` is used to receive data buffers.
   - This mechanism allows efficient distribution of data to the correct `FTPFileWriter` based on the sender's address.

3. **File Writing**:
   - Each `FTPFileWriter` processes its own channel's data buffers concurrently.
   - The data is written to the respective files, ensuring efficient and parallel file writing operations.

### Example Workflow

1. **Receiving Connection Request**:
   - Client sends a connection request to the server.
   - Server starts a new `FTPFileWriter` instance via the thread pool.
   - `FTPFileWriter` instance is saved in the map with the client’s address as the key.

2. **Receiving Send Request**:
   - Client sends a send request with data.
   - Server processes the UDP layer of the message.
   - Data buffer is directed to the appropriate `FTPFileWriter`'s channel based on the client’s address.

3. **Data Buffer Processing**:
   - `FTPFileWriter` retrieves data from its channel.
   - Data is written to the file concurrently with other `FTPFileWriter` instances.

### Building the project

```bash
make # to make
make clean # to clean
make remake # to clean and make again
```

There will be alot of compiler warning in tux, ignore them, none of them effect it

### Running the project

```bash
./bin/du-ftp -f anything -s # run in server mode, the filename doesnt matter becuase it is getting the filename from the client
./bin/du-ftp -f ./outfile/test.c -c # client mode send the certain file
# create 3 different terminal or run them in the background
# you can have multiple clients sending the messages
# it can cause segmentation fault sometimes on tux because it was not developed on tux,
# tux has different level of sensitivity for memory issue than my workstation so if it causes seg fault try again please
```