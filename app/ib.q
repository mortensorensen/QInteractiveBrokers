out:{-1 string[.z.Z]," ",x;}

.ib:(`:build/Debug/qib.0.0.1 2:(`LoadLibrary;1))`
.ib.onrecv:{[fname;args] $[null code:.ib.callbacks[fname];.ib.unknown[fname;args];code . (),args]}
.ib.callbacks:()!()
.ib.unknown:{[fname;args] out" unknown function ",(string fname),", args: ";0N!args}
.ib.reg:{[fname;code] @[`.ib.callbacks;fname;:;code];}
.ib.dreg:{[fname] .ib.callbacks _::fname;}

quote:1!flip`id`sym`time`bid`ask`bidsize`asksize`autoexe!"jspffjjb"$\:()
trade:1!flip`id`sym`time`brokertime`price`size`autoexe!"jspjfjb"$\:()

i:`quote`trade!0 0

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
.ib.reg[`error] {[id;code;msg] '"ERROR: ","|" sv string[id,code],enlist msg}

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

\
/ loadqib:{.ib::(x 2:(`LoadLibrary;1))`}
loadqib `:build/Debug/qib.0.0.1
.ib.connect[`$"127.0.0.1";7497;1]
.ib.connect[`$"127.0.0.1";4001;1]
.ib.disconnect[]
.ib.isConnected[]
.ib.reqCurrentTime[]
.ib.reqMktData[1;`a`b`c!1 2 3;"";0b]

/- dynamically load shared object
so:`feed_handler
connect:so 2:(`connect;3)
disconect:so 2:(`disconnect;1)
connected:so 2:(`is_connected;1)
reqcurrenttime:so 2:(`req_current_time;1)
checkmessages:so 2:(`check_messages;1)
serverversion:so 2:(`server_version;1)

/- API callbacks
/onerror:{.lg.o[`onerror;x]}
onerror:{x}
oncurrenttime:{0N!x; x}
onopenorder:{}
onopenorderend:{}
onorderstatus:{}
onnextorderid:{}

source:`ib

sub:so 2:(`subscribe;2)
unsub:so 2:(`unsubscribe;1)
subscriptions:so 2:(`get_subscriptions;1)

/- get schema
quote:`sid xkey update sid:() from delete time from `. `quote
trade:`sid xkey update sid:() from delete time from `. `trade
/depth:`sid xkey update sid:() from delete time from `. `depth

/- tick types. See https://www.interactivebrokers.com/en/software/api/api.htm
/tickmap: `tick_type xkey ("ISS"; enlist ",") 0:`tick_types.csv
tickmap:([ticktype:()] field:(); table:())
ticktypes: 3 cut (
	0;`bsize;`quote;
	1;`bid;`quote;
	2;`ask;`quote;
	3;`asize;`quote;
	4;`price;`trade;
	5;`size;`trade)
/ 8;`volume;`trade)
`tickmap insert/: ticktypes;

/- custom functions
subscribe:{
	sub[x; nextsid:1+0^exec last sid from subscriptions[]];
	`quote insert update sid:nextsid,sym:x from exec from quote where sid=nextsid;
	`trade insert update sid:nextsid,sym:x from exec from trade where sid=nextsid;}

unsubscribe:{
	@[usub x;
	 delete from `quote where sid=x;
	 delete from `trade where sid=x;];}

consumetick:{
	tt:tickmap x`ticktype;
	if[not all null tt; / ensure tick type is defined
	 vals:![value (!; (`sid`ticktype _x); (); 0b; (enlist tt`field)!enlist`value); (); 0b; enlist`value]; / rename (delete value from update field:value from tbl)
	 ![tbl:` sv `.ib,tt`table; enlist (=;`sid;x`sid); 0b; vals]; / update local quote
	 if[not all null d:first ?[tbl; enlist (=;`sid;x`sid); 0b; ()]; .u.upd[tt`table;raze d]];]} / send to .u.upd if complete


/- API callbacks
ontickprice:consumetick / sid, tick_type, price, auto_executable
onticksize:consumetick / sym, tick_type, value (float)
ontickstring:{-1"ontickstring"; 0N!x} / sym, tick_type, value (string)
ontickstring:{}

/ TODO: If connection to IB lost, try reconnect every 1 minute

init:{
	$[connect[host;port;clientid];
	 [subscribe each syms; .z.ts:checkmessages];
	 .lg.e[`tickerplant;"Could not connect to IB"]]}
init[]
