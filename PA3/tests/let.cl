class A {
	x : Int;
	z : Int;
	m() : SELF_TYPE {
		{
			let x : A <- new A, c : A <- x, y : Int <- z in 1;
			x;
			z;
			(* let y : Int in 2; *)
		}

	};
};

class Main {
	main() : SELF_TYPE {
		self
	};	
};