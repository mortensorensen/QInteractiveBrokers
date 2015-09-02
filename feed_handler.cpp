//
//  feed_handler.cpp
//  ib_feed_handler
//
//  Created by Morten Sørensen on 8/25/14.
//  Copyright (c) 2014 Morten Sorensen. All rights reserved.
//

#include "feed_handler.h"

#ifndef __CYGWIN__
#ifdef HAVE_WINSOCK2_H
#include "winsock2.h"
#endif
#endif

#include <twsapi/Execution.h>

#define DBG(x, e) { if(x) O("%s\n", e); }

const std::string    IB_HOST      = "localhost"; // TODO: make as arguments
const int            IB_PORT      = 4001;
const int            IB_CLIENT_ID = 1;

/*
 *********************************************************************************
 * IB Feed Handler
 *********************************************************************************
 */

IBFeedhandler::IBFeedhandler() :
    IBFeedhandler::IBFeedhandler(IB_HOST, IB_PORT, IB_CLIENT_ID)
{
}


IBFeedhandler::IBFeedhandler(const std::string &_ib_host, const int _ib_port,
                             const int _ib_client_id) :
    ib_host(_ib_host), ib_port(_ib_port), ib_client_id(_ib_client_id),
    ib_client(new IB::EPosixClientSocket(this))
{
}

IBFeedhandler::~IBFeedhandler()
{
    if (ib_client) delete ib_client;
    for (auto it = subscriptions.begin(); it != subscriptions.end(); ++it) {
        delete it->second;
    }
}

bool IBFeedhandler::connect(char *host, int port, int client_id)
{
    if (!is_connected())
        ib_client->eConnect(host, port, client_id);

    return ib_client->isConnected();
}

void IBFeedhandler::disconnect()
{
    ib_client->eDisconnect();
}

void IBFeedhandler::subscribe(const int &sid, IB::Contract &contract)
{
//    ib_client->reqMktData(sid, contract, "233", false);     // 233 = include level II trades
    ib_client->reqMktData(sid, contract, "", false);
    subscriptions[sid] = &contract;
}

void IBFeedhandler::unsubscribe(const int &sid)
{
    auto it = subscriptions.find(sid);
    if (it != subscriptions.end()) {
        ib_client->cancelMktData(sid);
        delete subscriptions[sid];
        subscriptions.erase(it);
    }
}

void IBFeedhandler::unsubscribe(const std::string &sym)
{
    for (auto it = subscriptions.begin(); it != subscriptions.end(); ++it) {
        if (it->second->symbol == sym) {
            unsubscribe(it->first);
            break;
        }
    }
}

bool IBFeedhandler::is_connected() const
{
    return ib_client ? ib_client->isConnected() : false;
}

IB::Contract &IBFeedhandler::find_contract(const std::string &symbol)
{
    for (auto it = subscriptions.begin(); it != subscriptions.end(); ++it) {
        if (it->second->symbol == symbol)
            return *(it->second);
    }
    throw; // Handle more elegantly
}

K IBFeedhandler::contract2dict(const IB::Contract &contract)
{
    K keys = ktn(KS, 15);
//    kS(keys)[0] = ss((S)"combolegs");
//    kS(keys)[0] = ss((S)"combolegsdescrib");
    kS(keys)[0] = ss((S)"id");
    kS(keys)[1] = ss((S)"currency");
    kS(keys)[2] = ss((S)"exchange");
    kS(keys)[3] = ss((S)"expiry");
    kS(keys)[4] = ss((S)"includeexpired");
    kS(keys)[5] = ss((S)"localsym");
    kS(keys)[6] = ss((S)"multiplier");
    kS(keys)[7] = ss((S)"primaryexch");
    kS(keys)[8] = ss((S)"right");
    kS(keys)[9] = ss((S)"secid");
    kS(keys)[10] = ss((S)"secidtype");
    kS(keys)[11] = ss((S)"sectype");
    kS(keys)[12] = ss((S)"strike");
    kS(keys)[13] = ss((S)"sym");
    kS(keys)[14] = ss((S)"tradingclass");
    
    K id = ki(contract.conId);
    K currency = ks((S)contract.currency.c_str());
    K exchange = ks((S)contract.exchange.c_str());
    K expiry = ks((S)contract.expiry.c_str());
    K includeexpired = kb(contract.includeExpired);
    K localsym = ks((S)contract.localSymbol.c_str());
    K multiplier = ks((S)contract.multiplier.c_str());
    K primaryexch = ks((S)contract.primaryExchange.c_str());
    K right = ks((S)contract.right.c_str());
    K secid = ks((S)contract.secId.c_str());
    K secidtype = ks((S)contract.secIdType.c_str());
    K sectype = ks((S)contract.secType.c_str());
    K strike = kf(contract.strike);
    K sym = ks((S)contract.symbol.c_str());
    K tradingclass = ks((S)contract.tradingClass.c_str());
    
    K vals = knk(15, id, currency, exchange, expiry, includeexpired, localsym,
                 multiplier, primaryexch, right, secid, secidtype, sectype,
                 strike, sym, tradingclass);
    
    return xD(keys, vals);
}

