#include "IBClient.h"

#include "EPosixClientSocket.cpp"
#include "EPosixClientSocketPlatform.h"
#include "EClientSocketBaseImpl.h"
#include "helpers.h"

/////////////////////////////////////////////
//// Public functions
/////////////////////////////////////////////

IBClient::IBClient()
    : socket(new EPosixClientSocket(this))
{
}

IBClient::~IBClient()
{
}

bool IBClient::connect(const char *host, unsigned int port, int clientId)
{
    return socket->eConnect(host, port, clientId, false);
}

void IBClient::disconnect() const
{
    sd0(fd());
    socket->eDisconnect();
}

bool IBClient::isConnected() const
{
    return socket->isConnected();
}

int IBClient::fd() const
{
    return socket->fd();
}

void IBClient::onReceive()
{
    socket->onReceive();
}

/////////////////////////////////////////////
//// Private functions
/////////////////////////////////////////////

void IBClient::receiveData(const char *fun, K x)
{
    K r = k(0, (S)".ib.onrecv", ks((S)fun), x, (K)0);
    if (!r) {
        O("Broken socket");
    } else if (r->t == -128) {
        O("Error calling '%s': %s. Type: %i. Length: %lli\n", fun, r->s, xt, xn);
    }
    r0(r);
}

K IBClient::convertContract(const Contract &contract)
{
    auto dict = createDictionary(std::map<std::string, K> {
        { "conId",              kj(contract.conId) },
        { "symbol",             kis(contract.symbol) },
        { "secType",            kis(contract.secType) },
        { "expiry",             kip(contract.expiry) },
        { "strike",             kf(contract.strike) },
        { "right",              kis(contract.right) },
        { "multiplier",         kip(contract.multiplier) },
        { "exchange",           kis(contract.exchange) },
        { "primaryExchange",    kis(contract.primaryExchange) },
        { "localSymbol",        kis(contract.localSymbol) },
        { "tradingClass",       kis(contract.tradingClass) },
        { "includeExpired",     kb(contract.includeExpired) },
        { "secIdType",          kis(contract.secIdType) },
        { "secId",              kis(contract.secId) },
        // COMBOS - TODO ...
        { "comboLegsDescrip",   ks((S)"") },
        { "comboLegList",       ks((S)"") }
    });
    R dict;
}

K IBClient::convertContractDetails(const ContractDetails &contract)
{
    auto dict = createDictionary(std::map<std::string, K> {
        { "summary",        convertContract(contract.summary) },
        { "marketName",     kip(contract.marketName) },
        { "minTick",        kf(contract.minTick) },
        { "orderTypes",     kip(contract.orderTypes) },
        { "validExchanges", kip(contract.validExchanges) },
        { "priceMagnifier", kj(contract.priceMagnifier) },
        { "underConId",     kj(contract.underConId) },
        { "longName",       kip(contract.longName) },
        { "contractMonth",  kip(contract.contractMonth) },
        { "industry",       kip(contract.industry) },
        { "category",       kip(contract.category) },
        { "subcategory",    kip(contract.subcategory) },
        { "timeZoneId",     kip(contract.timeZoneId) },
        { "tradingHours",   kip(contract.tradingHours) },
        { "liquidHours",    kip(contract.liquidHours) },
        { "evRule",         kip(contract.evRule) },
        { "evMultiplier",   kf(contract.evMultiplier) },
        { "secIdList",      ks((S)"") } // TODO
    });
    R dict;
}

K IBClient::convertBondContractDetails(const ContractDetails &contract)
{
    auto dict = convertContractDetails(contract);
    auto bondDict = createDictionary(std::map<std::string, K> {
        { "cusip",              kis(contract.cusip) },
        { "ratings",            kip(contract.ratings) },
        { "descAppend",         kis(contract.descAppend) },
        { "bondType",           kip(contract.bondType) },
        { "couponType",         kip(contract.couponType) },
        { "callable",           kb(contract.callable) },
        { "putable",            kb(contract.putable) },
        { "coupon",             kf(contract.coupon) },
        { "convertible",        kb(contract.convertible) },
        { "maturity",           kip(contract.maturity) },       // TODO - convert
        { "issueDate",          kip(contract.issueDate) },      // TODO - convert
        { "nextOptionDate",     kip(contract.nextOptionDate) }, // TODO - convert
        { "nextOptionType",     kip(contract.nextOptionType) }, // TODO - convert
        { "nextOptionPartial",  kb(contract.nextOptionPartial) },
        { "notes",              kip(contract.notes) }
    });
    jk(&dict, bondDict);
    R dict;
}

