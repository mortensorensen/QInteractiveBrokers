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

#endif
