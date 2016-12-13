#include "qib.h"
#include "config.h"
#include "IBClient.h"
#include <sstream>
#include <typeinfo>

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
    R createDictionary(std::map<std::string, K> {
        { "release",    ks((S) BUILD_PROJECT_VERSION) },
        { "os",         ks((S) BUILD_OPERATING_SYSTEM) },
        { "tws",        ks((S) BUILD_TWS_VER) },
        { "kx",         ks((S) BUILD_KX_VER) }
    });
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

    auto dict = createDictionary(std::map<std::string, K> {
        { "version",        dl((V*) version,         1) },
        { "connect",        dl((V*) connect,         3) },
        { "disconnect",     dl((V*) disconnect,      1) },
        { "isConnected",    dl((V*) isConnected,     1) },
        { "reqCurrentTime", dl((V*) reqCurrentTime,  1) },
        { "reqMktData",     dl((V*) reqMktData,      4) },
        { "reqAccountUpdates", dl((V*) reqAccountUpdates, 2) },
        { "placeOrder",     dl((V*) placeOrder,      3) },
        { "cancelOrder",    dl((V*) cancelOrder,     1) }
    });
    R dict;
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
    
    ib->reqMktData(tickerId->j, c, "", snapsnot->i == 1);
    
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

template<typename T> std::string typeError(T *property, G expectedType)
{
    R stringFormat("Type %s not handled. Expected type id %s",
                   typeid(T).name(),
                   reinterpret_cast<char*>(expectedType));
}

template<typename T> V setAtom(T *property, K x, G type, std::string &error)
{
    // Base case. Should never be called.
    error = typeError(property, type);
}

template<> V setAtom(bool *property, K x, G type, std::string &error)
{
    SW(xt) {
        CS(-1, *property = x->g ==1)
        CD: typeError(property, type);
    }
}

template<> V setAtom(int *property, K x, G type, std::string &error)
{
    SW(xt) {
        CS(-KI, *property = x->i);
        CD: typeError(property, type);
    }
}

template<> V setAtom(long *property, K x, G type, std::string &error)
{
    SW(xt) {
        CS(-KJ, *property = x->j);
        CD: typeError(property, type);
    }
}

template<> V setAtom(double *property, K x, G type, std::string &error)
{
    SW(xt) {
        CS(-KF, *property = x->f);
        CD: typeError(property, type);
    }
}

template<> V setAtom(IBString *property, K x, G type, std::string &error)
{
    SW(xt) {
        CS(-KS, *property = x->s);                                                          // symbol
        CS(-KM, *property = stringFormat("%04d%02d", (x->i)/12+2000, (x->i)%12+1));         // month
        CS(-KD, *property = formatTime("%Y%m%d", ((x->i) + 10957)*8.64e4, 0));              // date
        CS(-KZ, *property = formatTime("%Y%m%dD %H:%M:%S", ((x->f) + 10957)*8.64e4, 0));    // datetime
        CD: typeError(property, type);
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
    I xtype = (0 < xt) && (xt < 20) ? -xt : kK(x)[index]->t;
    if (xtype != expectedType) {
        error = stringFormat("Invalid type: %i. Expected %i", xtype, expectedType);
        R;
    }
    
    if (xt < 0) setAtom(property, x, expectedType, error);
    else if ((0 <= xt) && (xt < 20)) setItem(property, expectedType, x, index, error);
    else R;
}

V setProperties(K dict, std::map<std::string, std::function<V(K x, I i, std::string &err)>> &props, std::string &error)
{
    K keys = kK(dict)[0];
    K vals = kK(dict)[1];
    
    if (keys->t != KS) {
        error = "Keys must be syms";
        R;
    }
    
    std::string key;
    for (I i = 0; i < keys->n; i++) {
        key = kS(keys)[i];
        
        auto it = props.find(key);
        if (it != props.end()) {
            (it->second)(vals, i, error);
            if (!error.empty()) R;
        } else {
            error = "Key not recognized: " + key;
            R;
        }
    }
}

auto f = [](auto property, I expectedType, K x, I index, std::string &error) {
    R setProperty(property, expectedType, x, index, error);
};

Z Contract createContract(K dict, std::string &error)
{
    Contract c;
    auto map = new std::map<std::string, std::function<V(K, I, std::string&)>> {
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
    auto map = new std::map<std::string, std::function<V(K, I, std::string&)>> {
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
        { "activeStopTime", partial(f, &o.activeStopTime, -KZ) },
        // Financial Advisor Fields
        { "designatedLocation", partial(f, &o.designatedLocation, -KS) },
        { "openClose",      partial(f, &o.openClose,    -KS) },
        { "origin",         partial(f, &o.origin,       -KI) },
        { "shortSaleSlot",  partial(f, &o.shortSaleSlot, -KI) },
        // SMART Routing Only
        { "discretionaryAmt", partial(f, &o.discretionaryAmt, -KF) },
        { "eTradeOnly",     partial(f, &o.eTradeOnly,   -KB) },
        { "firmQuoteOnly",  partial(f, &o.firmQuoteOnly,-KB) },
        { "nbboPriceCap",   partial(f, &o.nbboPriceCap, -KF) },
        { "optOutSmartRouting", partial(f, &o.optOutSmartRouting, -KB) }
    };
    setProperties(dict, *map, error);
    R o;
}
