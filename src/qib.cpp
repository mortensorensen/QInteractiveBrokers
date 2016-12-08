#include "qib.h"
#include "config.h"
#include "IBClient.h"
#include <sstream>
#include <functional>

#define streq(x, y) strcmp(x, y) == 0

IBClient *ib;

__attribute__((constructor))
ZV initialize_api() {
    ib = new IBClient();
}

__attribute__((destructor))
ZV destroy_api() {
    if (ib) {
        ib->disconnect();
        delete ib;
    }
}

K version(K x)
{
    K keys = ktn(KS, 4);
    K vals = ktn(KS, 4);

    kS(keys)[0] = ss((S) "release");
    kS(keys)[1] = ss((S) "os");
    kS(keys)[2] = ss((S) "tws");
    kS(keys)[3] = ss((S) "kx");

    kS(vals)[0] = ss((S) BUILD_PROJECT_VERSION);
    kS(vals)[1] = ss((S) BUILD_OPERATING_SYSTEM);
    kS(vals)[2] = ss((S) BUILD_TWS_VER);
    kS(vals)[3] = ss((S) BUILD_KX_VER);

    R xD(keys, vals);
}

K LoadLibrary(K x)
{
    O("\n");
    O("========= %s =========\n",       PROGRAM_NAME);
    O("release » %-5s\n",               BUILD_PROJECT_VERSION);
    O("os » %-5s\n",                    BUILD_OPERATING_SYSTEM);
    O("arch » %-5s\n",                  BUILD_PROCESSOR);
    O("git commit » %-5s\n",            BUILD_GIT_SHA1);
    O("git datetime » %-5s\n",          BUILD_GIT_DATE);
    O("kdb compatibility » %s.x\n",     BUILD_KX_VER);
    O("compiler flags »%-5s\n",         BUILD_COMPILER_FLAGS);
    O("\n");

    auto map = std::map<const char*, std::pair<void*, unsigned short> > {
        { "version",        { (void*)version,           1 } },
        { "connect",        { (void*)connect,           3 } },
        { "disconnect",     { (void*)disconnect,        1 } },
        { "isConnected",    { (void*)isConnected,       1 } },
        { "reqCurrentTime", { (void*)reqCurrentTime,    1 } },
        { "reqMktData",     { (void*)reqMktData,        4 } },
        { "reqAccountUpdates", { (void*)reqAccountUpdates, 2 } },
        { "placeOrder",     { (void*)placeOrder,        3 } },
        { "cancelOrder",    { (void*)cancelOrder,       1 } }
    };

    K keys = ktn(KS, 0);
    K vals = knk(0);
    for (auto it = map.begin(); it != map.end(); ++it) {
        js(&keys, ss((S)it->first));
        jk(&vals, dl(it->second.first, it->second.second));
    }

    R xD(keys, vals);
}

K eventLoop(I fd)
{
    ib->onReceive();
    R NULL;
}

///////////////
// IB TWS
///////////////

K connect(K host, K port, K clientId)
{
    Q(host->t != -KS || port->t != -KJ || clientId->t != -KJ, "type");
    
    if (!ib->isConnected()) {
        ib->connect(host->s, port->j, clientId->j);
        if (ib->isConnected())
            sd1(ib->fd(), eventLoop);
    }

    R kb(ib->isConnected());
}

K disconnect(K x)
{
    ib->disconnect();
    R NULL;
}

K isConnected(K x)
{
    R kb(ib->isConnected());
}

K reqCurrentTime(K x)
{
    ib->reqCurrentTime();
    R NULL;
}

K reqMktData(K tickerId, K contract, K genericTicks, K snapsnot)
{
    Q(tickerId->t != -KJ || contract->t != XD || genericTicks->t != KC || snapsnot->t != -KB, "type");
    Q(!ib->isConnected(), "connection");
    
    std::string error;
    auto c = createContract(contract, error);
    Q(!error.empty(), error.c_str());
    
//    ib->reqMktData(tickerId->j, *c, "", snapsnot->i == 1);
    
    R NULL;
}

