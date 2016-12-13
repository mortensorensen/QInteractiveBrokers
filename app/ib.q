out:{-1 string[.z.Z]," ",x;}

.ib:(`:build/Debug/qib.0.0.1 2:(`LoadLibrary;1))`
.ib.onrecv:{[fname;args] if[null func:.ib.callbacks[fname];func:.ib.unknown];eval enlist[func],args};
.ib.callbacks:()!()
.ib.unknown:{[fname;args] out" unknown function ",(string fname),", args: ";0N!args}
.ib.reg:{[fname;code] @[`.ib.callbacks;fname;:;code];}
.ib.dreg:{[fname] .ib.callbacks _::fname;}

quote:1!flip`id`sym`time`bid`ask`bidsize`asksize`autoexe!"jspffjjb"$\:()
trade:1!flip`id`sym`time`brokertime`price`size`autoexe!"jspjfjb"$\:()

i:`quote`trade!0 0
.ib.nextId:0Nj

tickmap:1!flip `field`table`column!flip 3 cut (
	0;`quote;`bidsize;
  1;`quote;`bid;
  2;`quote;`ask;
  3;`quote;`asksize;
  4;`trade;`price;		/ last
  5;`trade;`size			/ last size
  / 6;`quote;`high;
  / 6;`quote;`low;
  / 8;`trade;`volume;
  / 9;`quote;`close
  );

.ib.reg[`system] {[id;code;msg] out "SYSTEM: ","|" sv string[id,code],enlist msg}
.ib.reg[`warning] {[id;code;msg] out "WARNING: ","|" sv string[id,code],enlist msg}
.ib.reg[`error] {[id;code;msg] out"ERROR: ","|" sv string[id,code],enlist msg}

.ib.reg[`connectionClosed] {[x] out"Connection closed"}

.ib.reg[`currentTime] {out"Current time: ",string x}
.ib.reg[`nextValidId] {.ib.nextId::x; out"Next valid ID: ",string x}

/ https://www.interactivebrokers.com.hk/en/software/api/apiguide/tables/tick_types.htm

.ib.reg[`tickPrice] {[id;field;price;ae]
	/ out"tickPrice: ","|" sv string (id;field;price;ae);
	tm[`table] upsert (`id`time,(tm:tickmap field)[`column],`autoexe)!(id;.z.p;price;ae);
	i[tm`table]+:1;
 };

.ib.reg[`tickSize] {[id;field;size]
	/ out"tickSize: ","|" sv string (id;field;size);
	tm[`table] upsert (`id`time,(tm:tickmap field)[`column])!(id;.z.p;size);
	i[tm`table]+:1;
 };

.ib.tickGenericMap:()!()
.ib.tickGenericMap[49]:{[id;val]
	/ halted
	out"halted: ",string[id],"|",$[1f=val;"halted";"not halted"]
 };

.ib.reg[`tickGeneric] {[id;field;val]
	if[null f:.ib.tickGenericMap field;
		out"tickGeneric: ","|" sv string[(id;field;val)];
		f[id;val]]
 };

.ib.tickStringMap:()!()
.ib.tickStringMap[45]:{[id;str]
	/ last timestamp
	out"last timestamp: ",string[id],"|",str
 };

.ib.reg[`tickString] {[id;field;str]
	if[null f:.ib.tickStringMap field;
		out"tickString: ","|" sv string[(id;field)],str;
		f[id;str]]
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
	out"updateAccountTime"
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
