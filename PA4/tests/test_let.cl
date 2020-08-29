class Main {
	z : Int <- 0;
	io : IO <- new IO;
	main() : Object {
		{
			z <- let x : Int <- 1, y : Int <- 2 in x <- x + y;
			io.out_int(z);
			z <- let x : Int <- 9, y : Int <- 10, z : Int <- 11 in x <- x + y + z;
			io.out_int(z);

			z <- let x : Int <- 9, y : Int <- 10, z : Int <- 11 in {
				x <- x + y + z;
				z <- let x : Int <- 3 in {
					io.out(x);
				};
			};
		}
	};
};