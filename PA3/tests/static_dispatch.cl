class A {
	m(x : Int, y : Int) : Int {
		x - y
	};
};

class B inherits A {
	m(x : Int, y : Int) : Int {
		x - y
	};
};

class Main {
	c() : Int {
		1
	};

	main() : Int {
		{
			(new A)@A.m(1, 2);
			self@Main.c();
			self@SELF_TYPE.c();
		}
	};
};
