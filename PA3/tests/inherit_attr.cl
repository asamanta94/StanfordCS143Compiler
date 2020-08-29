class C {
	x : Int <- 0;
	init(y : Int) : C {
		self
	};
};

class D inherits C {
	x : Int <- 0;
	init(z: Int) : SELF_TYPE {
		self
	};
};

class Main {
	main() : SELF_TYPE {
		self
	};
};