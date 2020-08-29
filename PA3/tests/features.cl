class C {
	x : Int <- 0;
	init() : C {
		self
	};	
};

class Main {
	main : Int <- 0;

	main() : C {
		(new C).init()
	};

	init(x : Int) : SELF_TYPE {
		self
	};
};