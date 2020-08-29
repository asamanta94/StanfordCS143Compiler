class A {
	a : Bool <- ( 1 < 2 );
	b : Bool <- ( 1 <= 2);
	c : Int <- ( ~ 1 );
	d : Bool <- ( 1 = 2 );
	e : Bool <- ( "Hello" = "Hello" );
	f : Bool <- ( true = false );
	g : Bool <- ( not true );
	h : Bool <- ( not a );
};

class Main {
	main() : SELF_TYPE {
		self
	};
};