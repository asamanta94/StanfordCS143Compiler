class U {

};

class V inherits U {

};

class W inherits V {

};

class X inherits W {

};

class Y inherits V {

};

class A {
	a : Y;
	b : X;
	m() : Object {
		if true then a else b fi
	};
};

class Main {
	main() : SELF_TYPE {
		self
	};
};