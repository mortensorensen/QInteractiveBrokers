out:{-1 string[.z.Z]," ",x;}
zu:{"z"$-10957+x%8.64e4} / kdb+ datetime from unix
format:{ssr[ssr[;"\"";""] .j.j x;",";", "]}

.ib:(`:bin/qib.0.0.1 2:(`LoadLibrary;1))`
.ib.onrecv:{[fname;args] value (enlist $[null func:.ib.callbacks[fname];.ib.unknown;func]),$[type[args] in 10 98 99h;enlist;::] args};
.ib.callbacks:()!()
.ib.unknown:{[fname;args] out" unknown function ",(string fname),", args: ";0N!args}
.ib.reg:{[fname;code] @[`.ib.callbacks;fname;:;code];}
.ib.dreg:{[fname] .ib.callbacks _::fname;}

contract:1!flip`id`symbol`secType`exchange`currency!"issss"$\:()
quote:1!flip`id`sym`time`bid`ask`bidsize`asksize`autoexe!"ispffjjb"$\:()
trade:1!flip`id`sym`time`price`size`autoexe!"ispfjb"$\:()

i:`quote`trade!0 0
.ib.nextId:0Nj
.ib.ready:0b

sym:{contract[x;`symbol]}

updtick:{[tbl;col;val;dict] tbl upsert (enlist[col]!enlist val),dict; i[tbl]+:1;};

/ https://interactivebrokers.github.io/tws-api/tick_types.html
tick:()!()
tick[0]:updtick[`quote;`bidsize]
tick[1]:updtick[`quote;`bid]
tick[2]:updtick[`quote;`ask]
tick[3]:updtick[`quote;`asksize]
tick[4]:updtick[`trade;`price]
tick[5]:updtick[`trade;`size]
tick[6]:{[val;dict] out string[sym dict`id]," high = ",string val}
tick[7]:{[val;dict] out string[sym dict`id]," low = ",string val}
tick[8]:{[val;dict] out string[sym dict`id]," volume = ",string 100*val}
tick[9]:{[val;dict] out string[sym dict`id]," close = ",string val}
tick[45]:{[str;dict] updtick[`trade;`time;;dict] "p"$zu "J"$str} 			/ last timestamp
tick[49]:{[val;dict] out string[sym dict`id]," ",$[1f=val;"halted";"tradable"]}

.ib.reg[`system] {[id;code;msg] out "SYSTEM: ","|" sv string[id,code],enlist msg}
.ib.reg[`warning] {[id;code;msg] out "WARNING: ","|" sv string[id,code],enlist msg}
.ib.reg[`error] {[id;code;msg] out"ERROR: ","|" sv string[id,code],enlist msg}

.ib.reg[`connectionClosed] {[x] out"Connection closed"}

.ib.reg[`currentTime] {out"Current time: ",string x}
.ib.reg[`nextValidId] {
	.ib.nextId::x; out"Next valid ID: ",string x;
	if[not .ib.ready;.ib.ready::1b]; / this is the last event in the login sequence
 };

.ib.reg[`tickPrice] {[id;field;price;ae]
	$[null f:tick[field];
		out"unknown tickPrice: ","|" sv string (id;field;price;ae);
		f[price;`id`time`autoexe!(id;.z.p;ae)]];
 };

.ib.reg[`tickSize] {[id;field;size]
	$[null f:tick[field];
		out"unknown tickSize: ","|" sv string (id;field;size);
		f[size;`id`time!(id;.z.p)]];
 };

.ib.reg[`tickGeneric] {[id;field;val]
	$[null f:tick[field];
		out"unknown tickGeneric: ","|" sv string (id;field;val);
		f[val;enlist[`id]!enlist id]];
 };

.ib.reg[`tickString] {[id;field;str]
	$[null f:tick[field];
		out"unknown tickString: ","|" sv string[(id;field)],enlist str;
		f[str;enlist[`id]!enlist id]];
 };

