class A {
	m() : SELF_TYPE {
		self
	};
};

class D {

};

class B inherits A {

};

class X inherits IO {

};

class Main {
	a : A <- new A;
	main() : Object {
		case a of
			q : Int => q;
			b : B => { b.m(); (new IO).out_int(2); };
			a : A => { a.m(); (new IO).out_int(1); };
			c : C => { c.m(); (new IO).out_int(3); };
			x : X => x;
			y : IO => y;
		esac
	};
};

class C inherits B {

};