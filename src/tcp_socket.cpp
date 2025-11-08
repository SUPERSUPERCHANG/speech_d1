//
// Created by chang on 11/8/25.
//

#include "tcp_socket.hpp"

#include <cstring>
#include <cerrno>
#include <chrono>
#include <vector>
#include <stdexcept>

#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>

namespace {
// Helper function to toggle blocking mode
inline int setFdBlocking(int fd, bool blocking) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    if (blocking) flags &= ~O_NONBLOCK;
    else          flags |=  O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags);
}
}

TcpSocket::TcpSocket() = default;

TcpSocket::~TcpSocket() {
    close();
}

TcpSocket::TcpSocket(int fd) : fd_(fd) {}

TcpSocket::TcpSocket(TcpSocket&& other) noexcept {
    fd_ = other.fd_;
    other.fd_ = -1;
}

TcpSocket& TcpSocket::operator=(TcpSocket&& other) noexcept {
    if (this != &other) {
        close();
        fd_ = other.fd_;
        other.fd_ = -1;
    }
    return *this;
}

void TcpSocket::close() {
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

bool TcpSocket::setBlocking(bool blocking) {
    if (fd_ < 0) return false;
    return setFdBlocking(fd_, blocking) == 0;
}

bool TcpSocket::setNoDelay(bool nodelay) {
    if (fd_ < 0) return false;
    int flag = nodelay ? 1 : 0;
    return setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag)) == 0;
}

bool TcpSocket::setKeepAlive(bool enable) {
    if (fd_ < 0) return false;
    int flag = enable ? 1 : 0;
    return setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &flag, sizeof(flag)) == 0;
}

bool TcpSocket::resolve(const std::string& host, uint16_t port,
                        std::vector<std::pair<std::string,int>>& out) {
    struct addrinfo hints{};
    hints.ai_family   = AF_UNSPEC;   // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo* res = nullptr;
    std::string portStr = std::to_string(port);
    int rc = getaddrinfo(host.c_str(), portStr.c_str(), &hints, &res);
    if (rc != 0) return false;

    for (auto* p = res; p != nullptr; p = p->ai_next) {
        char hostBuf[NI_MAXHOST] = {0};
        if (getnameinfo(p->ai_addr, p->ai_addrlen, hostBuf, sizeof(hostBuf),
                        nullptr, 0, NI_NUMERICHOST) == 0) {
            out.emplace_back(std::string(hostBuf), p->ai_family);
        }
    }
    freeaddrinfo(res);
    return !out.empty();
}

bool TcpSocket::connectTo(const std::string& host, uint16_t port, int timeoutMs) {
    close();

    // Resolve hostname into multiple candidate IPs
    std::vector<std::pair<std::string,int>> candidates;
    if (!resolve(host, port, candidates)) return false;

    for (auto& [ipStr, family] : candidates) {
        int fd = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
        if (fd < 0) continue;

        bool needWait = false;
        if (timeoutMs >= 0) {
            if (setFdBlocking(fd, false) != 0) { ::close(fd); continue; }
            needWait = true;
        }

        int rc = -1;
        if (family == AF_INET) {
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_port   = htons(port);
            ::inet_pton(AF_INET, ipStr.c_str(), &addr.sin_addr);
            rc = ::connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        } else if (family == AF_INET6) {
            sockaddr_in6 addr6{};
            addr6.sin6_family = AF_INET6;
            addr6.sin6_port   = htons(port);
            ::inet_pton(AF_INET6, ipStr.c_str(), &addr6.sin6_addr);
            rc = ::connect(fd, reinterpret_cast<sockaddr*>(&addr6), sizeof(addr6));
        }

        // Connected immediately
        if (rc == 0) {
            fd_ = fd;
            if (timeoutMs >= 0) setFdBlocking(fd_, true);
            return true;
        }

        // Wait for connect() to finish (EINPROGRESS)
        if (rc < 0 && errno == EINPROGRESS && needWait) {
            fd_set wfds; FD_ZERO(&wfds); FD_SET(fd, &wfds);
            timeval tv{};
            tv.tv_sec  = timeoutMs / 1000;
            tv.tv_usec = (timeoutMs % 1000) * 1000;

            int sel = ::select(fd + 1, nullptr, &wfds, nullptr, &tv);
            if (sel > 0) {
                int err = 0; socklen_t len = sizeof(err);
                if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len) == 0 && err == 0) {
                    fd_ = fd;
                    setFdBlocking(fd_, true);
                    return true;
                }
            }
        }
        ::close(fd);
    }
    return false;
}

