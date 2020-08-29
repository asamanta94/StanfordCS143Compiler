class A {
	int_var : Int <- 1;
	bl_var : Boolean;
	str_var : String <- "Hello World";

    num_cells() : Int {
    	{
     		int_var <- 2;
     		bl_var <- true;
     		str_var;
     		int_var <- 2 + 3;
    		int_var <- 2 - 3;
    		int_var <- 2 * 3;
    		int_var <- 2 / 3;
    	}
    };
};