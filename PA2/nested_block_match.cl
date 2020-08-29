class A {
x : Int <- 1;
y : Int <- 2;

init() : SELF_TYPE {
        {
            x <- x + y;
            {
            	x <- x + x;
            	x <- x - y;
            	x <- y * y;
            	x <- y / y;
            };
        }
    };
};