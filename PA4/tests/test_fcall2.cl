class Math {
	add(i : Int, j : Int) : Int {
		i + j
	};
};

class Main {
	i  : Int <- 0;
	m : Math <- (new Math);
	main() : Object {
		{
			i <- (new Math).add(1, 2);
			(new IO).out_int(i);
		}
	};

	add(i : Int, j : Int) : Int {
		i + j
	};

	sub(i : Int, j : Int) : Int {
		i - j
	};

	mul(i : Int, j : Int) : Int {
		i * j
	};

	div(i : Int, j : Int) : Int {
		i / j
	};
};
