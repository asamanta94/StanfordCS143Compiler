class A {
x <- Int 1;
y <- Int 2;

init(map : String) : SELF_TYPE {
        {
            x <- x + y;
            self;
        }
    };
};