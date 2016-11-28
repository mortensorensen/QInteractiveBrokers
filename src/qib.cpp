#include "qib.h"
#include "config.h"
#include "IBClient.h"
#include <sstream>

IBClient *ib;

__attribute__((constructor))
static void initialize_api() {
    ib = new IBClient();
}

__attribute__((destructor))
static void destroy_api() {
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
    
    Q(!checkDictTypes(contract, contractPropTypes), "dict type");
    
    const char *errorMsg = nullptr;
    auto c = createContract(contract, errorMsg);
    Q(errorMsg, errorMsg);
    
    ib->reqMktData(tickerId->j, *c, "", snapsnot->i == 1);
    
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
    Q(!checkDictTypes(contract, contractPropTypes), "contract dict type");
    Q(!checkDictTypes(order, orderPropTypes), "order dict type");
    
    const char *errorMsg = nullptr;
    auto c = createContract(order, errorMsg);
    Q(errorMsg, errorMsg);
    
    auto o = createOrder(order, errorMsg);
    Q(errorMsg, errorMsg);
    
    ib->placeOrder(id->j, *c, *o);
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

static bool checkDictTypes(K dict, std::map<std::string, short> &propTypes)
{
    if (dict->t != XD) return false;
    
    K keys = kK(dict)[0];
    K vals = kK(dict)[1];
    
    if (vals->t != KS) return false;
    
    int type;
    
    for (int i = 0; i < keys->n; i++) {
        auto key = kS(keys)[i];
        auto search = propTypes.find(key);
        if (search != propTypes.end()) {
            type = vals->t == 0 ? kK(vals)[i]->t : -abs(vals->t);
            if (type != search->second) return false;
        }
    }
            
    return true;
}

static Contract *createContract(K dict, const char *&error)
{
    if (dict->t != XD) {
        error = "type";
        return nullptr;
    }
    
    auto c = new Contract();
    K keys = kK(dict)[0];
    K vals = kK(dict)[1];
    
    int type;
    
    for (int i = 0; i < keys->n; i++) {
        auto key = kS(keys)[i];
        type = vals->t == 0 ? kK(vals)[i]->t : -abs(vals->t);
        
        if (strcmp(key, "conId") == 0) c->conId = kJ(vals)[i];
        else if (strcmp(key, "currency") == 0) c->currency = kS(vals)[i];
        else if (strcmp(key, "exchange") == 0) c->exchange = kS(vals)[i];
        else if (strcmp(key, "expiry") == 0) c->expiry = kS(vals)[i];
        else if (strcmp(key, "includeExpired") == 0) c->includeExpired = kG(vals)[i];
        else if (strcmp(key, "localSymbol") == 0) c->localSymbol = kS(vals)[i];
        else if (strcmp(key, "multiplier") == 0) c->multiplier = kS(vals)[i];
        else if (strcmp(key, "primaryExchange") == 0) c->primaryExchange = kS(vals)[i];
        else if (strcmp(key, "right") == 0) c->right = kS(vals)[i];
        else if (strcmp(key, "secId") == 0) c->secId = kS(vals)[i];
        else if (strcmp(key, "secType") == 0) c->secType = kS(vals)[i];
        else if (strcmp(key, "strike") == 0) c->strike = kF(vals)[i];
        else if (strcmp(key, "symbol") == 0) c->symbol = kS(vals)[i];
        else if (strcmp(key, "tradingClass") == 0) c->tradingClass = kS(vals)[i];
        else {
            std::stringstream ss;
            ss << "Value " << key << ": Type " << type << " not recognized\n";
            error = ss.str().c_str();
            return nullptr;
        }
    }
    
    return c;
}

static Order *createOrder(K dict, const char *&error)
{
    if (dict->t != XD) {
        error = "type";
        return nullptr;
    }
    
    auto order = new Order();
    K keys = kK(dict)[0];
    K vals = kK(dict)[1];
    
    int type;
    
    for (int i = 0; i < keys->n; i++) {
        auto key = kS(keys)[i];
        type = vals->t == 0 ? kK(vals)[i]->t : -abs(vals->t);
        
        if (strcmp(key, "clienId") == 0) order->clientId = kJ(vals)[i];
        else if (strcmp(key, "orderId") == 0) order->orderId = kJ(vals)[i];
        else if (strcmp(key, "permId") == 0) order->permId = kJ(vals)[i];
        else if (strcmp(key, "action") == 0) order->action = kS(vals)[i];
        else if (strcmp(key, "auxPrice") == 0) order->auxPrice = kF(vals)[i];
        else if (strcmp(key, "lmtPrice") == 0) order->lmtPrice = kF(vals)[i];
        else if (strcmp(key, "orderType") == 0) order->orderType = kS(vals)[i];
        else if (strcmp(key, "totalQuantity") == 0) order->totalQuantity= kJ(vals)[i];
        // TODO: Add remaining options
        else {
            std::stringstream ss;
            ss << "Value " << key << ": Type " << type << " not recognized\n";
            error = ss.str().c_str();
            return nullptr;
        }
    }
    
    return order;
}
