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
