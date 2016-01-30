#include <string>
#include <iostream>
#include <asio/ip/address.hpp>
#include <functional>

#include <redisasyncclient.h>

using namespace std::placeholders;

static const std::string redisKey = "unique-redis-key-example";
static const std::string redisValue = "unique-redis-value";

class Worker
{
public:
    Worker(asio::io_service &ioService, RedisAsyncClient &redisClient)
        : ioService(ioService), redisClient(redisClient)
    {}

    void onConnect(bool connected, const std::string &errorMessage);
    void onSet(const RedisValue &value);
    void onGet(const RedisValue &value);
    void stop();

private:
    asio::io_service &ioService;
    RedisAsyncClient &redisClient;
};

void Worker::onConnect(bool connected, const std::string &errorMessage)
{
    if( !connected )
    {
        std::cerr << "Can't connect to redis: " << errorMessage;
    }
    else
    {
        redisClient.command("SET",  redisKey, redisValue,
                            std::bind(&Worker::onSet, this, _1));
    }
}

void Worker::onSet(const RedisValue &value)
{
    std::cerr << "SET: " << value.toString() << std::endl;
    if( value.toString() == "OK" )
    {
        redisClient.command("GET",  redisKey,
                            std::bind(&Worker::onGet, this, _1));
    }
    else
    {
        std::cerr << "Invalid value from redis: " << value.toString() << std::endl;
    }
}

void Worker::onGet(const RedisValue &value)
{
    std::cerr << "GET " << value.toString() << std::endl;
    if( value.toString() != redisValue )
    {
        std::cerr << "Invalid value from redis: " << value.toString() << std::endl;
    }

    redisClient.command("DEL", redisKey,
                        std::bind(&asio::io_service::stop, std::ref(ioService)));
}


int main(int, char **)
{
    const char *address = "127.0.0.1";
    const int port = 6379;

    asio::io_service ioService;
    RedisAsyncClient client(ioService);
    Worker worker(ioService, client);
    asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string(address), port);

    client.asyncConnect(endpoint, std::bind(&Worker::onConnect, &worker, _1, _2));

    ioService.run();

    return 0;
}
