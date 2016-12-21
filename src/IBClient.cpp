#include "IBClient.h"

#include "EPosixClientSocket.cpp"
#include "EPosixClientSocketPlatform.h"
#include "EClientSocketBaseImpl.h"
#include "helpers.h"

/////////////////////////////////////////////
//// Member funcs
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

/////////////////////////////////////////////
//// Methods
/////////////////////////////////////////////

void IBClient::reqCurrentTime()
{
    socket->reqCurrentTime();
}

void IBClient::reqMktData(TickerId id, const Contract &contract, const std::string &genericTicks, bool snapshot)
{
    TagValueListSPtr tag;
    socket->reqMktData(id, contract, genericTicks, snapshot, tag);
}

void IBClient::reqAccountUpdates(bool subscribe, const char *acctCode)
{
    socket->reqAccountUpdates(subscribe, acctCode);
}

void IBClient::placeOrder(OrderId id, const Contract &contract, const Order &order)
{
    socket->placeOrder(id, contract, order);
}

void IBClient::cancelOrder(OrderId id)
{
    socket->cancelOrder(id);
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
    
    receiveData(type.c_str(), knk(3, kj(id), kj(errorCode), kp((S)errorString.c_str())));
    
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
    receiveData("tickString", knk(3, kj(tickerId), ki(tickType), kp((S)value.c_str())));
}

void IBClient::tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const IBString &formattedBasisPoints, double totalDividends, int holdDays, const IBString &futureExpiry, double dividendImpact, double dividendsToExpiry)
{
    auto dict = createDictionary(std::map<std::string, K> {
        { "tickerId",               kj(tickerId) },
        { "tickType",               ki(tickType) },
        { "basisPoints",            kf(basisPoints) },
        { "formattedBasisPoints",   kp((S)formattedBasisPoints.c_str()) },
        { "totalDividends",         kf(totalDividends) },
        { "holdDays",               ki(holdDays) },
        { "futureExpiry",           kp((S)futureExpiry.c_str()) },
        { "dividendImpact",         kf(dividendImpact) },
        { "dividendsToExpiry",      kf(dividendsToExpiry) }
    });
    receiveData("tickEFP", dict);
}

void IBClient::orderStatus(OrderId orderId, const IBString &status, int filled, int remaining, double avgFillPrice, int permId, int parentId, double lastFillPrice, int clientId, const IBString &whyHeld)
{
    auto dict = createDictionary(std::map<std::string, K> {
        { "id",                 ki(orderId) },
        { "status",             kp((S)status.c_str()) },
        { "filled",             ki(filled) },
        { "remaining",          ki(remaining) },
        { "avgFillPrice",       kf(avgFillPrice) },
        { "permId",             ki(permId) },
        { "parentId",           ki(parentId) },
        { "lastFilledPrice",    kf(lastFillPrice) },
        { "clientId",           ki(clientId) },
        { "whyHeld",            kp((S)whyHeld.c_str()) }
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
                                          kp((S)key.c_str()),
                                          kp((S)val.c_str()),
                                          ks((S)currency.c_str()),
                                          ks((S)accountName.c_str()) ));
}

void IBClient::updatePortfolio(const Contract &contract, int position, double marketPrice, double marketValue, double averageCost, double unrealizedPNL, double realizedPNL, const IBString &accountName)
{
    auto dict = createDictionary(std::map<std::string, K> {
        { "contract",       kj(contract.conId) },
        { "position",       ki(position) },
        { "marketPrice",    kf(marketPrice) },
        { "marketValue",    kf(marketValue) },
        { "averageCost",    kf(averageCost) },
        { "unrealizedPNL",  kf(unrealizedPNL) },
        { "realizedPNL",    kf(realizedPNL) },
        { "accountName",    kp((S)accountName.c_str()) }
    });
    receiveData("updatePortfolio", dict);
}

void IBClient::updateAccountTime(const IBString &timeStamp)
{
    receiveData("updateAccountTime", kp((S)timeStamp.c_str()));
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
                                        ks((S)marketMaker.c_str()),
                                        ki(operation),
                                        ki(side),
                                        kf(price),
                                        ki(size)));
}

void IBClient::realtimeBar(TickerId reqId, long time, double open, double high, double low, double close, long volume, double wap, int count)
{
    auto dict = createDictionary(std::map<std::string, K> {
        { "reqId",  kj(reqId) },
        { "time",   kj(time) }, // TODO: Convert
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
                                ks((S)account.c_str()),
                                kj(contract.conId),
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
                                      ks((S)account.c_str()),
                                      ks((S)tag.c_str()),
                                      kp((S)value.c_str()),
                                      ks((S)curency.c_str())));
}

void IBClient::accountSummaryEnd(int reqId)
{
    receiveData("accountSummaryEnd", ki(reqId));
}