K IBClient::convertExecution(const Execution &execution)
{
    std::tm time;
    std::istringstream ss(execution.time);
    ss >> std::get_time(&time, "%Y%m%d %T");
    
    auto dict = createDictionary(std::map<std::string, K> {
        { "execId",         kis(execution.execId) },
        { "time",           ss.fail() ? kip(execution.time) : kz(zu(mktime(&time))) }, // GMT time
        { "acctNumber",     kis(execution.acctNumber) },
        { "exchange",       kis(execution.exchange) },
        { "side",           kis(execution.side) },
        { "shares",         ki(execution.shares) },
        { "price",          kf(execution.price) },
        { "permId",         ki(execution.permId) },
        { "clientId",       kj(execution.clientId) },
        { "orderId",        kj(execution.orderId) },
        { "liquidation",    ki(execution.liquidation) },
        { "cumQty",         ki(execution.cumQty) },
        { "avgPrice",       kf(execution.avgPrice) },
        { "evRule",         kip(execution.evRule) },
        { "evMultiplier",   kf(execution.evMultiplier) }
    });
    R dict;
}

K IBClient::convertCommissionReport(const CommissionReport &report)
{
    auto dict = createDictionary(std::map<std::string, K> {
        { "commission",             kf(report.commission) },
        { "currency",               kis(report.currency) },
        { "execId",                 kis(report.execId) },
        { "realizedPNL",            kf(report.realizedPNL) },
        { "yield",                  kf(report.yield) },
        { "yieldRedemptionDate",    kip(stringFormat("%i", report.yieldRedemptionDate)) }
    });
    R dict;
}

K IBClient::convertUnderComp(const UnderComp &comp)
{
    auto dict = createDictionary(std::map<std::string, K> {
        { "conId",  kj(comp.conId) },
        { "delta",  kf(comp.delta) },
        { "price",  kf(comp.price) }
    });
    R dict;
}

K IBClient::convertOrderState(const OrderState &orderState)
{
    auto dict = createDictionary(std::map<std::string, K> {
        { "commission",     kf(orderState.commission) },
        { "commissionCurrency", kis(orderState.commissionCurrency) },
        { "equityWithLoan", kip(orderState.equityWithLoan) },
        { "initMargin",     kip(orderState.initMargin) },
        { "maintMargin",    kip(orderState.maintMargin) },
        { "maxCommission",  kf(orderState.maxCommission) },
        { "minCommission",  kf(orderState.minCommission) },
        { "status",         kip(orderState.status) },
        { "warningText",    kip(orderState.warningText) }
    });
    R dict;
}

/////////////////////////////////////////////
//// Methods
/////////////////////////////////////////////

bool IBClient::checkMessages()
{
    return socket->checkMessages();
}

IBString IBClient::TwsConnectionTime()
{
    return socket->TwsConnectionTime();
}

int IBClient::serverVersion()
{
    return socket->serverVersion();
}

void IBClient::calculateImpliedVolatility(TickerId reqId, const Contract &contract, double optionPrice, double underPrice)
{
    socket->calculateImpliedVolatility(reqId, contract, optionPrice, underPrice);
}

void IBClient::calculateOptionPrice(TickerId reqId, const Contract &contract, double volatility, double underPrice)
{
    socket->calculateOptionPrice(reqId, contract, volatility, underPrice);
}

void IBClient::cancelAccountSummary(int reqId)
{
    socket->cancelAccountSummary(reqId);
}

void IBClient::cancelCalculateImpliedVolatility(TickerId reqId)
{
    socket->cancelCalculateImpliedVolatility(reqId);
}

void IBClient::cancelCalculateOptionPrice(TickerId reqId)
{
    socket->cancelCalculateOptionPrice(reqId);
}

void IBClient::cancelFundamentalData(TickerId reqId)
{
    socket->cancelFundamentalData(reqId);
}

void IBClient::cancelHistoricalData(TickerId tickerId)
{
    socket->cancelHistoricalData(tickerId);
}

void IBClient::cancelMktData(TickerId id)
{
    socket->cancelMktData(id);
}

