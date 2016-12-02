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

Z bool checkDictTypes(K dict, std::map<std::string, short> &propTypes, const char *&error);

Z Contract *createContract(K dict, std::string &error);
Z Order *createOrder(K dict, std::string &error);

V setList(V *property, K x);
V setItem(V *property, K x, I index);
V setProperty(V *property, K x, I index);
V setProperties(K dict, std::map<std::string, void*> props, std::string &error);


auto contractPropTypes = std::map<std::string, short> {
    { "conId",          -KJ },
    { "currency",       -KS },
    { "exchange",       -KS },
    { "expiry",         -KS },
    { "includeExpired", -KB },
    { "localSymbol",    -KS },
    { "multiplier",     -KS },
    { "primaryExchange", -KS },
    { "right",          -KS },
    { "secId",          -KS },
    { "secIdType",      -KS },
    { "secType",        -KS },
    { "strike",         -KS },
    { "symbol",         -KS },
    { "tradingClass",   -KS }
};

auto orderPropTypes = std::map<std::string, short> {
    // Order Identifiers
    { "clientId",           -KJ },
    { "orderId",            -KJ },
    { "permId",             -KJ },
    // Main Order Fields
    { "action",             -KS },
    { "auxPrice",           -KF },
    { "lmtPrice",           -KF },
    { "orderType",          -KS },
    { "totalQuantity",      -KJ },
    // Extended Order Fields
    { "allOrNone",          -KB },
    { "blockOrder",         -KB },
    { "displaySize",        -KI },
    { "goodAfterTime",      -KS },
    { "goodTillDate",       -KS },
    { "hidden",             -KB },
    { "minQty",             -KI },
    { "ocaType",            -KI },
    { "orderRef",           -KS },
    { "outsideRth",         -KB },
    { "overridePercentageConstraints", -KB },
    { "parentId",           -KJ },
    { "percentOffset",      -KF },
    { "rule80A",            -KS },
    { "tif",                -KS },
    { "sweepToFill",        -KB },
    { "trailingPercent",    -KS },
    { "trailStopPrice",     -KF },
    { "transmit",           -KB },
    { "triggerfunction",    -KI },
    { "activeStartTime",    -KS },
    { "activeStopTime",     -KS }
};

#endif
