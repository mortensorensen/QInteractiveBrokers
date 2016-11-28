qib:.Q.def[enlist[`appdir]!enlist`$"/Users/morten/Sandbox/algotrading/feeds/qib/app"] .Q.opt .z.x;
system"l ",string[qib`appdir],"/ib.q"

out"Connecting"
.ib.connect[`$"127.0.0.1";7497;1];
$[.ib.isConnected[]; out"Connected"; [out"Not connected";exit 1]]

out"Requesting current time"
.ib.reqCurrentTime[]

contract:`symbol`secType`exchange`currency!(`AAPL;`STK;`SMART;`USD)

out"Requesting mkt data"
.ib.reqMktData[1;contract;"";0b]

showupd:{
	out"Trades: ",string i`trade;
	out"Quotes: ",string i`quote;
 };

.z.ts:showupd

if[not system"t";system"t 1500"];

\
/ loadqib:{.ib::(x 2:(`LoadLibrary;1))`}
loadqib `:build/Debug/qib.0.0.1
.ib.connect[`$"127.0.0.1";7497;1]
.ib.connect[`$"127.0.0.1";4001;1]
.ib.disconnect[]
.ib.isConnected[]
.ib.reqCurrentTime[]
.ib.reqMktData[1;`a`b`c!1 2 3;"";0b]
