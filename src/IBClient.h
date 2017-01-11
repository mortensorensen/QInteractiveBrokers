#ifndef IBClient_h
#define IBClient_h

#include "EWrapper.h"
#include "helpers.h"
#include "Contract.h"
#include "Order.h"
#include "Execution.h"
#include "ScannerSubscription.h"

class EPosixClientSocket;

class IBClient : public EWrapper
{
public:
    IBClient();
    ~IBClient();
    
public:
    bool connect(const char * host, unsigned int port, int clientId = 0);
    void disconnect() const;
    bool isConnected() const;
    
    int fd() const;
    void onReceive();
    
    std::auto_ptr<EPosixClientSocket> socket;
    
private:
    void receiveData(const char *fun, K x);
    K convertContract(const Contract &contract);
    K convertContractDetails(const ContractDetails &contract);
    K convertBondContractDetails(const ContractDetails &contract);
    K convertExecution(const Execution &execution);
    K convertCommissionReport(const CommissionReport &report);
    K convertUnderComp(const UnderComp &comp);
    K convertOrderState(const OrderState &orderState);
    
public:
    // Methods
    bool checkMessages();
    IBString TwsConnectionTime();
    int serverVersion();
    void calculateImpliedVolatility(TickerId reqId, const Contract &contract, double optionPrice, double underPrice);
    void calculateOptionPrice(TickerId reqId, const Contract &contract, double volatility, double underPrice);
    void cancelAccountSummary( int reqId);
    void cancelCalculateImpliedVolatility(TickerId reqId);
    void cancelCalculateOptionPrice(TickerId reqId);
    void cancelFundamentalData(TickerId reqId);
    void cancelHistoricalData(TickerId tickerId );
    void cancelMktData(TickerId id);
    void cancelMktDepth(TickerId tickerId);
    void cancelNewsBulletins();
    void cancelOrder(OrderId id);
    void cancelPositions();
    void cancelRealTimeBars(TickerId tickerId );
    void cancelScannerSubscription(int tickerId);
    void exerciseOptions(TickerId tickerId, const Contract &contract, int exerciseAction, int exerciseQuantity, const IBString &account, int override);
    void placeOrder(OrderId id, const Contract &contract, const Order &order);
    void queryDisplayGroups( int reqId);
    void replaceFA(faDataType pFaDataType, const IBString& cxml);
    void reqAccountSummary( int reqId, const IBString& groupName, const IBString& tags);
    void reqAccountUpdates(bool subscribe, const IBString& acctCode);
    void reqAllOpenOrders();
    void reqAutoOpenOrders(bool bAutoBind);
    void reqContractDetails(int reqId, const Contract &contract);
    void reqCurrentTime();
    void reqExecutions(int reqId, const ExecutionFilter& filter);
    void reqFundamentalData(TickerId reqId, const Contract&, const IBString& reportType);
    void reqGlobalCancel();
    void reqHistoricalData(TickerId id, const Contract &contract, const IBString &endDateTime, const IBString &durationStr, const IBString & barSizeSetting, const IBString &whatToShow, int useRTH, int formatDate, const TagValueListSPtr& chartOptions);
    void reqIds(int numIds);
    void reqManagedAccts();
    void reqMarketDataType(int marketDataType);
    void reqMktData(TickerId id, const Contract &contract, const IBString &genericTicks, bool snapshot, const TagValueListSPtr& mktDataOptions);
    void reqMktDepth(TickerId tickerId, const Contract &contract, int numRows, const TagValueListSPtr& mktDepthOptions);
    void reqNewsBulletins(bool allMsgs);
    void reqOpenOrders();
    void reqPositions();
    void reqRealTimeBars(TickerId id, const Contract &contract, int barSize, const IBString &whatToShow, bool useRTH, const TagValueListSPtr& realTimeBarsOptions);
    void reqScannerParameters();
    void reqScannerSubscription(int tickerId, const ScannerSubscription &subscription, const TagValueListSPtr& scannerSubscriptionOptions);
    void requestFA(faDataType pFaDataType);
    void setServerLogLevel(int level);
    void subscribeToGroupEvents( int reqId, int groupId);
    void unsubscribeFromGroupEvents( int reqId);
    void updateDisplayGroup( int reqId, const IBString& contractInfo);
    void verifyMessage( const IBString& apiData);
    void verifyRequest( const IBString& apiName, const IBString& apiVersion);
    
public:
    // Events
    void accountDownloadEnd(const IBString& accountName);
    void accountSummary( int reqId, const IBString& account, const IBString& tag, const IBString& value, const IBString& curency);
    void accountSummaryEnd( int reqId);
    void bondContractDetails(int reqId, const ContractDetails& contractDetails);
    void commissionReport( const CommissionReport& commissionReport);
    void connectionClosed();
    void contractDetails(int reqId, const ContractDetails& contractDetails);
    void contractDetailsEnd(int reqId);
    void currentTime(long time);
    void deltaNeutralValidation(int reqId, const UnderComp& underComp);
    void displayGroupList( int reqId, const IBString& groups);
    void displayGroupUpdated( int reqId, const IBString& contractInfo);
    void error(const int id, const int errorCode, const IBString errorString);
    void execDetails(int reqId, const Contract& contract, const Execution& execution);
    void execDetailsEnd(int reqId);
    void fundamentalData(TickerId reqId, const IBString& data);
    void historicalData(TickerId reqId, const IBString& date, double open, double high, double low, double close, int volume, int barCount, double WAP, int hasGaps);
    void managedAccounts(const IBString& accountsList);
    void marketDataType(TickerId reqId, int marketDataType);
    void nextValidId(OrderId orderId);
    void openOrder(OrderId orderId, const Contract&, const Order&, const OrderState&);
    void openOrderEnd();
    void orderStatus(OrderId orderId, const IBString &status, int filled, int remaining, double avgFillPrice, int permId, int parentId, double lastFillPrice, int clientId, const IBString& whyHeld);
    void position( const IBString& account, const Contract& contract, int position, double avgCost);
    void positionEnd();
    void realtimeBar(TickerId reqId, long time, double open, double high, double low, double close, long volume, double wap, int count);
    void receiveFA(faDataType pFaDataType, const IBString& cxml) {};
    void scannerData(int reqId, int rank, const ContractDetails &contractDetails, const IBString &distance, const IBString &benchmark, const IBString &projection, const IBString &legsStr) {};
    void scannerDataEnd(int reqId);
    void scannerParameters(const IBString &xml);
    void tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const IBString& formattedBasisPoints, double totalDividends, int holdDays, const IBString& futureExpiry, double dividendImpact, double dividendsToExpiry);
    void tickGeneric(TickerId tickerId, TickType tickType, double value);
    void tickOptionComputation( TickerId tickerId, TickType tickType, double impliedVol, double delta, double optPrice, double pvDividend, double gamma, double vega, double theta, double undPrice);
    void tickSize(TickerId tickerId, TickType field, int size);
    void tickSnapshotEnd(int reqId);
    void tickString(TickerId tickerId, TickType tickType, const IBString& value);
    void updateAccountTime(const IBString& timeStamp);
    void updateAccountValue(const IBString& key, const IBString& val, const IBString& currency, const IBString& accountName);
    void updateMktDepth(TickerId id, int position, int operation, int side, double price, int size);
    void updateMktDepthL2(TickerId id, int position, IBString marketMaker, int operation, int side, double price, int size);
    void updateNewsBulletin(int msgId, int msgType, const IBString& newsMessage, const IBString& originExch);
    void updatePortfolio(const Contract& contract, int position, double marketPrice, double marketValue, double averageCost, double unrealizedPNL, double realizedPNL, const IBString& accountName);
    void verifyCompleted( bool isSuccessful, const IBString& errorText);
    void verifyMessageAPI( const IBString& apiData);
    void winError(const IBString &str, int lastError);
    void tickPrice(TickerId tickerId, TickType field, double price, int canAutoExecute);
};

#endif
