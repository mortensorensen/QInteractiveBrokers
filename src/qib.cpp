#include "qib.h"
#include "config.h"
#include "IBClient.h"

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

K version(K ignore)
{
    R createDictionary(std::map<std::string, K> {
        { "release",    ks((S) BUILD_PROJECT_VERSION) },
        { "os",         ks((S) BUILD_OPERATING_SYSTEM) },
        { "tws",        ks((S) BUILD_TWS_VER) },
        { "kx",         ks((S) BUILD_KX_VER) }
    });
}

K LoadLibrary(K ignore)
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
        { "version",                dl((V*) version,            1) },
        // EPosixClientSocket
        { "connect",                dl((V*) connect,            3) },
        { "disconnect",             dl((V*) disconnect,         1) },
        { "isConnected",            dl((V*) isConnected,        1) },
        // EClientSocketBase
        { "calculateImpliedVolatility", dl((V*) calculateImpliedVolatility, 1) },
        { "cancelAccountSummary",   dl((V*) cancelAccountSummary, 1) },
        { "cancelCalculateImpliedVolatility", dl((V*) cancelCalculateImpliedVolatility, 1) },
        { "cancelCalculateOptionPrice", dl((V*) cancelCalculateOptionPrice, 1) },
        { "cancelFundamentalData",  dl((V*) cancelFundamentalData, 1) },
        { "cancelHistoricalData",  dl((V*) cancelHistoricalData, 1) },
        { "cancelMktData",          dl((V*) cancelMktData,      1) },
        { "cancelMktDepth",         dl((V*) cancelMktDepth,     1) },
        { "cancelNewsBulletins",    dl((V*) cancelNewsBulletins,1) },
        { "cancelOrder",            dl((V*) cancelOrder,        1) },
        { "cancelPositions",        dl((V*) cancelPositions,    1) },
        { "cancelRealTimeBars",     dl((V*) cancelRealTimeBars, 1) },
        { "checkMessages",          dl((V*) checkMessages,      1) },
        { "exerciseOptions",        dl((V*) exerciseOptions,    6) },
        { "placeOrder",             dl((V*) placeOrder,         3) },
        { "queryDisplayGroups",     dl((V*) queryDisplayGroups, 1) },
        { "replaceFA",              dl((V*) replaceFA,          1) },
        { "reqAccountSummary",      dl((V*) reqAccountSummary,  3) },
        { "reqAccountUpdates",      dl((V*) reqAccountUpdates,  2) },
        { "reqAllOpenOrders",       dl((V*) reqAllOpenOrders,   1) },
        { "reqAutoOpenOrders",      dl((V*) reqAutoOpenOrders,  1) },
        { "reqContractDetails",     dl((V*) reqContractDetails, 2) },
        { "reqCurrentTime",         dl((V*) reqCurrentTime,     1) },
        { "reqExecutions",          dl((V*) reqExecutions,      2) },
        { "reqFundamentalData",     dl((V*) reqFundamentalData, 3) },
        { "reqGlobalCancel",        dl((V*) reqGlobalCancel,    1) },
        { "reqHistoricalData",      dl((V*) reqHistoricalData,  1) },
        { "reqIds",                 dl((V*) reqIds,             1) },
        { "reqManagedAccts",        dl((V*) reqManagedAccts,    1) },
        { "reqMarketDataType",      dl((V*) reqMarketDataType,  1) },
        { "reqMktData",             dl((V*) reqMktData,         4) },
        { "reqMktDepth",            dl((V*) reqMktDepth,        4) },
        { "reqNewsBulletins",       dl((V*) reqNewsBulletins,   1) },
        { "reqOpenOrders",          dl((V*) reqOpenOrders,      1) },
        { "reqPositions",           dl((V*) reqPositions,       1) },
        { "reqRealTimeBars",        dl((V*) reqRealTimeBars,    6) },
        { "reqScannerParameters",   dl((V*) reqScannerParameters, 1) },
        { "reqScannerSubscription", dl((V*) reqScannerSubscription, 3) },
        { "requestFA",              dl((V*) requestFA,          1) },
        { "serverVersion",          dl((V*) serverVersion,      1) },
        { "setServerLogLevel",      dl((V*) setServerLogLevel,  1) },
        { "subscribeToGroupEvents", dl((V*) subscribeToGroupEvents, 2) },
        { "TwsConnectionTime",      dl((V*) TwsConnectionTime,  1) },
        { "unsubscribeFromGroupEvents", dl((V*) unsubscribeFromGroupEvents, 1) },
        { "updateDisplayGroup",     dl((V*) updateDisplayGroup, 2) },
        { "verifyMessage",          dl((V*) verifyMessage,      1) },
        { "verifyRequest",          dl((V*) verifyRequest,      1) }
    });
    R dict;
}

