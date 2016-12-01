#include "IBClient.h"

#include "EPosixClientSocket.cpp"
#include "EPosixClientSocketPlatform.h"
#include "EClientSocketBaseImpl.h"
#include "helpers.h"

#define arraylen(x) (sizeof(x)/(sizeof(*(x))))

/////////////////////////////////////////////
//// Member funcs
/////////////////////////////////////////////

IBClient::IBClient()
    : m_pClient(new EPosixClientSocket(this))
{
}

IBClient::~IBClient()
{
}

bool IBClient::connect(const char *host, unsigned int port, int clientId)
{
    return m_pClient->eConnect(host, port, clientId, false);
}

void IBClient::disconnect() const
{
    sd0(fd());
    m_pClient->eDisconnect();
}

bool IBClient::isConnected() const
{
    return m_pClient->isConnected();
}

int IBClient::fd() const
{
    return m_pClient->fd();
}

void IBClient::onReceive()
{
    m_pClient->onReceive();
}

void IBClient::receiveData(const char *fun, K x)
{
    K r = k(0, (S)".ib.onrecv", ks((S)fun), x, (K)0);
    if (!r) {
        O("Broken socket");
    } else if (r->t == -128) {
        // TODO: Give more details about types
        O("Error calling '%s': %s\n", fun, r->s);
    }
    r0(r);
}

/////////////////////////////////////////////
//// Methods
/////////////////////////////////////////////

void IBClient::reqCurrentTime()
{
    m_pClient->reqCurrentTime();
}

void IBClient::reqMktData(TickerId id, const Contract &contract, const std::string &genericTicks, bool snapshot)
{
    TagValueListSPtr tag;
    m_pClient->reqMktData(id, contract, genericTicks, snapshot, tag);
}

void IBClient::reqAccountUpdates(bool subscribe, const char *acctCode)
{
    m_pClient->reqAccountUpdates(subscribe, acctCode);
}

void IBClient::placeOrder(OrderId id, const Contract &contract, const Order &order)
{
    m_pClient->placeOrder(id, contract, order);
}

void IBClient::cancelOrder(OrderId id)
{
    m_pClient->cancelOrder(id);
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
    const char* cKeys[] = {"tickerId", "tickType", "impliedVol", "delta", "optPrice", "pvDividend", "gamma", "vega",
                           "theta", "undPrice"};
    K keys = ktn(KS, arraylen(cKeys));
    DO(keys->n, kS(keys)[i] = ss((S)cKeys[i]));

    K vals = knk(keys->n,
                 kj(tickerId),
                 ki(tickType),
                 kf(impliedVol),
                 kf(delta),
                 kf(optPrice),
                 kf(pvDividend),
                 kf(gamma),
                 kf(vega),
                 kf(theta),
                 kf(undPrice));

    receiveData("tickOptionComputation", xD(keys, vals));
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
    const char* cKeys[] = {"tickerId", "tickType", "basisPoints", "formattedBasisPoints", "totalDividends",
                              "holdDays", "futureExpiry", "dividendImpact", "dividendsToExpiry"};
    K keys = ktn(KS, arraylen(cKeys));
    DO(keys->n, kS(keys)[i] = ss((S)cKeys[i]));
    
    K vals = knk(keys->n,
                 kj(tickerId),
                 ki(tickType),
                 kf(basisPoints),
                 kp((S)formattedBasisPoints.c_str()),
                 kf(totalDividends),
                 ki(holdDays),
                 kp((S)futureExpiry.c_str()),
                 kf(dividendImpact),
                 kf(dividendsToExpiry));
    
    receiveData("tickEFP", xD(keys, vals));
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
                                          kp((S)currency.c_str()),
                                          kp((S)accountName.c_str()) ));
}

void IBClient::updatePortfolio(const Contract &contract, int position, double marketPrice, double marketValue, double averageCost, double unrealizedPNL, double realizedPNL, const IBString &accountName)
{
    const char* cKeys[] = {"contract", "position", "marketPrice", "marketValue", "averageCost", "unrealizedPNL",
                           "realizedPNL", "accountName"};
    K keys = ktn(KS, arraylen(cKeys));
    DO(keys->n, kS(keys)[i] = ss((S)cKeys[i]));
    
    K vals = knk(keys->n,
                 kj(contract.conId),
                 ki(position),
                 kf(marketPrice),
                 kf(marketValue),
                 kf(averageCost),
                 kf(unrealizedPNL),
                 kf(realizedPNL),
                 kp((S)accountName.c_str() ));
    
    receiveData("updatePortfolio", xD(keys, vals));
}

void IBClient::updateAccountTime(const IBString &timeStamp)
{
    receiveData("updateAccountTime", kp((S)timeStamp.c_str()));
}
