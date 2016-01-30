#include <string>
#include <iostream>
#include <functional>
#include <asio/io_service.hpp>
#include <asio/ip/address.hpp>

#include <redisasyncclient.h>

using namespace std::placeholders;

static const std::string redisKey = "unique-redis-key-example";
static const std::string redisValue = "unique-redis-value";

void handleConnected(asio::io_service &ioService, RedisAsyncClient &redis,
        bool ok, const std::string &errmsg)
{
    if( ok )
    {
        redis.command("SET", redisKey, redisValue, [&](const RedisValue &v) {
            std::cerr << "SET: " << v.toString() << std::endl;

            redis.command("GET", redisKey, [&](const RedisValue &v) {
                std::cerr << "GET: " << v.toString() << std::endl;

                redis.command("DEL", redisKey, [&](const RedisValue &) {
                    ioService.stop();
                });
            });
        });
    }
    else
    {
        std::cerr << "Can't connect to redis: " << errmsg << std::endl;
    }
}

int main(int, char **)
{
    asio::ip::address address = asio::ip::address::from_string("127.0.0.1");
    const unsigned short port = 6379;

    asio::io_service ioService;
    RedisAsyncClient redis(ioService);

    redis.asyncConnect(address, port,
            std::bind(&handleConnected, std::ref(ioService), std::ref(redis), _1, _2));

    ioService.run();

    return 0;
}