bool TcpSocket::bindAndListen(uint16_t port, int backlog, bool reuseAddr) {
    close();

    int fd = ::socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (fd >= 0) {
        int v6only = 0; // Allow both IPv4 and IPv6
        setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &v6only, sizeof(v6only));
    } else {
        fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (fd < 0) return false;
    }

    if (reuseAddr) {
        int yes = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    }

    int rc = -1;
    if (fd >= 0) {
        sockaddr_in6 addr6{};
        addr6.sin6_family = AF_INET6;
        addr6.sin6_addr   = in6addr_any;
        addr6.sin6_port   = htons(port);
        rc = ::bind(fd, reinterpret_cast<sockaddr*>(&addr6), sizeof(addr6));
    }
    if (rc != 0) { ::close(fd); return false; }

    if (::listen(fd, backlog) != 0) { ::close(fd); return false; }

    fd_ = fd;
    return true;
}

TcpSocket TcpSocket::accept(std::string* peerIp, uint16_t* peerPort) const {
    if (fd_ < 0) return TcpSocket();

    sockaddr_storage ss{};
    socklen_t slen = sizeof(ss);
    int cfd = ::accept(fd_, reinterpret_cast<sockaddr*>(&ss), &slen);
    if (cfd < 0) return TcpSocket();

    if (peerIp || peerPort) {
        char ipbuf[NI_MAXHOST]{};
        char servbuf[NI_MAXSERV]{};
        if (getnameinfo(reinterpret_cast<sockaddr*>(&ss), slen,
                        ipbuf, sizeof(ipbuf), servbuf, sizeof(servbuf),
                        NI_NUMERICHOST | NI_NUMERICSERV) == 0) {
            if (peerIp)   *peerIp = ipbuf;
            if (peerPort) *peerPort = static_cast<uint16_t>(std::stoi(servbuf));
        }
    }
    return TcpSocket(cfd);
}

long long TcpSocket::sendAll(const void* data, size_t len) {
    if (fd_ < 0) return -1;
    const char* p = static_cast<const char*>(data);
    size_t left = len;
    while (left > 0) {
        ssize_t n = ::send(fd_, p, left, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        p += n;
        left -= static_cast<size_t>(n);
    }
    return static_cast<long long>(len);
}

long long TcpSocket::recvSome(void* buf, size_t maxBytes) {
    if (fd_ < 0) return -1;
    ssize_t n = ::recv(fd_, buf, maxBytes, 0);
    if (n < 0) {
        if (errno == EINTR) return 0;
        return -1;
    }
    return static_cast<long long>(n);
}

long long TcpSocket::recvExact(void* buf, size_t nBytes, int timeoutMs) {
    if (fd_ < 0) return -1;
    char* p = static_cast<char*>(buf);
    size_t got = 0;

    while (got < nBytes) {
        if (timeoutMs >= 0) {
            fd_set rfds; FD_ZERO(&rfds); FD_SET(fd_, &rfds);
            timeval tv{};
            tv.tv_sec  = timeoutMs / 1000;
            tv.tv_usec = (timeoutMs % 1000) * 1000;
            int sel = ::select(fd_ + 1, &rfds, nullptr, nullptr, &tv);
            if (sel == 0) return -2; // timeout
            if (sel < 0) {
                if (errno == EINTR) continue;
                return -1;
            }
        }

        ssize_t n = ::recv(fd_, p + got, nBytes - got, 0);
        if (n == 0) break;           // peer closed
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        got += static_cast<size_t>(n);
    }
    return static_cast<long long>(got);
}

std::string TcpSocket::localAddress(uint16_t* portOut) const {
    if (fd_ < 0) return {};
    sockaddr_storage ss{}; socklen_t slen = sizeof(ss);
    if (getsockname(fd_, reinterpret_cast<sockaddr*>(&ss), &slen) != 0) return {};
    char host[NI_MAXHOST]{}, serv[NI_MAXSERV]{};
    if (getnameinfo(reinterpret_cast<sockaddr*>(&ss), slen, host, sizeof(host),
                    serv, sizeof(serv), NI_NUMERICHOST | NI_NUMERICSERV) != 0) return {};
    if (portOut) *portOut = static_cast<uint16_t>(std::stoi(serv));
    return host;
}

std::string TcpSocket::peerAddress(uint16_t* portOut) const {
    if (fd_ < 0) return {};
    sockaddr_storage ss{}; socklen_t slen = sizeof(ss);
    if (getpeername(fd_, reinterpret_cast<sockaddr*>(&ss), &slen) != 0) return {};
    char host[NI_MAXHOST]{}, serv[NI_MAXSERV]{};
    if (getnameinfo(reinterpret_cast<sockaddr*>(&ss), slen, host, sizeof(host),
                    serv, sizeof(serv), NI_NUMERICHOST | NI_NUMERICSERV) != 0) return {};
    if (portOut) *portOut = static_cast<uint16_t>(std::stoi(serv));
    return host;
}
