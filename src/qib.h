#ifndef qib_h
#define qib_h

#include "helpers.h"
#include <config.h>
#include "Contract.h"
#include "Order.h"

extern "C"
{
    K version(K x);
    K LoadLibrary(K x);
    K connect(K host, K port, K clientId);
    K disconnect(K x);
    K isConnected(K x);
    K reqCurrentTime(K x);
    K reqMktData(K conId, K contract, K genericTicks, K snapsnot);
    K reqAccountUpdates(K subscribe, K acctCode);
    K placeOrder(K id, K contract, K order);
    K cancelOrder(K id);
}

Z Contract createContract(K dict, std::string &error);
Z Order createOrder(K dict, std::string &error);

template<typename T> V setItem(T &property, G type, K x, I index, std::string &error);
template<typename T> V setProperty(T &property, I expectedType, K x, I index, std::string &error);
V setProperties(K dict, std::map<std::string, std::function<void(Contract&)>> &map, std::string &error);

template<typename F> V setProperties(K dict, F &props, std::string &error);

#endif