K reqAccountUpdates(K subscribe, K acctCode)
{
    Q(subscribe->t != -KB || acctCode->t != -KS, "type");
    Q(!ib->isConnected(), "connection");
    ib->reqAccountUpdates(subscribe->i == 1, acctCode->s);
    R NULL;
}

K placeOrder(K id, K contract, K order)
{
    Q(id->t != -KJ || contract->t != XD || order->t != XD, "type");
    Q(!ib->isConnected(), "connection");
    
    std::string error;
    auto c = createContract(contract, error);
    Q(!error.empty(), error.c_str());
    
    auto o = createOrder(order, error);
    Q(!error.empty(), error.c_str());
    
    ib->placeOrder(id->j, c, o);
    R NULL;
}

K cancelOrder(K id)
{
    Q(id->t != -KJ, "type");
    Q(!ib->isConnected(), "connection");
    ib->cancelOrder(id->j);
    R NULL;
}


////////////////////
// Static methods
////////////////////

template<typename T> V setAtom(T *property, K x, G type, std::string &error)
{
    // Base case. Should never be called.
    // TODO: Test and warn
}

template<> V setAtom(bool *property, K x, G type, std::string &error)
{
    SW(xt) {
        CS(-1, *property = x->g ==1)
        CD: R;
    }
}

template<> V setAtom(int *property, K x, G type, std::string &error)
{
    SW(xt) {
        CS(-KI, *property = x->i);
        CD: R;
    }
}

template<> V setAtom(long *property, K x, G type, std::string &error)
{
    SW(xt) {
        CS(-KJ, *property = x->j);
        CD: R;
    }
}

template<> V setAtom(double *property, K x, G type, std::string &error)
{
    SW(xt) {
        CS(-KF, *property = x->f);
        CD: R;
    }
}

template<> V setAtom(IBString *property, K x, G type, std::string &error)
{
    SW(xt) {
        CS(-KS, *property = x->s);   // symbol
        CS(-KM, *property = string_format("%04d%02d", (x->i)/12+2000, (x->i)%12+1));    // month
        CS(-KD, *property = fmt_time("%Y%m%d", ((x->i) + 10957)*8.64e4, 0));    // date
        CS(-KZ, *property = fmt_time("%Y%m%dD %H:%M:%S", ((x->f) + 10957)*8.64e4, 0));    // datetime
        CD: R;
    }
}

template<typename T>
V setItem(T &property, I type, K x, I index, std::string &error)
{
    SW(xt) {
        CS( 0, setProperty(property, type, kK(x)[index], index, error))
        CS( 1, setAtom(property, kb(kG(x)[index]), type, error))
        CS( 4, setAtom(property, kg(kG(x)[index]), type, error))
        CS( 5, setAtom(property, kh(kH(x)[index]), type, error))
        CS( 6, setAtom(property, ki(kI(x)[index]), type, error))
        CS( 7, setAtom(property, kj(kJ(x)[index]), type, error))
        CS( 8, setAtom(property, ke(kE(x)[index]), type, error))
        CS( 9, setAtom(property, kf(kF(x)[index]), type, error))
        CS(10, setAtom(property, kc(kC(x)[index]), type, error))
        CS(11, setAtom(property, ks(kS(x)[index]), type, error))
        CS(14, setAtom(property, kd(kI(x)[index]), type, error))
        CS(15, setAtom(property, kz(kF(x)[index]), type, error))
        CD: R;
    }
}

template<typename T>
V setProperty(T &property, I expectedType, K x, I index, std::string &error)
{
    I xtype = (0 <= xt) && (xt < 20) ? kK(x)[index]->t : xt;
    if (xtype != expectedType) {
        error = string_format("Invalid type: %i. Expected %i", xtype, expectedType);
        return;
    }
    
    if (xt < 0) setAtom(property, x, expectedType, error);
    else if ((0 <= xt) && (xt < 20)) setItem(property, expectedType, x, index, error);
    else return;
}

V setProperties(K dict, std::map<std::string, std::function<void(K x, I i, std::string &err)>> &props, std::string &error)
{
    K keys = kK(dict)[0];
    K vals = kK(dict)[1];
    
    if (keys->t != KS) {
        error = "keys must be syms";
        return;
    }
    
    const char *key;
    
    for (I i = 0; i < keys->n; i++) {
        key = kS(keys)[i];
        
        auto it = props.find(key);
        if (it != props.end()) {
            (it->second)(vals, i, error);
            if (!error.empty()) return;
        } else {
            // TODO: error
        }
    }
}