void IBClient::cancelMktDepth(TickerId tickerId)
{
    socket->cancelMktDepth(tickerId);
}

void IBClient::cancelNewsBulletins()
{
    socket->cancelNewsBulletins();
}

void IBClient::cancelOrder(OrderId id)
{
    socket->cancelOrder(id);
}

void IBClient::cancelPositions()
{
    socket->cancelPositions();
}

void IBClient::cancelRealTimeBars(TickerId tickerId)
{
    socket->cancelRealTimeBars(tickerId);
}

void IBClient::cancelScannerSubscription(int tickerId)
{
    socket->cancelScannerSubscription(tickerId);
}

void IBClient::exerciseOptions(TickerId tickerId, const Contract &contract, int exerciseAction, int exerciseQuantity, const IBString &account, int override)
{
    socket->exerciseOptions(tickerId, contract, exerciseAction, exerciseQuantity, account, override);
}

void IBClient::placeOrder(OrderId id, const Contract &contract, const Order &order)
{
    socket->placeOrder(id, contract, order);
}

void IBClient::queryDisplayGroups(int reqId)
{
    socket->queryDisplayGroups(reqId);
}

void IBClient::replaceFA(faDataType pFaDataType, const IBString& cxml)
{
    socket->replaceFA(pFaDataType, cxml);
}

void IBClient::reqAccountSummary(int reqId, const IBString& groupName, const IBString& tags)
{
    socket->reqAccountSummary(reqId, groupName, tags);
}

void IBClient::reqAccountUpdates(bool subscribe, const IBString& acctCode)
{
    socket->reqAccountUpdates(subscribe, acctCode);
}

void IBClient::reqAllOpenOrders()
{
    socket->reqAllOpenOrders();
}

void IBClient::reqAutoOpenOrders(bool bAutoBind)
{
    socket->reqAutoOpenOrders(bAutoBind);
}

void IBClient::reqContractDetails(int reqId, const Contract &contract)
{
    socket->reqContractDetails(reqId, contract);
}

void IBClient::reqCurrentTime()
{
    socket->reqCurrentTime();
}

void IBClient::reqExecutions(int reqId, const ExecutionFilter& filter)
{
    socket->reqExecutions(reqId, filter);
}

void IBClient::reqFundamentalData(TickerId reqId, const Contract& contract, const IBString& reportType)
{
    socket->reqFundamentalData(reqId, contract, reportType);
}

void IBClient::reqGlobalCancel()
{
    socket->reqGlobalCancel();
}

void IBClient::reqHistoricalData(TickerId id, const Contract &contract, const IBString &endDateTime, const IBString &durationStr, const IBString & barSizeSetting, const IBString &whatToShow, int useRTH, int formatDate, const TagValueListSPtr& chartOptions)
{
    socket->reqHistoricalData(id, contract, endDateTime, durationStr, barSizeSetting, whatToShow, useRTH, formatDate, chartOptions);
}

void IBClient::reqIds(int numIds)
{
    socket->reqIds(numIds);
}

void IBClient::reqManagedAccts()
{
    socket->reqManagedAccts();
}

void IBClient::reqMarketDataType(int marketDataType)
{
    socket->reqMarketDataType(marketDataType);
}

void IBClient::reqMktData(TickerId id, const Contract &contract, const IBString &genericTicks, bool snapshot, const TagValueListSPtr& mktDataOptions)
{
    socket->reqMktData(id, contract, genericTicks, snapshot, mktDataOptions);
}

void IBClient::reqMktDepth(TickerId tickerId, const Contract &contract, int numRows, const TagValueListSPtr& mktDepthOptions)
{
    socket->reqMktDepth(tickerId, contract, numRows, mktDepthOptions);
}

void IBClient::reqNewsBulletins(bool allMsgs)
{
    socket->reqNewsBulletins(allMsgs);
}

void IBClient::reqOpenOrders()
{
    socket->reqOpenOrders();
}

void IBClient::reqPositions()
{
    socket->reqPositions();
}

void IBClient::reqRealTimeBars(TickerId id, const Contract &contract, int barSize, const IBString &whatToShow, bool useRTH, const TagValueListSPtr& realTimeBarsOptions)
{
    socket->reqRealTimeBars(id, contract, barSize, whatToShow, useRTH, realTimeBarsOptions);
}

