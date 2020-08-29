class A {
	m() : SELF_TYPE {
		self
	};	
};

class B inherits A {
	a() : Int {
		1
	};
};

class Main {
	c(x : Int, y : String, z : A) : Int {
		1
	};

	main() : Int {
		{
			(new A).m();
			(new B).m();
			(new B).a();
			c(1, "HELLO", new A);
		}
	};
};