K IBFeedhandler::order2dict(const IB::Order &order)
{
    K keys = ktn(KS, 8);
    kS(keys)[0] = ss((S)"clientid");
    kS(keys)[1] = ss((S)"orderid");
    kS(keys)[2] = ss((S)"permid");
    kS(keys)[3] = ss((S)"action");
    kS(keys)[4] = ss((S)"totalqty");
    kS(keys)[5] = ss((S)"type");
    kS(keys)[6] = ss((S)"lmtprice");
    kS(keys)[7] = ss((S)"auxprice");
    
    K clientid = ki(order.clientId);
    K orderid = ki(order.orderId);
    K permid = ki(order.permId);
    K action = ks((S)order.action.c_str());
    K totalqty = ki(order.totalQuantity);
    K type = ks((S)order.orderType.c_str());
    K lmtprice = kf(order.lmtPrice);
    K auxprice = kf(order.auxPrice);
    
    K vals = knk(8, clientid, orderid, permid, action,
                 totalqty, type, lmtprice, auxprice);
    
    return xD(keys, vals);
}

K IBFeedhandler::orderstate2dict(const IB::OrderState &state)
{
    K keys = ktn(KS, 9);
    kS(keys)[0] = ss((S)"commission");
    kS(keys)[1] = ss((S)"commissionccy");
    kS(keys)[2] = ss((S)"equitywithloan");
    kS(keys)[3] = ss((S)"initmargin");
    kS(keys)[4] = ss((S)"maintmargin");
    kS(keys)[5] = ss((S)"maxcommission");
    kS(keys)[6] = ss((S)"mincommission");
    kS(keys)[7] = ss((S)"status");
    kS(keys)[8] = ss((S)"warning");
    
    K commission = kf(state.commission);
    K commissionccy = ks((S)state.commissionCurrency.c_str());
    K equitywithloan = ks((S)state.equityWithLoan.c_str());
    K initmargin = ks((S)state.initMargin.c_str());
    K maintmargin = ks((S)state.maintMargin.c_str());
    K maxcommission = kf(state.maxCommission);
    K mincommission = kf(state.minCommission);
    K status = ks((S)state.status.c_str());
    K warning = ks((S)state.warningText.c_str());
    
    K vals = knk(9, commission, commissionccy, equitywithloan, initmargin,
                 maintmargin, maxcommission, mincommission, status, warning);
    
    return xD(keys, vals);
}