void IBClient::reqScannerParameters()
{
    socket->reqScannerParameters();
}

void IBClient::reqScannerSubscription(int tickerId, const ScannerSubscription &subscription, const TagValueListSPtr& scannerSubscriptionOptions)
{
    socket->reqScannerSubscription(tickerId, subscription, scannerSubscriptionOptions);
}

void IBClient::requestFA(faDataType pFaDataType)
{
    socket->requestFA(pFaDataType);
}

void IBClient::setServerLogLevel(int level)
{
    socket->setServerLogLevel(level);
}

void IBClient::subscribeToGroupEvents(int reqId, int groupId)
{
    socket->subscribeToGroupEvents(reqId, groupId);
}

void IBClient::unsubscribeFromGroupEvents(int reqId)
{
    socket->unsubscribeFromGroupEvents(reqId);
}

void IBClient::updateDisplayGroup(int reqId, const IBString& contractInfo)
{
    socket->updateDisplayGroup(reqId, contractInfo);
}

void IBClient::verifyMessage(const IBString& apiData)
{
    socket->verifyMessage(apiData);
}

void IBClient::verifyRequest(const IBString& apiName, const IBString& apiVersion)
{
    socket->verifyRequest(apiName, apiVersion);
}

/////////////////////////////////////////////
//// Events
/////////////////////////////////////////////

void IBClient::currentTime(long time)
{
    K qtime = kz(zu(time));
    receiveData("currentTime", qtime);
}

void IBClient::error(const int id, const int errorCode, const IBString errorString)
{
    std::string type;
    
    if (1100 <= errorCode && errorCode <= 1300) {
        type = "system";
    } else if (2100 <= errorCode && errorCode <= 2110) {
        type = "warning";
    } else {
        type = "error";
    }
    
    receiveData(type.c_str(), knk(3, kj(id), kj(errorCode), kip(errorString)));
    
    // "Connectivity between IB and TWS has been lost"
    if (id == -1 && errorCode == 1100) {
        disconnect();
    }
    
    // Exception caught while reading socket - Connection reset by peer
    if (id == -1 && errorCode == 509) {
        O("Connection reset by peer\n");
        disconnect();
    }
}

void IBClient::nextValidId(OrderId orderId)
{
    receiveData("nextValidId", kj(orderId));
}

void IBClient::tickPrice(TickerId tickerId, TickType field, double price, int canAutoExecute)
{
    
    receiveData("tickPrice", knk(4, kj(tickerId), ki(field), kf(price), kb(canAutoExecute)));
}

void IBClient::tickSize(TickerId tickerId, TickType field, int size)
{
    receiveData("tickSize", knk(3, kj(tickerId), ki(field), ki(size)));
}

void IBClient::tickOptionComputation(TickerId tickerId, TickType tickType, double impliedVol, double delta, double optPrice, double pvDividend, double gamma, double vega, double theta, double undPrice)
{
    auto dict = createDictionary(std::map<std::string, K> {
        { "tickerId",   kj(tickerId) },
        { "tickType",   ki(tickType) },
        { "impliedVol", kf(impliedVol) },
        { "delta",      kf(delta) },
        { "optPrice",   kf(optPrice) },
        { "pvDividend", kf(pvDividend) },
        { "gamma",      kf(gamma) },
        { "vega",       kf(vega) },
        { "theta",      kf(theta) },
        { "undPrice",   kf(undPrice) }
    });
    receiveData("tickOptionComputation", dict);
}

void IBClient::tickGeneric(TickerId tickerId, TickType tickType, double value)
{
    receiveData("tickGeneric", knk(3, kj(tickerId), ki(tickType), kf(value)));
}

void IBClient::tickString(TickerId tickerId, TickType tickType, const IBString &value)
{
    receiveData("tickString", knk(3, kj(tickerId), ki(tickType), kip(value)));
}

void IBClient::tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const IBString &formattedBasisPoints, double totalDividends, int holdDays, const IBString &futureExpiry, double dividendImpact, double dividendsToExpiry)
{
    auto dict = createDictionary(std::map<std::string, K> {
        { "tickerId",               kj(tickerId) },
        { "tickType",               ki(tickType) },
        { "basisPoints",            kf(basisPoints) },
        { "formattedBasisPoints",   kip(formattedBasisPoints) },
        { "totalDividends",         kf(totalDividends) },
        { "holdDays",               ki(holdDays) },
        { "futureExpiry",           kip(futureExpiry) },
        { "dividendImpact",         kf(dividendImpact) },
        { "dividendsToExpiry",      kf(dividendsToExpiry) }
    });
    receiveData("tickEFP", dict);
}

