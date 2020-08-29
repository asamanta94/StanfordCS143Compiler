class A {
	y : Int;
	m(x : Int) : Int {
		x
	};
};

class Main inherits A {
	y : Int;

	main() : SELF_TYPE {
		self
	};

	m(x : String) : Int {
		"Hello"
	};
};