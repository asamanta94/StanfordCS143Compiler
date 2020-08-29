class Main {
	a : Bool <- true;
	b : Bool <- false;
	io : IO <- (new IO);
	main() : Object {
		{
			check(not a);
			check(not b);
		}
	};

	check(b : Bool) : Object {
		if b then {
			io.out_int(1);
		} else {
			io.out_int(0);
		} fi
	};
};