void IBClient::orderStatus(OrderId orderId, const IBString &status, int filled, int remaining, double avgFillPrice, int permId, int parentId, double lastFillPrice, int clientId, const IBString &whyHeld)
{
    auto dict = createDictionary(std::map<std::string, K> {
        { "id",                 ki(orderId) },
        { "status",             kip(status) },
        { "filled",             ki(filled) },
        { "remaining",          ki(remaining) },
        { "avgFillPrice",       kf(avgFillPrice) },
        { "permId",             ki(permId) },
        { "parentId",           ki(parentId) },
        { "lastFilledPrice",    kf(lastFillPrice) },
        { "clientId",           ki(clientId) },
        { "whyHeld",            kip(whyHeld) }
    });
    receiveData("orderStatus", dict);
}

void IBClient::connectionClosed()
{
    receiveData("connectionClosed", ks((S)""));
}

void IBClient::updateAccountValue(const IBString &key, const IBString &val, const IBString &currency, const IBString &accountName)
{
    receiveData("updateAccountValue", knk(4,
                                          kip(key),
                                          kip(val),
                                          kis(currency),
                                          kis(accountName) ));
}

void IBClient::bondContractDetails(int reqId, const ContractDetails& contractDetails)
{
    receiveData("bondContractDetails", convertBondContractDetails(contractDetails));
}

void IBClient::updatePortfolio(const Contract &contract, int position, double marketPrice, double marketValue, double averageCost, double unrealizedPNL, double realizedPNL, const IBString &accountName)
{
    auto dict = createDictionary(std::map<std::string, K> {
        { "contract",       convertContract(contract) },
        { "position",       ki(position) },
        { "marketPrice",    kf(marketPrice) },
        { "marketValue",    kf(marketValue) },
        { "averageCost",    kf(averageCost) },
        { "unrealizedPNL",  kf(unrealizedPNL) },
        { "realizedPNL",    kf(realizedPNL) },
        { "accountName",    kip(accountName) }
    });
    receiveData("updatePortfolio", dict);
}

void IBClient::updateAccountTime(const IBString &timeStamp)
{
    receiveData("updateAccountTime", kip(timeStamp));
}

void IBClient::updateMktDepth(TickerId id, int position, int operation, int side, double price, int size)
{
    receiveData("updateMktDepth", knk(6,
                                      kj(id),
                                      ki(position),
                                      ki(operation),
                                      ki(side),
                                      kf(price),
                                      ki(size)));
}

void IBClient::updateMktDepthL2(TickerId id, int position, IBString marketMaker, int operation, int side, double price, int size)
{
    receiveData("updateMktDepthL2", knk(7,
                                        kj(id),
                                        ki(position),
                                        kis(marketMaker),
                                        ki(operation),
                                        ki(side),
                                        kf(price),
                                        ki(size)));
}

void IBClient::realtimeBar(TickerId reqId, long time, double open, double high, double low, double close, long volume, double wap, int count)
{
    auto dict = createDictionary(std::map<std::string, K> {
        { "reqId",  kj(reqId) },
        { "time",   kz(zu(time)) },
        { "open",   kf(open) },
        { "high",   kf(high) },
        { "low",    kf(low) },
        { "close",  kf(close) },
        { "volume", kj(volume) },
        { "wap",    kf(wap) },
        { "count",  ki(count) }
    });
    receiveData("realtimeBar", dict);
}

void IBClient::position(const IBString &account, const Contract &contract, int position, double avgCost)
{
    receiveData("position", knk(4,
                                kis(account),
                                convertContract(contract),
                                ki(position),
                                kf(avgCost)));
}

void IBClient::positionEnd()
{
    receiveData("positionEnd", ks((S)""));
}

void IBClient::accountSummary(int reqId, const IBString &account, const IBString &tag, const IBString &value, const IBString &curency)
{
    receiveData("accountSummary", knk(5,
                                      ki(reqId),
                                      kis(account),
                                      kis(tag),
                                      kip(value),
                                      kis(curency)));
}

