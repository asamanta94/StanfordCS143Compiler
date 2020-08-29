class Math {
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

	neg(i : Int) : Int {
		~ i
	};
};

class Main {
	io : IO <- (new IO);

	(* Test variables *)
	a : Int <- 10;
	b : Int <- 11;
	c : Int <- 12;

	main() : Object {
		{
			math_test();
			let_test();
		}
	};

	let_test() {
		io.out_int(a);
		io.out_int(b);
		io.out_int(c);
		let a : Int <- 0, b : Int <- 1, c : Int <- 2 in {
			if (a = 10) then out_string("Check 1 Failed.")
		}
	};

	math_test() : Object {
		{
			let a : Int, b : Int <- 10, c : Int <- 20, m : Math <- (new Math) in {
				a <- m.add(b, c);
				io.out_int(a);
				io.out_string("\n");

				a <- m.sub(b, c);
				io.out_int(a);
				io.out_string("\n");

				a <- m.mul(b, c);
				io.out_int(a);
				io.out_string("\n");

				a <- m.div(c, b);
				io.out_int(a);
				io.out_string("\n");
			};
		}
	};
};