# psiw

## This is a project for the university.
- **Course**: System and concurrent programming
- **Topic**: Chat application using IPC mechanisms based on server/client structure

## Compiling:
1. Clone this repository.
```
git clone https://github.com/shym3q/psiw.git && cd psiw
```
2. Run the build script. The output binary 'chat' should be in the in the current directory.
```
./build.sh && cd bin
```

## Usage:
1. Launch the server.
```
./chat server
```
2. In a separate terminals or a 'tmux' windows launch as many clients as you wish.
```
./chat client
```

## TODO:
- [x] Connecting clients to the server.
- [x] Partial implementation of registering a topic.
- [x] Splitting messages into separate channels.
- [x] The client messing up a request does not block the server.
- [x] The server handling concurrent requests.
- [ ] GUI of the application defined.
- [x] A bus / a message broker implemented.
- [ ] Closing the client with the server being informed.
- [ ] Avoided using the global variables.
- [ ] Avoided forking with each request.
