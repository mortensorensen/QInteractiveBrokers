//
//  feed_handler.h
//  ib_feed_handler
//
//  Created by Morten Sørensen on 8/25/14.
//  Copyright (c) 2014 Morten Sorensen. All rights reserved.
//

#ifndef __ib_feed_handler__feed_handler__
#define __ib_feed_handler__feed_handler__

#include <twsapi/EPosixClientSocket.h>
#include <twsapi/EWrapper.h>
#include <twsapi/TwsSocketClientErrors.h>
#include <twsapi/Contract.h>
#include <twsapi/Order.h>
#include <twsapi/OrderState.h>
#include <twsapi/Execution.h>
#include <twsapi/CommissionReport.h>
#include "ib_wrapper.h"
#include "k.h"
#include <string>
#include <map>


class IBFeedhandler;
class DebugWrapper;

class IBFeedhandler : IBWrapper
{
public:
    IBFeedhandler();
    IBFeedhandler(const std::string &_ib_host, const int ib_port, const int _ib_client_id);
    virtual ~IBFeedhandler();

    std::string ib_host;
    int ib_port;
    int ib_client_id;
    IB::EPosixClientSocket *ib_client;
    
    std::map<int, IB::Contract*> subscriptions;
    bool is_connected() const;

    bool connect(char *host, int port, int client_id);
    void disconnect();
    void subscribe(const int &sid, IB::Contract &contract);
    void unsubscribe(const int &sid);
    void unsubscribe(const std::string &sym);
    
    // Callbacks from IBWrapper
    void tickPrice(IB::TickerId sid, IB::TickType tick_type, double price, int auto_executable);
    void tickSize(IB::TickerId sid, IB::TickType tick_type, int size);
    void tickString(IB::TickerId sid, IB::TickType tick_type, const IB::IBString& value);
    void updateMktDepthL2(IB::TickerId sid, int position, IB::IBString market_maker, int operation,
                          int side, double price, int size);
    void connectionClosed();
    void error(const int id, const int errorCode, const IB::IBString errorString);
    void nextValidId(IB::OrderId orderId);
    void orderStatus(IB::OrderId orderId, const IB::IBString &status,
                     int filled, int remaining, double avgFillPrice, int permId,
                     int parentId, double lastFillPrice, int clientId,
                     const IB::IBString& whyHeld);
    void openOrder(IB::OrderId orderId, const IB::Contract&,
                   const IB::Order&, const IB::OrderState&);
    void openOrderEnd();
    void currentTime(long time);
    void execDetails(int reqId, const IB::Contract& contract,
                     const IB::Execution& execution);
//    void contractDetails(int reqId, const IB::ContractDetails& contractDetails);


    K consume(const std::string &fun, K x);
    
private:
    IB::Contract &find_contract(const std::string &symbol);
    K contract2dict(const IB::Contract &contract);
    K order2dict(const IB::Order &order);
    K orderstate2dict(const IB::OrderState &state);
    K execution2dict(const IB::Execution &exec);
    K commissionreport2dict(const IB::CommissionReport &report);
};


IBFeedhandler feed_handler;


#endif /* defined(__ib_feed_handler__feed_handler__) */
