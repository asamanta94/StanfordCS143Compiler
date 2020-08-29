class C {
	y : Int;
	m() : SELF_TYPE {
		self
	};
};

class B inherits C {
	y : Bool;
	m() : String {
		1
	};
};

class A inherits B {
	y : String;
	m() : Int {
		1
	};
};

class Main {
	main() : SELF_TYPE {
		self
	};
};