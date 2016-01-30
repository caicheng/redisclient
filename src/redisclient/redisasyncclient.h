/*
 * Copyright (C) Alex Nekipelov (alex@nekipelov.net)
 * License: MIT
 */

#ifndef REDISASYNCCLIENT_REDISCLIENT_H
#define REDISASYNCCLIENT_REDISCLIENT_H

#include <asio/io_service.hpp>
#include <functional>
#include <memory>

#include <string>
#include <list>

#include "impl/redisclientimpl.h"
#include "redisvalue.h"
#include "redisbuffer.h"
#include "config.h"

class RedisClientImpl;

class RedisAsyncClient
{
	RedisAsyncClient(const RedisAsyncClient&) = delete;
public:
    // Subscribe handle.
    struct Handle {
        size_t id;
        std::string channel;
    };

    REDIS_CLIENT_DECL RedisAsyncClient(asio::io_service &ioService);
    REDIS_CLIENT_DECL ~RedisAsyncClient();

    // Connect to redis server
    REDIS_CLIENT_DECL void connect(
            const asio::ip::address &address,
            unsigned short port,
            const std::function<void(bool, const std::string &)> &handler);

    // Connect to redis server
    REDIS_CLIENT_DECL void connect(
            const asio::ip::tcp::endpoint &endpoint,
            const std::function<void(bool, const std::string &)> &handler);

    // backward compatibility
    inline void asyncConnect(
            const asio::ip::address &address,
            unsigned short port,
            const std::function<void(bool, const std::string &)> &handler)
    {
        connect(address, port, handler);
    }

    // backward compatibility
    inline void asyncConnect(
            const asio::ip::tcp::endpoint &endpoint,
            const std::function<void(bool, const std::string &)> &handler)
    {
        connect(endpoint, handler);
    }


    // Set custom error handler. 
    REDIS_CLIENT_DECL void installErrorHandler(
        const std::function<void(const std::string &)> &handler);

    // Execute command on Redis server.
    REDIS_CLIENT_DECL void command(
            const std::string &cmd,
            const std::function<void(const RedisValue &)> &handler = &dummyHandler);

    // Execute command on Redis server with one argument.
    REDIS_CLIENT_DECL void command(
            const std::string &cmd, const RedisBuffer &arg1,
            const std::function<void(const RedisValue &)> &handler = &dummyHandler);

    // Execute command on Redis server with two arguments.
    REDIS_CLIENT_DECL void command(
            const std::string &cmd, const RedisBuffer &arg1, const RedisBuffer &arg2,
            const std::function<void(const RedisValue &)> &handler = &dummyHandler);

    // Execute command on Redis server with three arguments.
    REDIS_CLIENT_DECL void command(
            const std::string &cmd, const RedisBuffer &arg1,
            const RedisBuffer &arg2, const RedisBuffer &arg3,
            const std::function<void(const RedisValue &)> &handler = &dummyHandler);

    // Execute command on Redis server with four arguments.
    REDIS_CLIENT_DECL void command(
            const std::string &cmd, const RedisBuffer &arg1, const RedisBuffer &arg2,
            const RedisBuffer &arg3, const RedisBuffer &arg4,
            const std::function<void(const RedisValue &)> &handler = &dummyHandler);

    // Execute command on Redis server with five arguments.
    REDIS_CLIENT_DECL void command(
            const std::string &cmd, const RedisBuffer &arg1,
            const RedisBuffer &arg2, const RedisBuffer &arg3,
            const RedisBuffer &arg4, const RedisBuffer &arg5,
            const std::function<void(const RedisValue &)> &handler = &dummyHandler);

    // Execute command on Redis server with six arguments.
    REDIS_CLIENT_DECL void command(
            const std::string &cmd, const RedisBuffer &arg1,
            const RedisBuffer &arg2, const RedisBuffer &arg3,
            const RedisBuffer &arg4, const RedisBuffer &arg5,
            const RedisBuffer &arg6,
            const std::function<void(const RedisValue &)> &handler = &dummyHandler);


    // Execute command on Redis server with seven arguments.
    REDIS_CLIENT_DECL void command(
            const std::string &cmd, const RedisBuffer &arg1,
            const RedisBuffer &arg2, const RedisBuffer &arg3,
            const RedisBuffer &arg4, const RedisBuffer &arg5,
            const RedisBuffer &arg6, const RedisBuffer &arg7,
            const std::function<void(const RedisValue &)> &handler = &dummyHandler);

    // Execute command on Redis server with the list of arguments.
    REDIS_CLIENT_DECL void command(
            const std::string &cmd, const std::list<RedisBuffer> &args,
            const std::function<void(const RedisValue &)> &handler = &dummyHandler);

    // Subscribe to channel. Handler msgHandler will be called
    // when someone publish message on channel. Call unsubscribe 
    // to stop the subscription.
    REDIS_CLIENT_DECL Handle subscribe(
            const std::string &channelName,
            const std::function<void(const std::vector<char> &msg)> &msgHandler,
            const std::function<void(const RedisValue &)> &handler = &dummyHandler);

    // Unsubscribe
    REDIS_CLIENT_DECL void unsubscribe(const Handle &handle);

    // Subscribe to channel. Handler msgHandler will be called
    // when someone publish message on channel; it will be 
    // unsubscribed after call.
    REDIS_CLIENT_DECL void singleShotSubscribe(
            const std::string &channel,
            const std::function<void(const std::vector<char> &msg)> &msgHandler,
            const std::function<void(const RedisValue &)> &handler = &dummyHandler);

    // Publish message on channel.
    REDIS_CLIENT_DECL void publish(
            const std::string &channel, const RedisBuffer &msg,
            const std::function<void(const RedisValue &)> &handler = &dummyHandler);

    REDIS_CLIENT_DECL static void dummyHandler(const RedisValue &) {}

protected:
    REDIS_CLIENT_DECL bool stateValid() const;

private:
    std::shared_ptr<RedisClientImpl> pimpl;
};

#ifdef REDIS_CLIENT_HEADER_ONLY
#include "impl/redisasyncclient.cpp"
#endif

#endif // REDISASYNCCLIENT_REDISCLIENT_H
