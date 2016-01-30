/*
 * Copyright (C) Alex Nekipelov (alex@nekipelov.net)
 * License: MIT
 */

#ifndef REDISCLIENT_REDISVALUE_H
#define REDISCLIENT_REDISVALUE_H

#include <string>
#include <vector>
#include <variant.hpp>
#include "config.h"


class RedisValue {
public:
    struct ErrorTag {};

    REDIS_CLIENT_DECL RedisValue();
    REDIS_CLIENT_DECL RedisValue(int i);
    REDIS_CLIENT_DECL RedisValue(const char *s);
    REDIS_CLIENT_DECL RedisValue(const std::string &s);
    REDIS_CLIENT_DECL RedisValue(const std::vector<char> &buf);
    REDIS_CLIENT_DECL RedisValue(const std::vector<char> &buf, struct ErrorTag &);
    REDIS_CLIENT_DECL RedisValue(const std::vector<RedisValue> &array);

    // Return the value as a std::string if 
    // type is a byte string; otherwise returns an empty std::string.
    REDIS_CLIENT_DECL std::string toString() const;

    // Return the value as a std::vector<char> if 
    // type is a byte string; otherwise returns an empty std::vector<char>.
    REDIS_CLIENT_DECL std::vector<char> toByteArray() const;
    
    // Return the value as a std::vector<RedisValue> if 
    // type is an int; otherwise returns 0.
    REDIS_CLIENT_DECL int toInt() const;
    
    // Return the value as an array if type is an array;
    // otherwise returns an empty array.
    REDIS_CLIENT_DECL std::vector<RedisValue> toArray() const;

    // Return the string representation of the value. Use
    // for dump content of the value.
    REDIS_CLIENT_DECL std::string inspect() const;

    // Return true if value not a error
    REDIS_CLIENT_DECL bool isOk() const;
    // Return true if value is a error
    REDIS_CLIENT_DECL bool isError() const;


    // Return true if this is a null.
    REDIS_CLIENT_DECL bool isNull() const;
    // Return true if type is an int
    REDIS_CLIENT_DECL bool isInt() const;
    // Return true if type is an array
    REDIS_CLIENT_DECL bool isArray() const;
    // Return true if type is a string/byte array. Alias for isString();
    REDIS_CLIENT_DECL bool isByteArray() const;
    // Return true if type is a string/byte array. Alias for isByteArray().
    REDIS_CLIENT_DECL bool isString() const;

    REDIS_CLIENT_DECL bool operator == (const RedisValue &rhs) const;
    REDIS_CLIENT_DECL bool operator != (const RedisValue &rhs) const;

protected:
    template<typename T>
    REDIS_CLIENT_DECL T castTo() const;

    template<typename T>
    REDIS_CLIENT_DECL bool typeEq() const;

private:
    struct NullTag {
        inline bool operator == (const NullTag &) const {
            return true;
        }
    };

	variant<NullTag, int, std::vector<char>, std::vector<RedisValue> > value;
    bool error;
};


template<typename T>
T RedisValue::castTo() const
{
	if (value.is<T>())
		return value.get<T>();
	return T();
}

template<typename T>
bool RedisValue::typeEq() const
{
	return value.is<T>();
}

#ifdef REDIS_CLIENT_HEADER_ONLY
#include "impl/redisvalue.cpp"
#endif

#endif // REDISCLIENT_REDISVALUE_H
