class A {
	m() : A {
		new A
	};

	q() : SELF_TYPE {
		new SELF_TYPE
	};
};

class Main {
	main() : SELF_TYPE {
		self
	};
};