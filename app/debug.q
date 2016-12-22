qib:.Q.def[enlist[`appdir]!enlist`$"app"] .Q.opt .z.x;
system"l ",string[qib`appdir],"/ib.q"

out"Connecting"
.ib.connect[`$"127.0.0.1";7497;1];
$[.ib.isConnected[]; out"Connected"; [out"Connection failed";exit 1]]

out"Requesting current time"
.ib.reqCurrentTime[]

.ib.nextSubId:1

.ib.format:{ssr[ssr[;"\"";""] .j.j x;",";", "]}

subscribe:{[cont]
	out"Subscribing to ",.ib.format[cont];
	if[count ?[`contract;{(=;x;enlist y)}.'flip(key;value)@\:cont;0b;()];out"Already subscribed";:];
	`contract upsert cont:cont,enlist[`id]!enlist .ib.nextSubId;
	upsert[;select id,sym:symbol from enlist cont] each `trade`quote;
	.ib.reqMktData[cont`id;cont _`id;"";0b];
	.ib.nextSubId+:1;
 };

syms:("SSSS";enlist csv)0:.Q.dd[hsym qib`appdir;`syms.csv]

out"Requesting mkt data"
/ subscribe each syms;

\
.ib.onrecv:{[fname;args] show (fname;args)};
subscribe syms 1


showupd:{
	out"Trades: ",string i`trade;
	out"Quotes: ",string i`quote;
 };

.z.ts:showupd

if[not system"t";system"t 1500"];

out"Placing order"

lmtOrder:`action`totalQuantity`orderType`lmtPrice!(`BUY;1000;`LMT;0.01)
mktOrder:`action`totalQuantity`orderType!(`BUY;1000;`MKT)

.ib.placeOrder[1^.ib.nextId;contract] lmtOrder

.ib.placeOrder[.ib.nextId;contract] mktOrder
.ib.isConnected[]
.ib.connect[`$"127.0.0.1";7497;1];
.ib.reqCurrentTime[]
i

contract
quote
i
.ib.onrecv:{[fname;args] show(fname;args)};
cont:first contract

.ib.reqMktData[4;cont _`conId;"";0b];