K IBFeedhandler::execution2dict(const IB::Execution &exec)
{
    K keys = ktn(KS, 16);
    kS(keys)[0] = ss((S)"orderid");
    kS(keys)[1] = ss((S)"clientid");
    kS(keys)[2] = ss((S)"executionid");
    kS(keys)[3] = ss((S)"time");
    kS(keys)[4] = ss((S)"account");
    kS(keys)[5] = ss((S)"exchange");
    kS(keys)[6] = ss((S)"side");
    kS(keys)[7] = ss((S)"shares");
    kS(keys)[8] = ss((S)"price");
    kS(keys)[9] = ss((S)"permid");
    kS(keys)[10] = ss((S)"liquidation");
    kS(keys)[11] = ss((S)"cumqty");
    kS(keys)[12] = ss((S)"avgprice");
    kS(keys)[13] = ss((S)"orderref");
    kS(keys)[14] = ss((S)"evrule");
    kS(keys)[15] = ss((S)"evmultiplier");
    
    K orderid = ki(exec.orderId);
    K clientid = ki(exec.clientId);
    K execid = ks((S)exec.execId.c_str());
    K time = ks((S)exec.time.c_str());
    K account = ks((S)exec.acctNumber.c_str());
    K exchange = ks((S)exec.exchange.c_str());
    K side = ks((S)exec.side.c_str());
    K shares = ki(exec.shares);
    K price = kf(exec.price);
    K permid = ki(exec.permId);
    K liquidation = ki(exec.liquidation);
    K cumqty = ki(exec.cumQty);
    K avgprice = kf(exec.avgPrice);
    K orderref = ks((S)exec.orderRef.c_str());
    K evrule = ks((S)exec.evRule.c_str());
    K evmultiplier = kf(exec.evMultiplier);
    
    K vals = knk(16, orderid, clientid, execid, time, account, exchange, side,
                 shares, price, permid, liquidation, cumqty, avgprice, orderref,
                 evrule, evmultiplier);
    
    return xD(keys, vals);
}

K IBFeedhandler::commissionreport2dict(const IB::CommissionReport &report)
{
    K keys = ktn(KS, 6);
    kS(keys)[0] = ss((S)"commission");
    kS(keys)[1] = ss((S)"currency");
    kS(keys)[2] = ss((S)"execid");
    kS(keys)[3] = ss((S)"realizedpnl");
    kS(keys)[4] = ss((S)"yield");
    kS(keys)[5] = ss((S)"yieldredemptiondate");
    
    K commission = kf(report.commission);
    K currency = ks((S)report.currency.c_str());
    K execid = ks((S)report.execId.c_str());
    K realizedpnl = kf(report.realizedPNL);
    K yield = kf(report.yield);
    K yieldredemptiondate = ki(report.yieldRedemptionDate);
    
    K vals = knk(6, commission, currency, execid, realizedpnl,
                 yield, yieldredemptiondate);
    
    return xD(keys, vals);
}


/*
 *********************************************************************************
 * IB Callbacks
 *********************************************************************************
 */

void IBFeedhandler::tickPrice(IB::TickerId sid, IB::TickType tick_type, double price, int auto_executable)
{
    K x = knk(4, ki(sid),
              ki(tick_type),
              kf(price),
              kb(auto_executable));
    consume("ontickprice", x);
}

void IBFeedhandler::tickSize(IB::TickerId sid, IB::TickType tick_type, int size)
{
    K x = knk(3, ki(sid),
              ki(tick_type),
              ki(size));
    consume("onticksize", x);
}

void IBFeedhandler::tickString(IB::TickerId sid, IB::TickType tick_type, const IB::IBString &value)
{
    K x = knk(3, ki(sid),
              ki(tick_type),
              kp((S)value.c_str()));
    consume("ontickstring", x);
}

void IBFeedhandler::updateMktDepthL2(IB::TickerId sid, int position, IB::IBString market_maker,
                                     int operation, int side, double price, int size)
{
    K x = knk(7, ki(sid),
              ki(position),
              ks((S)market_maker.c_str()),
              ki(operation),
              ki(side),
              kf(price),
              ki(size));
    consume("ondepth", x);
}

void IBFeedhandler::connectionClosed()
{
    sd0(ib_client->fd());
}

void IBFeedhandler::error(const int id, const int errorCode, const IB::IBString errorString)
{
    K x = knk(3, ki(id), ki(errorCode), ks((S)errorString.c_str()));
    consume("onerror", x);
}

// Receives the next valid Order ID. This method also is called after a
// successful connection to TWS.
void IBFeedhandler::nextValidId(IB::OrderId orderId)
{
    consume("ib.onnextorderid", ki(orderId));
}

// This event is called whenever the status of an order changes.
// It is also fired after reconnecting if the client has any open orders.
void IBFeedhandler::orderStatus(IB::OrderId orderId, const IB::IBString &status,
                int filled, int remaining, double avgFillPrice, int permId,
                int parentId, double lastFillPrice, int clientId,
                const IB::IBString& whyHeld)
{
    K x = knk(10, ki(orderId),
                kp((S)status.c_str()),
                ki(filled),
                ki(remaining),
                kf(avgFillPrice),
                ki(permId),
                ki(parentId),
                kf(lastFillPrice),
                ki(clientId),
                kp((S)whyHeld.c_str()));
    consume("onorderstatus", x);
}

