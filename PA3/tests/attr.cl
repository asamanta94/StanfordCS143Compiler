class C {

};

class D {
	b : SELF_TYPE;
	get_b() : SELF_TYPE {
		b
	};
};

class B inherits C {
	b : SELF_TYPE;
	c : D <- (new D).get_b();
};

class A inherits B {
	a : B;
	-- d : D <- d.get_b();
	-- e : D;
	m() : C {
		(* a <- d *)
		(* {
			e <- d.get_b();
			a <- b;
		} *)
		{
			a <- b;
			b;
		}
	};
};

Class Main {
	main() : SELF_TYPE {
		self
	};
};