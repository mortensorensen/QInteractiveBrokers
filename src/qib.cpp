#include "qib.h"
#include "config.h"
#include "IBClient.h"
#include <sstream>

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
    
    std::string error;
    auto c = createContract(contract, error);
    Q(!error.empty(), error.c_str());
    
    auto o = createOrder(order, error);
    Q(!error.empty(), error.c_str());
    
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

Z bool checkDictTypes(K dict, std::map<std::string, short> &propTypes, const char *&error)
{
    if (dict->t != XD) {
        error = "argument must be a dict";
        return false;
    }
    
    K keys = kK(dict)[0];
    K vals = kK(dict)[1];
    
    if (keys->t != KS) {
        error = "keys must be syms";
        return false;
    }
    
    I type;
    
    for (I i = 0; i < keys->n; i++) {
        auto key = kS(keys)[i];
        auto search = propTypes.find(key);
        if (search != propTypes.end()) {
            type = vals->t == 0 ? kK(vals)[i]->t : -abs(vals->t);
            if (type != search->second) {
                std::stringstream ss;
                ss << "Key " << key << ": Type " << type << " (expected " << search->second << ")\n";
                error = ss.str().c_str();
                return false;
            }
        }
    }
            
    return true;
}

ZV setAtom(V *property, K x)
{
    switch (xt) {
        case  -1: *((bool*)property) = x->g == 1; break;
        case  -4: *((C*)property) = x->g; break;
        case  -5: *((H*)property) = x->h; break;
        case  -6: *((I*)property) = x->i; break;
        case  -7: *((J*)property) = x->j; break;
        case  -8: *((E*)property) = x->e; break;
        case  -9: *((F*)property) = x->f; break;
        case -10: *((C*)property) = x->g; break;
        case -11: *((IBString*)property) = x->s; break;
        case -12: *((J*)property) = x->j; break;
        case -13: *((I*)property) = x->i; break;
        case -14: *((I*)property) = x->i; break;
        case -15: *((F*)property) = x->f; break;
        case -16: *((J*)property) = x->j; break;
        case -17: *((I*)property) = x->i; break;
        case -18: *((I*)property) = x->i; break;
        case -19: *((I*)property) = x->i; break;
        default: return;
    }
}

V setItem(V *property, K x, I index)
{
    switch (xt) {
        case  0: setProperty(property, kK(x)[index], index); break;
        case  1: setAtom(property, kb(kG(x)[index])); break;
        case  4: setAtom(property, kg(kG(x)[index])); break;
        case  5: setAtom(property, kh(kH(x)[index])); break;
        case  6: setAtom(property, ki(kI(x)[index])); break;
        case  7: setAtom(property, kj(kJ(x)[index])); break;
        case  8: setAtom(property, ke(kE(x)[index])); break;
        case  9: setAtom(property, kf(kF(x)[index])); break;
        case 10: setAtom(property, kc(kC(x)[index])); break;
        case 11: setAtom(property, ks(kS(x)[index])); break;
        case 14: setAtom(property, kd(kI(x)[index])); break;
        case 15: setAtom(property, kz(kF(x)[index])); break;
        default: return;
    }
}

V setProperty(V *property, K x, I index)
{
    if (xt < 0) setAtom(property, x);
    else if ((0 <= xt) && (xt < 20)) setItem(property, x, index);
    else return;
}

V setProperties(K dict, std::map<std::string, void*> props, std::string &error)
{
    if (dict->t != XD) {
        error = "type";
        return;
    }
    
    K keys = kK(dict)[0];
    K vals = kK(dict)[1];
    
    if (keys->t != KS) {
        error = "keys must be syms";
        return;
    }
    
    std::map<std::string, void*>::const_iterator it;
    std::string key;
    
    for (I i = 0; i < keys->n; i++) {
        key = kS(keys)[i];
        it = props.find(key);
        if (it != props.end()) {
            setProperty(it->second, vals, i);
        } else {
            error = "Property " + key + " not recognized\n";
        }
    }
}

Z Contract *createContract(K dict, std::string &error)
{
    auto c = new Contract();
    auto props = std::map<std::string, void*> {
        { "conId",          &c->conId },
        { "currency",       &c->currency },
        { "exchange",       &c->exchange },
        { "expiry",         &c->expiry },
        { "includeExpired", &c->includeExpired },
        { "localSymbol",    &c->localSymbol },
        { "multiplier",     &c->multiplier },
        { "primaryExchange",&c->primaryExchange },
        { "right",          &c->right },
        { "secId",          &c->secId },
        { "secType",        &c->secType },
        { "strike",         &c->strike },
        { "symbol",         &c->symbol },
        { "tradingClass",   &c->tradingClass }
    };
    
    setProperties(dict, props, error);
    
    R c;
}

Z Order *createOrder(K dict, std::string &error)
{
    auto o = new Order();
    auto props = std::map<std::string, void*> {
        // Order Identifiers
        { "clienId",        &o->clientId },
        { "orderId",        &o->orderId },
        { "permId",         &o->permId },
        // Main Order Fields
        { "action",         &o->action },
        { "auxPrice",       &o->auxPrice },
        { "lmtPrice",       &o->lmtPrice },
        { "orderType",      &o->orderType },
        { "totalQuantity",  &o->totalQuantity },
        // Extended Order Fields
        { "allOrNone",      &o->allOrNone },
        { "blockOrder",     &o->blockOrder },
        { "displaySize",    &o->displaySize },
        { "goodAfterTime",  &o->goodAfterTime },
        { "goodTillDate",   &o->goodTillDate },
        { "hidden",         &o->hidden },
        { "minQty",         &o->minQty },
        { "ocaGroup",       &o->ocaGroup },
        { "orderRef",       &o->orderRef },
        { "outsideRth",     &o->outsideRth },
        { "overridePercentageConstraints", &o->overridePercentageConstraints },
        { "parentId",       &o->parentId },
        { "percentOffset",  &o->percentOffset },
        { "rule80A",        &o->rule80A },
        { "tif",            &o->tif },
        { "sweepToFill",    &o->sweepToFill },
        { "trailingPercent",&o->trailingPercent },
        { "trailStopPrice", &o->trailStopPrice },
        { "transmit",       &o->transmit },
        { "triggerMethod",  &o->triggerMethod },
        { "activeStartTime",&o->activeStartTime },
        { "activeStopTime", &o->activeStopTime }
    };
    
    setProperties(dict, props, error);

    R o;
}
