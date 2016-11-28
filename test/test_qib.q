.tst.desc["QIB"]{
	before{
		`so mock `:build/Debug/qib.0.0.1;
		/ loadqib so;
	};
	/ should["load library"]{
	/ 	mustnotthrow[(`loadqib;so)];
	/ };
	should["not be connected"]{
		0b musteq .ib.isConnected[];
	};
	should["disconnect"]{
		mustnotthrow[(`.ib.disconnect;`)];
	};
	should["throw error if requesting but disconnected"]{
		mustthrow[();(`.ib.currenttime;`)];
	};
 };
