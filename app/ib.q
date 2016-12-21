out:{-1 string[.z.Z]," ",x;}
zu:{"z"$-10957+x%8.64e4}

.ib:(`:build/Debug/qib.0.0.1 2:(`LoadLibrary;1))`
.ib.onrecv:{[fname;args] if[null func:.ib.callbacks[fname];func:.ib.unknown];value $[type[args] in 98 99h;::;raze] (func;args)};
.ib.callbacks:()!()
.ib.unknown:{[fname;args] out" unknown function ",(string fname),", args: ";0N!args}
.ib.reg:{[fname;code] @[`.ib.callbacks;fname;:;code];}
.ib.dreg:{[fname] .ib.callbacks _::fname;}

contract:1!flip`conId`symbol`secType`exchange`currency!"issss"$\:()
quote:1!flip`id`sym`time`bid`ask`bidsize`asksize`autoexe!"ispffjjb"$\:()
trade:1!flip`id`sym`time`price`size`autoexe!"ispfjb"$\:()

i:`quote`trade!0 0
.ib.nextId:0Nj

sym:contract[;`symbol]

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
.ib.reg[`nextValidId] {.ib.nextId::x; out"Next valid ID: ",string x}

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
	out"tickEFP"
 };

.ib.reg[`orderStatus] {[dict]
	out"orderStatus"
 };

.ib.reg[`tickOptionComputation] {[dict]
	out"tickOptionComputation"
 };

.ib.reg[`updateAccountValue] {[updkey;val;ccy;accName]
	out"updateAccountValue"
 };

.ib.reg[`updatePortfolio] {[dict]
	out"updatePortfolio"
 };

.ib.reg[`updateAccountTime] {[ts]
	out"updateAccountTime - ",string zu "J"$ts
 };

.ib.reg[`openOrder] {[dict]
	out"openOrder"
 };

.ib.reg[`updateMktDepth] {[id;position;operation;side;price;size]
	out"updateMktDepth"
 };

.ib.reg[`updateMktDepthL2] {[id;position;marketmaker;operation;side;price;size]
	out"updateMktDepthL2"
 };

.ib.reg[`realtimeBar] {[dict]
	out"realtimeBar"
 };

.ib.reg[`position] {[account;conId;position;avgCost]
	out"position"
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

.ib.reg[`execDetails] {[reqId;contractId;execution]
	out"execDetails"
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
	out"accountDownloadEnd"
 };

.ib.reg[`openOrder] {[orderId;contractId;state]
	out"openOrder"
 };

.ib.reg[`openOrderEnd] {
	out"openOrderEnd"
 };

.ib.reg[`marketDataType] {[reqId;marketDataType]
	out"marketDataType"
 };

.ib.reg[`historicalData] {[dict]
	out"historicalData"
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
	out"managedAccounts: ",accountsList
 };

.ib.reg[`deltaNeutralValidation] {[reqId;underComp]
	out"deltaNeutralValidation"
 };

.ib.reg[`scannerDataEnd] {[reqId]
	out"scannerDataEnd"
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