// This method is called to feed in open orders.
void IBFeedhandler::openOrder(IB::OrderId orderId, const IB::Contract& contract,
                              const IB::Order& order, const IB::OrderState& state)
{
    K x = knk(4, ki(orderId), contract2dict(contract),
              order2dict(order), orderstate2dict(state));
    consume("onopenorder", x);
}

void IBFeedhandler::openOrderEnd()
{
    consume("onopenorderend", (K)NULL);
}

// This function receives the current system time on the server side.
void IBFeedhandler::currentTime(long time)
{
    consume("oncurrenttime", ktj(-KP, 8.64e13 * (time / 8.64e4-10957)));
}

// This event is fired when the reqExecutions() functions is invoked, or when an order is filled.
void IBFeedhandler::execDetails(int reqId, const IB::Contract& contract,
                                const IB::Execution& execution)
{
    K x = knk(3, ki(reqId), contract2dict(contract), execution2dict(execution));
    consume("onexecdetails", x);
}

/*
 *********************************************************************************
 * Helper functions
 *********************************************************************************
 */


K IBFeedhandler::consume(const std::string &fun, K x)
{
    K ret = k(0, (S)fun.c_str(), x, (K)0);
    
    if (!ret) {
        O("Broken socket");
    } else if (ret->t == -128) {
        O("Error calling %s: %s\n", fun.c_str(), ret->s);
    }
    
    r0(ret);
    return NULL;
}

K eventloop(I fd)
{
    feed_handler.ib_client->onReceive();
    return NULL;
}

/*
 *********************************************************************************
 * KDB Interface
 *********************************************************************************
 */

extern "C" K connect(K host, K port, K client_id)
{
    if(host->t != -KS || port->t != -KJ || client_id->t != -KJ) return ks((S)"type");
    feed_handler.connect(host->s, port->i, client_id->i);

    if(feed_handler.is_connected()) {
        sd1(feed_handler.ib_client->fd(), eventloop);
        return (K)0;
    }
    return ks((S)"connection");
}

extern "C" K connect_default(K ignore)
{
    K ib_host = ks((S)IB_HOST.c_str());
    K ib_port = kj(IB_PORT);
    K ib_client_id = kj(IB_CLIENT_ID);
    
    return connect(ib_host, ib_port, ib_client_id);
}

extern "C" K disconnect(K ignore)
{
    sd0(feed_handler.ib_client->fd());
    feed_handler.disconnect();
    return 0;
}

extern "C" K subscribe(K sym, K sid)
{
    if (sym->t != -KS || sid->t != -KJ) return ks((S)"type");
    if (!feed_handler.is_connected()) return ks((S)"connection");
    
    IB::Contract *c = new IB::Contract();
    c->symbol = sym->s;
    c->secType = "STK";
    c->exchange = "SMART";
    c->currency = "USD";
    
    feed_handler.subscribe(sid->i, *c);
    return 0;
}

extern "C" K unsubscribe(K sid)
{
    //    DBG(sid->t != -KJ || sid->t != -KS, (S)"type");
    if (!feed_handler.is_connected()) return ks((S)"connection");
    
    if (sid->t == -7) {
        feed_handler.unsubscribe(sid->i);  // Numberic/long input
    } else {
        feed_handler.unsubscribe(sid->s);  // String/symbol input
    }

    return 0;
}

