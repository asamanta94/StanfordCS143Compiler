class A {
	x : Int <- 0;
	y : Int <- 1;

	add() : Int {
		1 + 2
	};

	sub() : Int {
		1 - 2
	};

	mul() : Int {
		1 * 2
	};

	div() : Int {
		1 / 2
	};

	m() : Int {
		{
			1 + 2;
			1 - 2;
			1 * 2;
			1 / 2;
		}
	};
};

class Main {
	main() : SELF_TYPE {
		self
	};
};