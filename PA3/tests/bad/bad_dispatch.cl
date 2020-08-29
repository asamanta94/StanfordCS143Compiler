class Main {
	d(x : Int) : SELF_TYPE {
		self
	};

	c(x : Int) : SELF_TYPE {
		self
	};

	main() : SELF_TYPE {
		{
			m();
			c();
			d("Hello");
			main(1);
		}
	};
};