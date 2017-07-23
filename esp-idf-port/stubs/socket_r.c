#include <lwip/sockets.h>

int lwip_accept_r(int s, struct sockaddr *addr, socklen_t *addrlen)
{
    return lwip_accept(s, addr, addrlen);
}

int lwip_bind_r(int s, const struct sockaddr *name, socklen_t namelen)
{
    return lwip_bind(s, name, namelen);
}

int lwip_shutdown_r(int s, int how)
{
    return lwip_shutdown(s, how);
}

int lwip_getpeername_r (int s, struct sockaddr *name, socklen_t *namelen)
{
    return lwip_getpeername(s, name, namelen);
}

int lwip_getsockname_r (int s, struct sockaddr *name, socklen_t *namelen)
{
    return lwip_getsockname(s, name, namelen);
}

int lwip_getsockopt_r (int s, int level, int optname, void *optval, socklen_t *optlen)
{
    return lwip_getsockopt(s, level, optname, optval, optlen);
}

int lwip_setsockopt_r (int s, int level, int optname, const void *optval, socklen_t optlen)
{
    return lwip_setsockopt(s, level, optname, optval, optlen);
}

int lwip_close_r(int s)
{
    return lwip_close(s);
}

int lwip_connect_r(int s, const struct sockaddr *name, socklen_t namelen)
{
    return lwip_connect(s, name, namelen);
}

int lwip_listen_r(int s, int backlog)
{
    return lwip_listen(s, backlog);
}

int lwip_recv_r(int s, void *mem, size_t len, int flags)
{
    return lwip_recv(s, mem, len, flags);
}

int lwip_read_r(int s, void *mem, size_t len)
{
    return lwip_read(s, mem, len);
}

int lwip_recvfrom_r(int s, void *mem, size_t len, int flags,
      struct sockaddr *from, socklen_t *fromlen)
{
    return lwip_recvfrom(s, mem, len, flags, from, fromlen);
}

int lwip_send_r(int s, const void *dataptr, size_t size, int flags)
{
    return lwip_send(s, dataptr, size, flags);
}

int lwip_sendmsg_r(int s, const struct msghdr *message, int flags)
{
    return lwip_sendmsg(s, message, flags);
}

int lwip_sendto_r(int s, const void *dataptr, size_t size, int flags,
    const struct sockaddr *to, socklen_t tolen)
{
    return lwip_sendto(s, dataptr, size, flags, to, tolen);
}

int lwip_write_r(int s, const void *dataptr, size_t size)
{
    return lwip_write(s, dataptr, size);
}

int lwip_writev_r(int s, const struct iovec *iov, int iovcnt)
{
    return lwip_writev(s, iov, iovcnt);
}

int lwip_ioctl_r(int s, long cmd, void *argp)
{
    return lwip_ioctl(s, cmd, argp);
}

int lwip_fcntl_r(int s, int cmd, int val)
{
    return lwip_fcntl(s, cmd, val);
}
