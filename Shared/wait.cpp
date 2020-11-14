#include "wait.h"
#include <sys/select.h>
int init_fd_set(const std::vector<int> fds_vect, int timeout_s, int timeout_ns, fd_set & fds_to_watch, timespec & tv)
{
    //Timeout values
    tv.tv_sec = timeout_s;
    tv.tv_nsec = timeout_ns;

    // fd_set passed into select
    //fd_set fds_to_watch;

    // Zero out the fds_to_watch
    FD_ZERO(&fds_to_watch);

    return (add_to_fd_set(fds_vect, fds_to_watch));
    
}

int add_to_fd_set(const std::vector<int> fds_vect, fd_set & fds_to_watch)
{
    int max_fd = 0;
    for (auto fd : fds_vect)
    {
        FD_SET(fd, &fds_to_watch);

        if (fd > max_fd)
        {
            max_fd = fd;
        }
    }

    return max_fd;
}

/*int wait_recv_or_send(std::vector<int> fds)
{
    int max_fd = 0;
    int select_ret;
    // timeout structure passed into select
    struct timeval tv;

    //Timeout after 5 seconds
    tv.tv_sec = 5;

    // fd_set passed into select
    fd_set fds_to_watch;

    // Zero out the fds_to_watch
    FD_ZERO(&fds_to_watch);

    //Add fds to fds_to_watch
    for (auto& fd : fds)
    {
        FD_SET(fd, &fds_to_watch);

        if (fd > max_fd)
        {
            max_fd = fd;
        }
    }


    //wait on fds in fds_to_watch
    do
    {
        select_ret = select(max_fd + 1, &fds_to_watch, NULL, NULL, &tv);
    } while (select_ret == 0);

    // return 0 if STDIN is not ready to be read.
    //return FD_ISSET(client_fd, &fds);
    return 1;
}*/