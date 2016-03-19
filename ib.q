\d .ib

system"l ",getenv[`KDBCODE],"/common/ib.q"

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