class A inherits BBBBB {
    position : Int <- 0;
    num : Int <- 100;
    temp : Int;
    count : Int <- 0;
    evolve(num : Int) : SELF_TYPE {
        {
            while position < num loop
                {
                    temp <- temp.concat(cell_at_next_evolution(position));
                    position <- position + 1;
                }
            pool;
            population_map <- temp;
            self;
        }
    };

    call_evolve(num : Int, should_call_after : Bool) : SELF_TYPE {
        {
            while count < num loop
                {
                    evolve(num);
                    count <- count + 1;
                }
            pool;
            
            if should_call_after = true then evolve(num) else self fi;

            self;
        }
    };
};
