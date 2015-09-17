1. create and close a socket

        int socket(int domain, int type, int protocal);
        // domain: AF_INET, AF_INET6, AF_UNIX, AF_USPEC
        // type: SOCK_DGRAM(UDP), SOCK_RAW, SOCK_SEQPACKET, SOCK_STREAM(TCP)
        // protocol: normally 0

        int close(int fd); // socket is also a file, so close can be used to close a socket fd
        int shutdown(int sockfd, int how);
        // how: SHUT_RD, SHUT_WR, SHUT_RDWR.
        // shutdown is sometime superior than close, because `shutdown` can set a socket inactive whenever the socket has
        // multiple duplicated file descriptors or not. And what's more `shutdown` can only close the read or write function,
        // this is quite convenient in some situation

2. addressing  
    1. struct sockaddr: To let address parameter with different format can be used in socket functions. All address are converted
    into a universal address format **struct sockaddr**.

            // The Linux implementation
            struct sockaddr {
                sa_family_t sa_family;
                char        sa_data[14];
            };

            // Linux ipv4 address format
            struct sockaddr_in {
                sa_family_t sin_family;
                in_port_t   sin_port;   // 16-bit, defined by uint16_t, so 0 - 65535, and 0 - 1024 is reserved for privileged process
                struct in_addr  sin_addr;
                unsigned char   sin_zero[8];
            };

            struct in_addr {
                in_addr_t s_addr;   // 32-bit, defined by uint32_t
            };
3. socket communication

        // bind a address to the socket, when client calls connect or sever calls listen with the socket without bind a address,
        // system will select a address to bind to the socket.
        // If port in addr is 0, then system will dynamically allocate a port and when bind success you can use `getsockname` to
        // get the allocated port number
        int bind(int sockfd, const struct sockaddr *addr, socklen_t len);
        // This function is mostly used for connection-oriented socket whose type is SOCK_STREAM or SOCK_SEQPACKET,
        // but SOCK_DGRAM can also use this for convenient.
        int connect(int sockfd, const struct sockaddr *addr, socklen_t len);
        // the sever calls listen to claim that ready to accept client's connection. We are using connection, that means SOCK_DGRAM
        // doesn't need these functions.
        int listen(int sockfd, int backlog);
        // the server calls accept to accept a client's connection request. This function will block, if no request come in.
        int accept(int sockfd, struct sockaddr *addr, socklen_t *len);

4. data transmit

        // for connection-oriented socket
        ssize_t send(int sockfd, const void *buf, size_t nbytes, int flags);
        ssize_t recv(int sockfd, const void *buf, size_t nbytes, int flags);

        // for non connection socket, but can also used for connection-oriented socket.
        ssize_t sendto(int sockfd, const void *buf, size_t nbytes, int flags,
            struct sockaddr *destaddr, addrlen_t destaddrlen);
        ssize_t recvfrom(int sockfd, const void *buf, size_t nbytes, int flags,
            struct sockaddr *destaddr, addrlen_t *destaddrlen);

5. when using exec-series function. The process ID is not changed, and the impaction is to use the exec function specified program's
data, code, stack.. to replace the original ones. One point should note that all exec family functions are internally called execve
system call. The file descriptors remain open across execs if the file descriptors' FD_CLOEXEC flag are not set. This is merit
quite useful. For example:

        char *command = "/bin/ls";
        int pipefd[2];
        if(pipe(pipefd) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        int cpid = fork();
        if(cpid == 0) {  // child process
            close(pipefd[0]); // close read end
            if(dup2(pipefd[1], STDOUT_FILENO) == -1) {  // duplicate pipefd[1] to stdout
                perror("pipe");
                eixt(EXIT_FAILURE);
            }
            execl(command, "ls", "-al", (char *)NULL);
        }
        else {  // parent process
            close(pipedfd[1]); // close write end
            // here we can read data from pipe[0]
        }
