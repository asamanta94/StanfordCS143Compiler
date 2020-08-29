class A {
	i : Int <- 0;
	b : B <- new B;
	c : String <- "Hello World";
	d : Bool <- true;
	e : Bool;
	f : Bool <- false;
	new_m() : Int { 1 };
	new_m2() : Int { 2 };
};

class B inherits A {
	new_m() : Int { 2 };
};

class C inherits B {
	g : Int;
	h : String;
};

class Main {
    main() : Int {
        1
    };
};