K eventLoop(I fd)
{
    ib->onReceive();
    R NULL;
}

///////////////////////
// EPosixClientSocket
///////////////////////

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

K disconnect(K ignore)
{
    ib->disconnect();
    R NULL;
}

K isConnected(K ignore)
{
    R kb(ib->isConnected());
}

///////////////////////
// EClient
///////////////////////

K calculateImpliedVolatility(K reqId, K contract, K optionPrice, K underPrice)
{
    R NULL;
}

K calculateOptionPrice( K reqId, K contract, K volatility, K underPrice)
{
    R NULL;
}

K cancelAccountSummary(K reqId)
{
    Q(reqId->t != -KJ, "type");
    ib->cancelAccountSummary(reqId->j);
    R NULL;
}

K cancelCalculateImpliedVolatility(K reqId)
{
    Q(reqId->t != -KJ, "type");
    ib->cancelCalculateImpliedVolatility(reqId->j);
    R NULL;
}

K cancelCalculateOptionPrice(K reqId)
{
    Q(reqId->t != -KJ, "type");
    ib->cancelCalculateOptionPrice(reqId->j);
    R NULL;
}

K cancelFundamentalData(K reqId)
{
    Q(reqId->t != -KJ, "type");
    ib->cancelFundamentalData(reqId->j);
    R NULL;
}

K cancelHistoricalData(K tickerId)
{
    Q(tickerId->t != -KJ, "type");
    ib->cancelHistoricalData(tickerId->j);
    R NULL;
}

K cancelMktData(K id) {
    Q(id->t != -KJ, "type");
    ib->cancelMktData(id->j);
    R NULL;
}

K cancelMktDepth(K tickerId)
{
    Q(tickerId->t != -KJ, "type");
    ib->cancelMktData(tickerId->j);
    R NULL;
}

K cancelNewsBulletins(K ignore)
{
    ib->cancelNewsBulletins();
    R NULL;
}

K cancelOrder(K id)
{
    Q(id->t != -KJ, "type");
    Q(!ib->isConnected(), "connection");
    ib->cancelOrder(id->j);
    R NULL;
}

K cancelPositions(K ignore)
{
    ib->cancelPositions();
    R NULL;
}

K cancelRealTimeBars(K tickerId)
{
    Q(tickerId->t != -KJ, "type");
    ib->cancelRealTimeBars(tickerId->j);
    R NULL;
}

K cancelScannerSubscription(K tickerId)
{
    Q(tickerId->t != -KJ, "type");
    ib->cancelScannerSubscription(tickerId->j);
    R NULL;
}

K checkMessages(K ignore)
{
    ib->checkMessages();
    R NULL;
}

