#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

int max_fd = 0, next_id = 0;
int ids[65536];
char *msgs[65536];
fd_set active_fds, read_fds, write_fds;
char buf_read[1001], buf_write[120000];

void fatal_error() {
    write(2, "Fatal error\n", 12);
    exit(1);
}

void notify_all(int sender, char *str) {
    for (int fd = 0; fd <= max_fd; ++fd) {
        if (FD_ISSET(fd, &write_fds) && fd != sender)
            send(fd, str, strlen(str), 0);
    }
}

int main(int ac, char **av) {
    if (ac != 2) {
        write(2, "Wrong number of arguments\n", 26);
        exit(1);
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) fatal_error();

    max_fd = server_fd;
    FD_ZERO(&active_fds);
    FD_SET(server_fd, &active_fds);

    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(2130706433);
    addr.sin_port = htons(atoi(av[1]));

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) fatal_error();
    if (listen(server_fd, 128) < 0) fatal_error();

    while (1) {
        read_fds = write_fds = active_fds;
        if (select(max_fd + 1, &read_fds, &write_fds, NULL, NULL) < 0) fatal_error();

        for (int fd = 0; fd <= max_fd; ++fd) {
            if (!FD_ISSET(fd, &read_fds)) continue;

            if (fd == server_fd) {
                int client_fd = accept(server_fd, NULL, NULL);
                if (client_fd < 0) continue;
                if (client_fd > max_fd) max_fd = client_fd;
                ids[client_fd] = next_id++;
                msgs[client_fd] = NULL;
                FD_SET(client_fd, &active_fds);
                sprintf(buf_write, "server: client %d just arrived\n", ids[client_fd]);
                notify_all(client_fd, buf_write);
                break;
            }

            int n = recv(fd, buf_read, 1000, 0);
            if (n <= 0) {
                sprintf(buf_write, "server: client %d just left\n", ids[fd]);
                notify_all(fd, buf_write);
                free(msgs[fd]);
                msgs[fd] = NULL;
                FD_CLR(fd, &active_fds);
                close(fd);
            } else {
                buf_read[n] = '\0';
                int old_len = msgs[fd] ? strlen(msgs[fd]) : 0;
                char *tmp = malloc(old_len + n + 1);
                if (!tmp) fatal_error();
                if (msgs[fd]) {
                    strcpy(tmp, msgs[fd]);
                    free(msgs[fd]);
                } else tmp[0] = '\0';
                strcat(tmp, buf_read);
                msgs[fd] = tmp;

                char *line = NULL;
                while (msgs[fd] && (line = strstr(msgs[fd], "\n"))) {
                    *line = '\0';
                    sprintf(buf_write, "client %d: %s\n", ids[fd], msgs[fd]);
                    notify_all(fd, buf_write);
                    char *rest = strdup(line + 1);
                    free(msgs[fd]);
                    msgs[fd] = rest;
                }
            }
        }
    }
    return 0;
}
