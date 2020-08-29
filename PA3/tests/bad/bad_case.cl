class Main {
	main() : SELF_TYPE {
		case 1 of
			a : Int => 1;
			b : Int => 2;
			b : Int => 2;
			self : SELF_TYPE => 2;
		esac
	};
};