class A {
	x : Int <- 0;
	y : Int <- 1;

	function(num : Int) : SELF_TYPE {
		case num of a : Int => x <- 2; b : Int => x <- 3; esac
	};
};

class B {

};