K exerciseOptions(K tickerId, K contract, K exerciseAction, K exerciseQuantity, K account, K override)
{
    Q(tickerId->t != -KJ || contract->t != XD || exerciseAction->t != -KJ ||
      exerciseQuantity->t != -KJ || account->t != -KS || override->t != -KB, "type");
    
    std::string error;
    auto c = createContract(contract, error);
    Q(!error.empty(), error.c_str());
    
    ib->exerciseOptions(tickerId->j,
                        c,
                        exerciseAction->j,
                        exerciseQuantity->j,
                        account->s,
                        static_cast<I>(override->g));
    
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

K queryDisplayGroups(K reqId)
{
    Q(reqId->t != -KJ, "type");
    ib->queryDisplayGroups(reqId->j);
    R NULL;
}

K replaceFA(K pFaDataType, K cxml)
{
    Q(pFaDataType->t != -KJ || cxml->t != KC, "type");
    Q(pFaDataType->j < 1 || pFaDataType->j > 3, "pFaDataType");
    
//    auto datatype = static_cast<faDataType>(pFaDataType->j);
//    auto xml = static_cast<IBString>(cxml->g);
//    ib->replaceFA(datatype, xml);
    
    R krr((S)"nyi");
}

K reqAccountSummary(K reqId, K groupName, K tags)
{
    Q(reqId->t != -KJ || groupName->t != KC || tags->t != KC, "type");
    R krr((S)"nyi");
}

K reqAccountUpdates(K subscribe, K acctCode)
{
    Q(subscribe->t != -KB || acctCode->t != -KS, "type");
    Q(!ib->isConnected(), "connection");
    ib->reqAccountUpdates(static_cast<I>(subscribe->g), acctCode->s);
    R NULL;
}

K reqAllOpenOrders(K ignore)
{
    ib->reqAllOpenOrders();
    R NULL;
}

K reqAutoOpenOrders(K bAutoBind)
{
    Q(bAutoBind->t != -KB, "type");
    ib->reqAutoOpenOrders(static_cast<I>(bAutoBind->g));
    R NULL;
}

K reqContractDetails(K reqId, K contract)
{
    Q(reqId->t != -KJ || contract->t != XD, "type");
    
    std::string error;
    auto c = createContract(contract, error);
    Q(!error.empty(), error.c_str());
    
    ib->reqContractDetails(reqId->j, c);
    R NULL;
}

K reqCurrentTime(K ignore)
{
    ib->reqCurrentTime();
    R NULL;
}

K reqExecutions(K reqId, K filter)
{
    Q(reqId->t != -KJ || filter->t != XD, "type");
    
    std::string error;
    auto ef = createExecutionFilter(filter, error);
    Q(!error.empty(), error.c_str());
    
    ib->reqExecutions(reqId->j, ef);
    R NULL;
}

K reqFundamentalData(K reqId, K contract, K reportType)
{
    Q(reqId->t != -KJ || contract->t != XD || reportType->t != -KS, "type");
    
    std::string error;
    auto c = createContract(contract, error);
    Q(!error.empty(), error.c_str());
    
    ib->reqFundamentalData(reqId->j, c, reportType->s);
    R NULL;
}

K reqGlobalCancel(K ignore)
{
    ib->reqGlobalCancel();
    R NULL;
}

K reqHistoricalData(K dict)
{
    Q(dict->t != XD, "type");
    // K id, K contract, K endDateTime, K durationStr, K barSizeSetting, K whatToShow, K useRTH, K formatDate, K chartOptions
    R krr((S)"nyi");
}

K reqIds(K numIds)
{
    Q(numIds->t != -KJ, "type");
    ib->reqIds(numIds->j);
    R NULL;
}

K reqManagedAccts(K ignore)
{
    ib->reqManagedAccts();
    R NULL;
}

K reqMarketDataType(K marketDataType)
{
    Q(marketDataType->t != -KJ, "type");
    ib->reqMarketDataType(marketDataType->j);
    R NULL;
}

K reqMktData(K tickerId, K contract, K genericTicks, K snapsnot)
{
    Q(tickerId->t != -KJ || contract->t != XD || genericTicks->t != KC || snapsnot->t != -KB, "type");
    Q(!ib->isConnected(), "connection");
    
    std::string error;
    auto c = createContract(contract, error);
    Q(!error.empty(), error.c_str());
    
    TagValueListSPtr tag;
    ib->reqMktData(tickerId->j, c, "", static_cast<I>(snapsnot->g), tag);
    
    R NULL;
}

K reqMktDepth(K tickerId, K contract, K numRows, K mktDepthOptions)
{
    Q(tickerId->t != -KJ || contract->t != XD || numRows->t != -KJ || mktDepthOptions->t != XD, "type");
    
    std::string error;
    auto c = createContract(contract, error);
    Q(!error.empty(), error.c_str());
    
    auto tvl = createTagValueList(mktDepthOptions, error);
    Q(!error.empty(), error.c_str());
    
    ib->reqMktDepth(tickerId->j, c, numRows->j, tvl);
    R NULL;
}

K reqNewsBulletins(K allMsgs)
{
    Q(allMsgs->t != -KB, "type");
    ib->reqNewsBulletins(static_cast<I>(allMsgs->g));
    R NULL;
}

K reqOpenOrders(K ignore)
{
    ib->reqOpenOrders();
    R NULL;
}

K reqPositions(K ignore)
{
    ib->reqPositions();
    R NULL;
}

K reqRealTimeBars(K id, K contract, K barSize, K whatToShow, K useRTH, K realTimeBarsOptions)
{
    Q(id->t != -KJ || contract->t != XD || barSize->t != -KJ || whatToShow->t != -KS ||
      useRTH->t != -KB || realTimeBarsOptions->t != XD, "type");
    
    std::string error;
    auto c = createContract(contract, error);
    Q(!error.empty(), error.c_str());
    
    auto tvl = createTagValueList(realTimeBarsOptions, error);
    Q(!error.empty(), error.c_str());
    
    ib->reqRealTimeBars(id->j,
                        c,
                        barSize->j,
                        whatToShow->s,
                        static_cast<I>(useRTH->g),
                        tvl);
    
    R NULL;
}

K reqScannerParameters(K ignore)
{
    ib->reqScannerParameters();
    R NULL;
}

K reqScannerSubscription(K tickerId, K subscription, K scannerSubscriptionOptions)
{
    Q(tickerId->t != -KJ || subscription->t != XD || scannerSubscriptionOptions->t != XD, "type");
    
    std::string error;
    auto ss = createScannerSubscription(subscription, error);
    Q(!error.empty(), error.c_str());
    
    auto tvl = createTagValueList(scannerSubscriptionOptions, error);
    Q(!error.empty(), error.c_str());

    ib->reqScannerSubscription(tickerId->j, ss, tvl);
    R NULL;
}

K requestFA(K pFaDataType)
{
    Q(pFaDataType->t != -KS, "type");
    Q(pFaDataType->j < 1 || pFaDataType->j > 3, "unknown faDataType");
    
    ib->requestFA(static_cast<faDataType>(pFaDataType->j));
    R NULL;
}

K serverVersion(K ignore)
{
    R ki(ib->serverVersion());
}

K setServerLogLevel(K level)
{
    Q(level->t != -KJ, "type");
    ib->setServerLogLevel(level->j);
    R NULL;
}

K subscribeToGroupEvents(K reqId, K groupId)
{
    Q(reqId->t != -KJ || groupId->t != -KJ, "type");
    ib->subscribeToGroupEvents(reqId->j, groupId->j);
    R NULL;
}

K TwsConnectionTime(K ignore)
{
    R kip(ib->TwsConnectionTime());
}

K unsubscribeFromGroupEvents(K reqId)
{
    Q(reqId->t != -KJ, "type");
    ib->unsubscribeFromGroupEvents(reqId->j);
    R NULL;
}

K updateDisplayGroup(K reqId, K contractInfo)
{
    Q(reqId->t != -KJ || contractInfo->t != KC, "type");
    ib->updateDisplayGroup(reqId->j, contractInfo->s); // TODO: Validate string extraction
    R NULL;
}

K verifyMessage(K apiData)
{
    Q(apiData->t != KC, "type");
    ib->verifyMessage(apiData->s);
    R NULL;
}

K verifyRequest(K apiName, K apiVersion)
{
    Q(apiName->t != -KS || apiVersion->t != KC, "type");
    ib->verifyRequest(apiName->s, apiVersion->s); // TODO: Validate string extraction
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
        CS(-1, *property = static_cast<I>(x->g));
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
        { "conId",          partial(f, &c.conId,        -KJ) },
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

Z ExecutionFilter createExecutionFilter(K dict, std::string &error)
{
    ExecutionFilter ef;
    auto map = new std::map<std::string, std::function<V(K, I, std::string&)>> {
        { "clientId",   partial(f, &ef.m_clientId,     -KJ) },
        { "acctCode",   partial(f, &ef.m_acctCode,     -KS) },
        { "time",       partial(f, &ef.m_time,         -KZ) },
        { "symbol",     partial(f, &ef.m_symbol,       -KS) },
        { "secType",    partial(f, &ef.m_secType,      -KS) },
        { "exchange",   partial(f, &ef.m_exchange,     -KS) },
        { "side",       partial(f, &ef.m_side,         -KS) }
    };
    setProperties(dict, *map, error);
    R ef;
}

Z ScannerSubscription createScannerSubscription(K dict, std::string &error)
{
    ScannerSubscription ss;
    auto map = new std::map<std::string, std::function<V(K, I, std::string&)>> {
        { "numberOfRows",           partial(f, &ss.numberOfRows,        -KJ) },
        { "instrument",             partial(f, &ss.instrument,          -KS) },
        { "locationCode",           partial(f, &ss.locationCode,         KC) },
        { "scanCode",               partial(f, &ss.scanCode,             KC) },
        { "abovePrice",             partial(f, &ss.abovePrice,          -KF) },
        { "belowPrice",             partial(f, &ss.belowPrice,          -KF) },
        { "aboveVolume",            partial(f, &ss.aboveVolume,         -KJ) },
        { "averageOptionVolumeAbove", partial(f, &ss.averageOptionVolumeAbove, -KJ) },
        { "marketCapAbove",         partial(f, &ss.marketCapAbove,      -KF) },
        { "marketCapBelow",         partial(f, &ss.marketCapBelow,      -KF) },
        { "moodyRatingAbove",       partial(f, &ss.moodyRatingAbove,     KC) },
        { "moodyRatingBelow",       partial(f, &ss.moodyRatingBelow,     KC) },
        { "spRatingAbove",          partial(f, &ss.spRatingAbove,        KC) },
        { "spRatingBelow",          partial(f, &ss.spRatingBelow,        KC) },
        { "maturityDateAbove",      partial(f, &ss.maturityDateAbove,   -KD) },
        { "maturityDateBelow",      partial(f, &ss.maturityDateBelow,   -KD) },
        { "couponRateAbove",        partial(f, &ss.couponRateAbove,     -KF) },
        { "couponRateBelow",        partial(f, &ss.couponRateBelow,     -KF) },
        { "excludeConvertible",     partial(f, &ss.excludeConvertible,  -KB) },
        { "scannerSettingPairs",    partial(f, &ss.scannerSettingPairs,  KC) },
        { "stockTypeFilter",        partial(f, &ss.stockTypeFilter,      KC) }
        
    };
    setProperties(dict, *map, error);
    R ss;
}

Z TagValueListSPtr createTagValueList(K dict, std::string &error)
{
    TagValueListSPtr list;
    if (dict->t != XD) {
        error = "type";
        R list;
    }
    
    K keys = kK(dict)[0];
    K vals = kK(dict)[1];
    
    if (keys->n > 0 && keys->t != KS) {
        error = "Keys must be syms";
        R list;
    }
    
    for (I i = 0; i < keys->n; i++) {
        TagValueSPtr tagValue(new TagValue());
        tagValue->tag = kS(keys)[i];
        partial(f, &tagValue->value, KC)(vals, i, error);
        
        if (!error.empty()) R list;
        list->push_back(tagValue);
    }
    
    R list;
}