void IBClient::execDetails(int reqId, const Contract &contract, const Execution &execution)
{
    auto exec = createDictionary(std::map<std::string, K> {
        { "execId",         ks((S)execution.execId.c_str()) },
        { "time",           kp((S)execution.time.c_str()) }, // TODO: Convert
        { "acctNumber",     ks((S)execution.acctNumber.c_str()) },
        { "exchange",       ks((S)execution.exchange.c_str()) },
        { "side",           ks((S)execution.side.c_str()) },
        { "shares",         ki(execution.shares) },
        { "price",          kf(execution.price) },
        { "permId",         ki(execution.permId) },
        { "clientId",       kj(execution.clientId) },
        { "orderId",        kj(execution.orderId) },
        { "liquidation",    ki(execution.liquidation) },
        { "cumQty",         ki(execution.cumQty) },
        { "avgPrice",       kf(execution.avgPrice) },
        { "evRule",         kp((S)execution.evRule.c_str()) },
        { "evMultiplier",   kf(execution.evMultiplier) }
    });
    receiveData("execDetails", knk(3, ki(reqId), kj(contract.conId), exec));
}

void IBClient::execDetailsEnd(int reqId)
{
    receiveData("execDetailsEnd", ki(reqId));
}

void IBClient::fundamentalData(TickerId reqId, const IBString &data)
{
    receiveData("fundamentalData", knk(2, kj(reqId), kp((S)data.c_str())));
}

void IBClient::commissionReport(const CommissionReport &commissionReport)
{
    auto dict = createDictionary(std::map<std::string, K> {
        { "commission",             kf(commissionReport.commission) },
        { "currency",               ks((S)commissionReport.currency.c_str()) },
        { "execId",                 ks((S)commissionReport.execId.c_str()) },
        { "realizedPNL",            kf(commissionReport.realizedPNL) },
        { "yield",                  kf(commissionReport.yield) },
        { "yieldRedemptionDate",    kp((S)stringFormat("%i", commissionReport.yieldRedemptionDate).c_str()) }
    });
    receiveData("commissionReport", dict);
}

void IBClient::tickSnapshotEnd(int reqId)
{
    receiveData("tickSnapshotEnd", ki(reqId));
}

void IBClient::accountDownloadEnd(const IBString &accountName)
{
    receiveData("accountDownloadEnd", kp((S)accountName.c_str()));
}

void IBClient::openOrder(OrderId orderId, const Contract &contract, const Order &order, const OrderState &orderState)
{
    auto state = createDictionary(std::map<std::string, K> {
        { "commission",     kf(orderState.commission) },
        { "commissionCurrency", ks((S)orderState.commissionCurrency.c_str()) },
        { "equityWithLoan", kp((S)orderState.equityWithLoan.c_str()) },
        { "initMargin",     kp((S)orderState.initMargin.c_str()) },
        { "maintMargin",    kp((S)orderState.maintMargin.c_str()) },
        { "maxCommission",  kf(orderState.maxCommission) },
        { "minCommission",  kf(orderState.minCommission) },
        { "status",         kp((S)orderState.status.c_str()) },
        { "warningText",    kp((S)orderState.warningText.c_str()) }
    });
    receiveData("openOrder", knk(3,
                                 kj(orderId),
                                 kj(contract.conId),
                                 state));
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
        { "date",   ks((S)date.c_str()) },
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
    receiveData("scannerParameters", kp((S)xml.c_str()));
}

void IBClient::winError(const IBString &str, int lastError)
{
    receiveData("winError", knk(2, kp((S)str.c_str()), ki(lastError)));
}

void IBClient::updateNewsBulletin(int msgId, int msgType, const IBString &newsMessage, const IBString &originExch)
{
    receiveData("updateNewsBulletin", knk(4,
                                          ki(msgId),
                                          ki(msgType),
                                          kp((S)newsMessage.c_str()),
                                          kp((S)originExch.c_str())));
}

void IBClient::managedAccounts(const IBString &accountsList)
{
    receiveData("managedAccounts", knk(1, kp((S)accountsList.c_str())));
}

void IBClient::deltaNeutralValidation(int reqId, const UnderComp &underComp)
{
    auto dict = createDictionary(std::map<std::string, K> {
        { "conId",  kj(underComp.conId) },
        { "delta",  kf(underComp.delta) },
        { "price",  kf(underComp.price) }
    });
    receiveData("deltaNeutralValidation", knk(2, ki(reqId), dict));
}

void IBClient::scannerDataEnd(int reqId)
{
    receiveData("scannerDataEnd", ki(reqId));
}

void IBClient::contractDetailsEnd(int reqId)
{
    receiveData("contractDetailsEnd", ki(reqId));
}

void IBClient::verifyMessageAPI(const IBString &apiData)
{
    receiveData("verifyMessageAPI", kp((S)apiData.c_str()));
}

void IBClient::verifyCompleted(bool isSuccessful, const IBString &errorText)
{
    receiveData("verifyCompleted", knk(2, kb(isSuccessful), kp((S)errorText.c_str())));
}

void IBClient::displayGroupList(int reqId, const IBString &groups)
{
    receiveData("displayGroupList", knk(2, ki(reqId), kp((S)groups.c_str())));
}

void IBClient::displayGroupUpdated(int reqId, const IBString &contractInfo)
{
    receiveData("displayGroupUpdated", knk(2, ki(reqId), kp((S)contractInfo.c_str())));
}
