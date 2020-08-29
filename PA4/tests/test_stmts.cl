class Main {
	i : Int <- 0;
	j : Int <- 0;
	main() : Object {
		while { 
			(i + j) < 2;
		} loop {
			i <- i + 1;
			j <- j + 1;
		} pool
	};
};