void IBClient::accountSummaryEnd(int reqId)
{
    receiveData("accountSummaryEnd", ki(reqId));
}

void IBClient::execDetails(int reqId, const Contract &contract, const Execution &execution)
{
    receiveData("execDetails", knk(3,
                                   ki(reqId),
                                   convertContract(contract),
                                   convertExecution(execution)));
}

void IBClient::execDetailsEnd(int reqId)
{
    receiveData("execDetailsEnd", ki(reqId));
}

void IBClient::fundamentalData(TickerId reqId, const IBString &data)
{
    receiveData("fundamentalData", knk(2, kj(reqId), kip(data)));
}

void IBClient::commissionReport(const CommissionReport &commissionReport)
{
    receiveData("commissionReport", convertCommissionReport(commissionReport));
}

void IBClient::tickSnapshotEnd(int reqId)
{
    receiveData("tickSnapshotEnd", ki(reqId));
}

void IBClient::accountDownloadEnd(const IBString &accountName)
{
    receiveData("accountDownloadEnd", kip(accountName));
}

void IBClient::openOrder(OrderId orderId, const Contract &contract, const Order &order, const OrderState &orderState)
{
    receiveData("openOrder", knk(3,
                                 kj(orderId),
                                 convertContract(contract),
                                 convertOrderState(orderState)));
}

void IBClient::openOrderEnd()
{
    receiveData("openOrderEnd", ks((S)""));
}

void IBClient::marketDataType(TickerId reqId, int marketDataType)
{
    receiveData("marketDataType", knk(2, kj(reqId), ki(marketDataType)));
}

void IBClient::historicalData(TickerId reqId, const IBString &date, double open, double high, double low, double close, int volume, int barCount, double WAP, int hasGaps)
{
    auto dict = createDictionary(std::map<std::string, K> {
        { "reqId",  kj(reqId) },
        { "date",   kis(date) }, // TODO: Convert
        { "open",   kf(open) },
        { "high",   kf(high) },
        { "low",    kf(low) },
        { "close",  kf(close) },
        { "volume", ki(volume) },
        { "barCount", ki(barCount) },
        { "WAP",    kf(WAP) },
        { "hasGaps", kb(hasGaps != 0) }
    });
    receiveData("historicalData", dict);
}

void IBClient::scannerParameters(const IBString &xml)
{
    receiveData("scannerParameters", kip(xml));
}

void IBClient::winError(const IBString &str, int lastError)
{
    receiveData("winError", knk(2, kip(str), ki(lastError)));
}

void IBClient::updateNewsBulletin(int msgId, int msgType, const IBString &newsMessage, const IBString &originExch)
{
    receiveData("updateNewsBulletin", knk(4,
                                          ki(msgId),
                                          ki(msgType),
                                          kip(newsMessage),
                                          kip(originExch)));
}

void IBClient::managedAccounts(const IBString &accountsList)
{
    receiveData("managedAccounts", kip(accountsList));
}

void IBClient::deltaNeutralValidation(int reqId, const UnderComp &underComp)
{
    K dict = convertUnderComp(underComp);
    receiveData("deltaNeutralValidation", knk(2, ki(reqId), dict));
}

void IBClient::scannerDataEnd(int reqId)
{
    receiveData("scannerDataEnd", ki(reqId));
}

void IBClient::contractDetails(int reqId, const ContractDetails& contractDetails)
{
    receiveData("orderStatus", knk(2, kj(reqId), convertContractDetails(contractDetails)));
}

void IBClient::contractDetailsEnd(int reqId)
{
    receiveData("contractDetailsEnd", ki(reqId));
}

void IBClient::verifyMessageAPI(const IBString &apiData)
{
    receiveData("verifyMessageAPI", kip(apiData));
}

void IBClient::verifyCompleted(bool isSuccessful, const IBString &errorText)
{
    receiveData("verifyCompleted", knk(2, kb(isSuccessful), kip(errorText)));
}

void IBClient::displayGroupList(int reqId, const IBString &groups)
{
    receiveData("displayGroupList", knk(2, ki(reqId), kip(groups)));
}

void IBClient::displayGroupUpdated(int reqId, const IBString &contractInfo)
{
    receiveData("displayGroupUpdated", knk(2, ki(reqId), kip(contractInfo)));
}
