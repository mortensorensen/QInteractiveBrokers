qib:.Q.def[enlist[`appdir]!enlist`$"app"] .Q.opt .z.x;
system"l ",string[qib`appdir],"/ib.q"

out"Connecting"
.ib.connect[`$"127.0.0.1";7497;1];
$[.ib.isConnected[]; out"Connected"; [out"Not connected";exit 1]]

out"Requesting current time"
.ib.reqCurrentTime[]

contracts:([]id:1 2 3;symbol:`IBM`AAPL`MSFT;secType:`STK;exchange:`SMART;currency:`USD)

out"Requesting mkt data"
{.ib.reqMktData[x`id;x _`id;"";0b]}each contracts


/ showupd:{
/ 	out"Trades: ",string i`trade;
/ 	out"Quotes: ",string i`quote;
/  };

/ .z.ts:showupd

/ if[not system"t";system"t 1500"];

/ out"Placing order"

/ lmtOrder:`action`totalQuantity`orderType`lmtPrice!(`BUY;1000;`LMT;0.01)
/ mktOrder:`action`totalQuantity`orderType!(`BUY;1000;`MKT)

/ .ib.placeOrder[1^.ib.nextId;contract] lmtOrder

\
.ib.placeOrder[.ib.nextId;contract] mktOrder
.ib.isConnected[]
.ib.connect[`$"127.0.0.1";7497;1];
.ib.reqCurrentTime[]
i
