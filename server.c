#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/select.h>

int main()
{
    // 1. Create a Listening Socket (Non-Blocking)
    // create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket failed");
        return 1;
    }

    // Make socket non-blocking
    int flags = fcntl(server_fd, F_GETFL, 0);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);

    // Bind and listen
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(8080)
    };

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind failed");
        return 1;
    }

    if (listen(server_fd, 10) < 0)
    {
        perror("listen failed");
        return 1;
    }

    printf("Server listening on port 8080...\n");

    // 2. Initialize fd_set for select()
    fd_set read_fds;        // Set of FDs to monitor for reading
    int max_fd = server_fd; // Tracks the highest FD (required for `select()`)

    // Client FDs (we'll store them here)
    int client_fds[FD_SETSIZE];
    for (int i = 0; i < FD_SETSIZE; i++)
    {
        client_fds[i] = -1; // -1 means unused
    }

    // 3. Main event loop with select()
    while (1)
    {
        FD_ZERO(&read_fds);           // Clear the set
        FD_SET(server_fd, &read_fds); // Add server socket

        // Add all active clients to the set
        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (client_fds[i] != -1)
            {
                FD_SET(client_fds[i], &read_fds);
                if (client_fds[i] > max_fd)
                {
                    max_fd = client_fds[i];
                }
            }
        }

        // Wait for activity (blocks until something happens)
        int ready = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (ready < 0)
        {
            perror("select failed");
            return 1;
        }

        // 4. check for new connections
        if (FD_ISSET(server_fd, &read_fds))
        {
            int client_fd = accept(server_fd, NULL, NULL);
            if (client_fd < 0)
            {
                if (errno == EWOULDBLOCK || errno == EAGAIN)
                {
                    // No pending connections (non-blocking mode)
                }
                else
                {
                    perror("accept failed");
                }
            }
            else
            {
                // Make client non-blocking
                fcntl(client_fd, F_SETFL, O_NONBLOCK);

                // Store the new client FD
                for (int i = 0; i < FD_SETSIZE; i++)
                {
                    if (client_fds[i] == -1)
                    {
                        client_fds[i] = client_fd;
                        break;
                    }
                }

                printf("New client connected (fd=%d)\n", client_fd);
            }
        }

        // 5. Check for Client Data (recv())
        // Check all clients for incoming data
        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (client_fds[i] != -1 && FD_ISSET(client_fds[i], &read_fds))
            {
                char buffer[1024];
                int bytes = recv(client_fds[i], buffer, sizeof(buffer), 0);

                if (bytes <= 0)
                {
                    // Client disconnected
                    if (bytes == 0)
                    {
                        printf("Client (fd=%d) disconnected\n", client_fds[i]);
                    }
                    else if (errno != EWOULDBLOCK && errno != EAGAIN)
                    {
                        perror("recv failed");
                    }
                    close(client_fds[i]);
                    client_fds[i] = -1;
                }
                else
                {
                    // Process received data
                    printf("Received from client (fd=%d): %.*s\n",
                           client_fds[i], bytes, buffer);
                }
            }
        }
    }

    return 0;
}
