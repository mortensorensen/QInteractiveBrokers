#ifndef qib_h
#define qib_h

#include "helpers.h"
#include <config.h>
#include "Contract.h"
#include "Order.h"
#include "Execution.h"
#include "ScannerSubscription.h"

extern "C"
{
    K version(K ignore);
    K LoadLibrary(K ignore);
    K connect_(K host, K port, K clientId);
    K disconnect(K ignore);
    K isConnected(K ignore);
    
    K calculateImpliedVolatility(K reqId, K contract, K optionPrice, K underPrice);
    K calculateOptionPrice(K reqId, K contract, K volatility, K underPrice);
    K cancelAccountSummary(K reqId);
    K cancelCalculateImpliedVolatility(K reqId);
    K cancelCalculateOptionPrice(K reqId);
    K cancelFundamentalData(K reqId);
    K cancelHistoricalData(K tickerId);
    K cancelMktData(K id);
    K cancelMktDepth(K tickerId);
    K cancelNewsBulletins(K ignore);
    K cancelOrder(K id) ;
    K cancelPositions(K ignore);
    K cancelRealTimeBars(K  tickerId);
    K cancelScannerSubscription(K tickerId);
    K checkMessages(K ignore);
    K exerciseOptions(K tickerId, K contract, K exerciseAction,  K exerciseQuantity, K account, K override);
    K placeOrder(K id, K contract, K order);
    K queryDisplayGroups( K reqId);
    K replaceFA(K pFaDataType, K cxml);
    K reqAccountSummary(K reqId, K groupName, K tags);
    K reqAccountUpdates(K subscribe, K acctCode);
    K reqAllOpenOrders(K ignore);
    K reqAutoOpenOrders(K bAutoBind);
    K reqContractDetails(K reqId, K contract);
    K reqCurrentTime(K ignore);
    K reqExecutions(K reqId, K filter);
    K reqFundamentalData(K reqId, K contract, K reportType);
    K reqGlobalCancel(K ignore);
    K reqHistoricalData(K id, K contract, K endDateTime, K durationStr, K barSizeSetting, K whatToShow, K useRTH);
    K reqIds(K numIds);
    K reqManagedAccts(K ignore);
    K reqMarketDataType(K marketDataType);
    K reqMktData(K id, K contract, K genericTicks, K snapshot);
    K reqMktDepth(K tickerId, K contract, K numRows, K mktDepthOptions);
    K reqNewsBulletins(K allMsgs);
    K reqOpenOrders(K ignore);
    K reqPositions(K ignore);
    K reqRealTimeBars(K id, K contract, K barSize, K whatToShow, K useRTH, K realTimeBarsOptions);
    K reqScannerParameters(K ignore);
    K reqScannerSubscription(K tickerId, K subscription, K scannerSubscriptionOptions);
    K requestFA(K pFaDataType);
    K serverVersion(K ignore);
    K setServerLogLevel(K level);
    K subscribeToGroupEvents(K reqId, K groupId);
    K TwsConnectionTime(K ignore);
    K unsubscribeFromGroupEvents(K reqId);
    K updateDisplayGroup(K reqId, K contractInfo);
    K verifyMessage(K apiData);
    K verifyRequest(K apiName, K apiVersion);
}

Z Contract createContract(K dict, std::string &error);
Z Order createOrder(K dict, std::string &error);
Z ExecutionFilter createExecutionFilter(K dict, std::string &error);
Z ScannerSubscription createScannerSubscription(K dict, std::string &error);
Z TagValueListSPtr createTagValueList(K dict, std::string &error);

template<typename T> V setList(T &property, G type, K x, I index, std::string &error);
template<typename T> V setProperty(T &property, I expectedType, K x, I index, std::string &error);
V setProperties(K dict, std::map<std::string, std::function<void(Contract&)>> &map, std::string &error);

template<typename F> V setProperties(K dict, F &props, std::string &error);

#endif