extern "C" K get_subscriptions(K ignore)
{
    if (!feed_handler.is_connected())
        return ks((S)"connection");

    int n = feed_handler.subscriptions.size();
    
    K pk = ktn(KS, 1);
    kS(pk)[0] = ss((S)"sid");
    
    K cols = ktn(KS, 4);
    kS(cols)[0] = ss((S)"sym");
    kS(cols)[1] = ss((S)"sec_type");
    kS(cols)[2] = ss((S)"exchange");
    kS(cols)[3] = ss((S)"currency");
    
    K sid = ktn(KI, n);
    K sym = ktn(KS, n);
    K sec_type = ktn(KS, n);
    K exchange = ktn(KS, n);
    K currency = ktn(KS, n);
    
    int i = 0;
    for (auto it = feed_handler.subscriptions.begin(); it != feed_handler.subscriptions.end(); ++it, ++i) {
        kI(sid)[i] = it->first;
        kS(sym)[i] = ss((S)it->second->symbol.c_str());
        kS(sec_type)[i] = ss((S)it->second->secType.c_str());
        kS(exchange)[i] = ss((S)it->second->exchange.c_str());
        kS(currency)[i] = ss((S)it->second->currency.c_str());
    }
    
    K key = xT(xD(pk, knk(1, sid)));
    K val = xT(xD(cols, knk(4, sym, sec_type, exchange, currency)));
    
    return xD(key, val);
}

extern "C" K is_connected(K ignore)
{
    return kb(feed_handler.is_connected());
}

extern "C" K next_order_id(K num_ids)
{
    feed_handler.ib_client->reqIds(num_ids->i);
    return 0;
}

extern "C" K order(K order_id, K symbol, K action, K qty, K lmt, K order_type, K exchange)
{
    IB::Contract c;
    c.symbol = symbol->s;
    c.exchange = exchange->s;
    c.currency = "USD";
    c.secType = "STK";
    
    IB::Order o;
    o.orderId = order_id->i;
    o.action = action->s;
    o.totalQuantity = qty->j;
    o.orderType = order_type->s;
    o.lmtPrice = lmt->f;
    o.tif = "DAY";
    
    feed_handler.ib_client->placeOrder(order_id->i, c, o);
    return 0;
}

extern "C" K cancel_order(K order_id)
{
    feed_handler.ib_client->cancelOrder(order_id->i);
    return 0;
}

extern "C" K req_current_time(K ignore)
{
    feed_handler.ib_client->reqCurrentTime();
    return 0;
}

// This function should be called frequently (every 1 second) to check for messages received from TWS.
extern "C" K check_messages(K ignore)
{
    feed_handler.ib_client->checkMessages();
    return 0;
}

// Use this function to cancel all open orders globally. It cancels both API and TWS open orders.
// If the order was created in TWS, it also gets canceled. If the order was initiated in the API, it also gets canceled.
extern "C" K req_global_cancel(K ignore)
{
    feed_handler.ib_client->reqGlobalCancel();
    return 0;
}

// Call this function to request the open orders that were placed from this client.
// Each open order will be fed back through the openOrder() and orderStatus() functions on the EWrapper.
extern "C" K req_open_orders(K ignore)
{
    feed_handler.ib_client->reqOpenOrders();
    return 0;
}

// Call this function to request the open orders placed from all clients and also from TWS.
// Each open order will be fed back through the openOrder() and orderStatus() functions on the EWrapper.
extern "C" K req_all_open_orders(K ignore)
{
    feed_handler.ib_client->reqAllOpenOrders();
    return 0;
}

// Call this function to request that newly created TWS orders be implicitly associated with the client.
// When a new TWS order is created, the order will be associated with the client, and fed back through
// the openOrder() and orderStatus() functions on the EWrapper.
extern "C" K req_auto_open_orders(K auto_bind)
{
    feed_handler.ib_client->reqAutoOpenOrders(auto_bind->g == 0);
    return 0;
}

// Returns the version of the TWS instance to which the API application is connected.
extern "C" K server_version(K ignore)
{
    return ki(feed_handler.ib_client->serverVersion());
}

// When this method is called, the execution reports from the last 24 hours that meet
// the filter criteria are downloaded to the client via the execDetails() method.
// To view executions beyond the past 24 hours, open the Trade Log in TWS and, while
// the Trade Log is displayed, request the executions again from the API.
extern "C" K req_executions(K client_id, K account_code, K time,
                            K sym, K sec_type, K exchange, K side)
{
    IB::ExecutionFilter filter;
    filter.m_clientId = client_id->i;
    filter.m_acctCode = account_code->s;
    filter.m_time = time->i;
    filter.m_symbol = sym->s;
    filter.m_secType = sec_type->s;
    filter.m_exchange = exchange->s;
    filter.m_side = side->s;
    
    feed_handler.ib_client->reqExecutions(client_id->i, filter);
    return 0;
}