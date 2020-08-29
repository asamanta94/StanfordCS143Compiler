
(*
 *  execute "coolc bad.cl" to see the error messages that the coolc parser
 *  generates
 *
 *  execute "myparser bad.cl" to see the error messages that your parser
 *  generates
 *)

(* no error *)
class A {
};

(* error:  b is not a type identifier *)
Class b inherits A {
};

(* error:  a is not a type identifier *)
Class C inherits a {
};

(* error:  keyword inherits is misspelled *)
Class D inherts A {
};

Class E inherits A {
};

(* error: Incomplete feature definition *)
class A {
;
;
;
x : Int <- 1;
};

(* error: Let error *)
class Ba {
	init() : SELF_TYPE {
        let A : A <- 1, B : G, C : H in C
	};
};

(* error: Block error *)
class BB {
	init() : SELF_TYPE {
		{
			{

			};
		}
	};
};