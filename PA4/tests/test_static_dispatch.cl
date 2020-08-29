class A {
	io : IO <- (new IO);
	m(i : Int, j : Int) : Object {
		io.out_int({
			(i * 2) + (j * 2);
		})
	};
};

class B inherits A {
	m(i : Int, j : Int) : Object {
		io.out_int(i + j)
	};
};

class Main {
	main() : Object {
		{
			(new B)@A.m(1, 2);
			(new B)@B.m(1, 2);
		}
	};
};