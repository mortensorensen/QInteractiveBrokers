#ifndef HELPERS_h
#define HELPERS_h

#include <string>
#include <iostream>

#include <list>
#include <map>
#include <vector>
#include <ctime>
#include <cstring>
#include <cmath>

#include <memory>
#include <cstdio>

#include "k.h"


#define DBG(x, e) {if(x) O("%s\n", e);}
#define Q(x, e) if(x) { krr((S)e); R 0; }

inline F zu(I u) { return u / 8.64e4 - 10957; }  // kdb+ datetime from unix
inline I uz(F f) { return 86400 * (f + 10957); } // unix from kdb+ datetime
inline struct tm *gt(double kd) {
    time_t t = uz(kd);
    return gmtime(&t);
}

inline void tsms(unsigned ts, char *h, char *m, char *s, short *mmm) {
    *h = ts / 3600000;
    ts -= 3600000 * (*h);
    *m = ts / 60000;
    ts -= 60000 * (*m);
    *s = ts / 1000;
    ts -= 1000 * (*s);
    *mmm = ts;
}


template<typename ... Args>
std::string string_format(const std::string& format, Args ... args )
{
    size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

static std::string fmt_time(const char *str, time_t time, int adjusted)
{
    static char buffer[4096];
    
    struct tm *timeinfo = localtime(&time);
    if (adjusted) timeinfo->tm_hour -= 1;
    strftime(buffer, sizeof(buffer), str, timeinfo);
    
    return string_format("%s ", buffer);
}

template<class F, class V>
auto partial(F&&f, V&&v)
{
    return
    [f=std::forward<F>(f), v=std::forward<V>(v)](auto&&... args)
    {
        return f(v, decltype(args)(args)...);
    };
}

template<class F, class A0, class...Args>
auto partial(F&&f, A0&&a0, Args&&...args)
{
    return partial(partial(std::forward<F>(f), std::forward<A0>(a0)), std::forward<Args>(args)...);
}

#endif
