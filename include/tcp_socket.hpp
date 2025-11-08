//
// Created by chang on 11/8/25.
//

#ifndef SPEECH_D1_TCP_SOCKETS_HPP
#define SPEECH_D1_TCP_SOCKETS_HPP

#ifndef SIMPLE_TCP_SOCKET_HPP
#define SIMPLE_TCP_SOCKET_HPP

#include <string>
#include <cstdint>
#include <vector>

class TcpSocket {
public:
    // Constructors / Destructors & Move semantics
    TcpSocket();
    ~TcpSocket();
    TcpSocket(TcpSocket&& other) noexcept;
    TcpSocket& operator=(TcpSocket&& other) noexcept;

    TcpSocket(const TcpSocket&) = delete;
    TcpSocket& operator=(const TcpSocket&) = delete;

    // Client: connect to remote host
    // 'host' can be an IP address or hostname
    // Returns true if connection is successful
    bool connectTo(const std::string& host, uint16_t port, int timeoutMs = -1);

    // Server: bind and listen on a port
    bool bindAndListen(uint16_t port, int backlog = 128, bool reuseAddr = true);

    // Server: accept a connection, returning a new socket
    TcpSocket accept(std::string* peerIp = nullptr, uint16_t* peerPort = nullptr) const;

    // Send all data (blocking until all bytes are sent or error occurs)
    // Returns number of bytes sent; -1 if failed
    long long sendAll(const void* data, size_t len);

    // Convenience overload for string
    long long sendAll(const std::string& s) { return sendAll(s.data(), s.size()); }

    // Receive some data (up to maxBytes)
    // Returns number of bytes received; 0 means peer closed; -1 means error
    long long recvSome(void* buf, size_t maxBytes);

    // Receive exactly nBytes (or until peer closes / timeout / error)
    // Returns actual bytes read; -2 means timeout
    long long recvExact(void* buf, size_t nBytes, int timeoutMs = -1);

    // Socket options
    bool setBlocking(bool blocking);
    bool setNoDelay(bool nodelay);   // Disable Nagle’s algorithm
    bool setKeepAlive(bool enable);  // Enable TCP keepalive
    bool valid() const { return fd_ >= 0; }
    void close();

    // Address utilities
    std::string localAddress(uint16_t* portOut = nullptr) const;
    std::string peerAddress(uint16_t* portOut = nullptr) const;

    // Access underlying file descriptor
    int fd() const { return fd_; }

private:
    explicit TcpSocket(int fd);
    static bool resolve(const std::string& host, uint16_t port, std::vector<std::pair<std::string,int>>& out);

    int fd_{-1};
};

#endif // SIMPLE_TCP_SOCKET_HPP


#endif //SPEECH_D1_TCP_SOCKETS_HPP