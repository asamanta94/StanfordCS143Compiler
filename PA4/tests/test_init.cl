class D {

	get_i() : Int { 1 };
};

class B inherits IO {
	a : Int;
	a1 : String;
	a2 : Bool;
	d : D;

	get_a() : Int { { a;} };
};

class Main inherits IO {
	b : B <- new B;
	s : String;
	c : Bool;

	main() : Object {
		{
			out_int(b.get_a());
			out_string("\n");
			out_string(s);
			out_string("\n");
			if c then out_string("True") else out_string("False") fi;
		}
	};
};