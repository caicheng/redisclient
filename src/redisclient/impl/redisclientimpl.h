/*
 * Copyright (C) Alex Nekipelov (alex@nekipelov.net)
 * License: MIT
 */

#ifndef REDISCLIENT_REDISCLIENTIMPL_H
#define REDISCLIENT_REDISCLIENTIMPL_H

#include <functional>
#include <asio/ip/tcp.hpp>
#include <asio/strand.hpp>
#include <memory>

#include <string>
#include <vector>
#include <queue>
#include <map>
#include <array>

#include "../redisparser.h"
#include "../redisbuffer.h"
#include "../config.h"

class RedisClientImpl : public std::enable_shared_from_this<RedisClientImpl> {
public:
    REDIS_CLIENT_DECL RedisClientImpl(asio::io_service &ioService);
    REDIS_CLIENT_DECL ~RedisClientImpl();

    REDIS_CLIENT_DECL void handleAsyncConnect(
            const asio::error_code &ec,
            const std::function<void(bool, const std::string &)> &handler);

    REDIS_CLIENT_DECL void close();

    REDIS_CLIENT_DECL static std::vector<char> makeCommand(const std::vector<RedisBuffer> &items);

    REDIS_CLIENT_DECL RedisValue doSyncCommand(const std::vector<RedisBuffer> &buff);

    REDIS_CLIENT_DECL void doAsyncCommand(
            const std::vector<char> &buff,
            const std::function<void(const RedisValue &)> &handler);

    REDIS_CLIENT_DECL void sendNextCommand();
    REDIS_CLIENT_DECL void processMessage();
    REDIS_CLIENT_DECL void doProcessMessage(const RedisValue &v);
    REDIS_CLIENT_DECL void asyncWrite(const asio::error_code &ec, const size_t);
    REDIS_CLIENT_DECL void asyncRead(const asio::error_code &ec, const size_t);

    REDIS_CLIENT_DECL void onRedisError(const RedisValue &);
    REDIS_CLIENT_DECL void defaulErrorHandler(const std::string &s);
    REDIS_CLIENT_DECL static void ignoreErrorHandler(const std::string &s);

    REDIS_CLIENT_DECL static void append(std::vector<char> &vec, const RedisBuffer &buf);
    REDIS_CLIENT_DECL static void append(std::vector<char> &vec, const std::string &s);
    REDIS_CLIENT_DECL static void append(std::vector<char> &vec, const char *s);
    REDIS_CLIENT_DECL static void append(std::vector<char> &vec, char c);
    template<size_t size>
    static inline void append(std::vector<char> &vec, const char (&s)[size]);

    template<typename Handler>
    inline void post(const Handler &handler);

    enum {
        NotConnected,
        Connected,
        Subscribed,
        Closed 
    } state;

    asio::strand strand;
    asio::ip::tcp::socket socket;
    RedisParser redisParser;
    std::array<char, 4096> buf;
    size_t subscribeSeq;

    typedef std::pair<size_t, std::function<void(const std::vector<char> &buf)> > MsgHandlerType;
    typedef std::function<void(const std::vector<char> &buf)> SingleShotHandlerType;

    typedef std::multimap<std::string, MsgHandlerType> MsgHandlersMap;
    typedef std::multimap<std::string, SingleShotHandlerType> SingleShotHandlersMap;

    std::queue<std::function<void(const RedisValue &v)> > handlers;
    MsgHandlersMap msgHandlers;
    SingleShotHandlersMap singleShotMsgHandlers;

    struct QueueItem {
        std::function<void(const RedisValue &)> handler;
        std::shared_ptr<std::vector<char> > buff;
    };

    std::queue<QueueItem> queue;

    std::function<void(const std::string &)> errorHandler;
};

template<size_t size>
void RedisClientImpl::append(std::vector<char> &vec, const char (&s)[size])
{
    vec.insert(vec.end(), s, s + size);
}

template<typename Handler>
inline void RedisClientImpl::post(const Handler &handler)
{
    strand.post(handler);
}


#ifdef REDIS_CLIENT_HEADER_ONLY
#include "redisclientimpl.cpp"
#endif

#endif // REDISCLIENT_REDISCLIENTIMPL_H