.ib.reg[`tickEFP] {[dict]
	out"tickEFP - ",format dict
 };

.ib.reg[`orderStatus] {[dict]
	out"orderStatus - ",format dict
 };

.ib.reg[`tickOptionComputation] {[dict]
	out"tickOptionComputation"
 };

.ib.reg[`updateAccountValue] {[updkey;val;ccy;accName]
	out"updateAccountValue - ",format `key`value`currency`accountName!(updkey;val;ccy;accName)
 };

.ib.reg[`updatePortfolio] {[dict]
	out"updatePortfolio - ",format dict
 };

.ib.reg[`updateAccountTime] {[ts]
	out"updateAccountTime - ",string .z.D+"T"$ts
 };

.ib.reg[`updateMktDepth] {[id;position;operation;side;price;size]
	out"updateMktDepth"
 };

.ib.reg[`updateMktDepthL2] {[id;position;marketmaker;operation;side;price;size]
	out"updateMktDepthL2"
 };

.ib.reg[`realtimeBar] {[dict]
	out"realtimeBar - ",format dict
 };

.ib.reg[`position] {[account;contract;position;avgCost]
	out"position - ",format `account`contract`position`avgCost!(account;contract;position;avgCost)
 };

.ib.reg[`positionEnd] {
	out"positionEnd"
 };

.ib.reg[`accountSummary] {[reqId;account;tag;val;ccy]
	out"accountSummary"
 };

.ib.reg[`accountSummaryEnd] {[reqId]
	out"accountSummaryEnd"
 };

.ib.reg[`execDetails] {[reqId;contract;execution]
	out"execDetails:";
	/ -1 format `reqId`contract`execution!(reqId;contract;execution);
 };

.ib.reg[`execDetailsEnd] {[reqId]
	out"execDetailsEnd"
 };

.ib.reg[`fundamentalData] {[reqId;data]
	out"fundamentalData"
 };

.ib.reg[`tickSnapshotEnd] {[reqId]
	out"tickSnapshotEnd"
 };

.ib.reg[`accountDownloadEnd] {[accountName]
	out"accountDownloadEnd - ",accountName
 };

.ib.reg[`openOrder] {[orderId;contract;order;state]
	out"openOrder",format `orderId`contract`order`state!(orderId;contract;order;state)
 };

.ib.reg[`openOrderEnd] {
	out"openOrderEnd"
 };

.ib.reg[`marketDataType] {[reqId;marketDataType]
	out"marketDataType"
 };

.ib.reg[`historicalData] {[dict]
	out"historicalData - ",format dict
 };

.ib.reg[`scannerParameters] {[xml]
	out"scannerParameters"
 };

.ib.reg[`winError] {[str;lastError]
	out"winError"
 };

.ib.reg[`updateNewsBulletin] {[msgId;msgType;newsMessage;originExch]
	out"updateNewsBulletin"
 };

.ib.reg[`managedAccounts] {[accountsList]
	out"managedAccounts: ",accountsList;
	.ib.managedAccounts:`$"," vs accountsList;
 };

.ib.reg[`deltaNeutralValidation] {[reqId;underComp]
	out"deltaNeutralValidation"
 };

.ib.reg[`scannerDataEnd] {[reqId]
	out"scannerDataEnd"
 };

.ib.reg[`contractDetails] {[reqId;dict]
	out"contractDetails - ",format dict
 };

.ib.reg[`contractDetailsEnd] {[reqId]
	out"contractDetailsEnd"
 };

.ib.reg[`verifyMessageAPI] {[apiData]
	out"verifyMessageAPI"
 };

.ib.reg[`verifyCompleted] {[succesful;errorText]
	out"verifyCompleted"
 };

.ib.reg[`displayGroupList] {[reqId;groups]
	out"displayGroupList"
 };

.ib.reg[`displayGroupUpdated] {[reqId;contractInfo]
	out"displayGroupUpdated"
 };