auto f = [](auto property, I expectedType, K x, I index, std::string &error) {
    return setProperty(property, expectedType, x, index, error);
};

Z Contract createContract(K dict, std::string &error)
{
    Contract c;
    auto map = new std::map<std::string, std::function<void(K x, I i, std::string &err)>> {
        { "conId",          partial(f, &c.conId,        -KI) },
        { "currency",       partial(f, &c.currency,     -KS) },
        { "exchange",       partial(f, &c.exchange,     -KS) },
        { "expiry",         partial(f, &c.expiry,       -KM) },
        { "includeExpired", partial(f, &c.includeExpired, -KB) },
        { "localSymbol",    partial(f, &c.localSymbol,  -KS) },
        { "multiplier",     partial(f, &c.multiplier,   -KS) },
        { "primaryExchange",partial(f, &c.primaryExchange, -KS) },
        { "right",          partial(f, &c.right,        -KS) },
        { "secId",          partial(f, &c.secId,        -KS) },
        { "secType",        partial(f, &c.secType,      -KS) },
        { "strike",         partial(f, &c.strike,       -KF) },
        { "symbol",         partial(f, &c.symbol,       -KS) },
        { "tradingClass",   partial(f, &c.tradingClass, -KS) }
    };
    setProperties(dict, *map, error);
    R c;
}

Z Order createOrder(K dict, std::string &error)
{
    Order o;
    auto map = new std::map<std::string, std::function<void(K x, I i, std::string &err)>> {
        // Order Identifiers
        { "clientId",       partial(f, &o.clientId,     -KJ) },
        { "orderId",        partial(f, &o.orderId,      -KJ) },
        { "permId",         partial(f, &o.permId,       -KJ) },
        // Main Order Fields
        { "action",         partial(f, &o.action,       -KS) },
        { "auxPrice",       partial(f, &o.auxPrice,     -KF) },
        { "lmtPrice",       partial(f, &o.lmtPrice,     -KF) },
        { "orderType",      partial(f, &o.orderType,    -KS) },
        { "totalQuantity",  partial(f, &o.totalQuantity, -KJ) },
        // Extended Order Fields
        { "allOrNone",      partial(f, &o.allOrNone,    -KB) },
        { "blockOrder",     partial(f, &o.blockOrder,   -KB) },
        { "displaySize",    partial(f, &o.displaySize,  -KI) },
        { "goodAfterTime",  partial(f, &o.goodAfterTime, -KZ) },
        { "goodTilDate",    partial(f, &o.goodTillDate, -KZ) },
        { "hidden",         partial(f, &o.hidden,       -KB) },
        { "minQty",         partial(f, &o.minQty,       -KI) },
        { "ocaGroup",       partial(f, &o.ocaGroup,     -KS) },
        { "orderRef",       partial(f, &o.orderRef,     -KS) },
        { "outsideRth",     partial(f, &o.outsideRth,   -KB) },
        { "overridePercentageConstraints", partial(f, &o.overridePercentageConstraints, -KB) },
        { "parentId",       partial(f, &o.parentId,     -KJ) },
        { "percentOffset",  partial(f, &o.percentOffset,-KF) },
        { "rule80A",        partial(f, &o.rule80A,      -KS) },
        { "tif",            partial(f, &o.tif,          -KS) },
        { "sweepToFill",    partial(f, &o.sweepToFill,  -KB) },
        { "trailingPercent",partial(f, &o.trailingPercent, -KF) },
        { "trailingStopPrice", partial(f, &o.trailStopPrice, -KF) },
        { "transmit",       partial(f, &o.transmit,     -KB) },
        { "triggerMethod",  partial(f, &o.triggerMethod,-KI) },
        { "activeStartTime",partial(f, &o.activeStartTime, -KZ) },
        { "activeStopTime", partial(f, &o.activeStopTime, -KZ) }
    };
    setProperties(dict, *map, error);
    R o;
}
