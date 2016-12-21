.tst.desc["QIB"]{
	before{
		`so mock `:build/Debug/qib.0.0.1;
		`fname mock `f;
	};
	should["not be connected"]{
		0b musteq .ib.isConnected[];
	};
	should["disconnect"]{
		mustnotthrow[(`.ib.disconnect;`)];
	};
	should["throw error if requesting but disconnected"]{
		mustthrow[();(`.ib.currenttime;`)];
	};
	should["call known functions with an atom"]{
		`args mock 5;
		.ib.reg[fname] {2*x};
		.ib.onrecv[fname;args] musteq 10;
	};
	should["call known functions with a simple list"]{
		`args mock 5 10;
		.ib.reg[fname] {x+y};
		.ib.onrecv[fname;args] musteq 15;
	};
	should["call known functions with a mixed list"]{
		`args mock (`MSFT;100f);
		.ib.reg[fname] {[x;y] (x;y)};
		.ib.onrecv[fname;args] musteq args;
	};
	should["call known functions with a dict"]{
		`args mock `a`b`c!1 2 3;
		.ib.reg[fname] {sum x};
		.ib.onrecv[fname;args] musteq 6;
	};
	should["call known functions with a table"]{
		`args mock ([]a:1 2 3;b:4 5 6);
		.ib.reg[fname] {sum x};
		.ib.onrecv[fname;args] musteq `a`b!6 15;
	};
 };
