#pragma once

namespace griddle
{
	template<typename I> // not correct for negative m
	inline I modulo(I x, I m){ return ((x%m) + m) % m; }

	template<typename I>
	I gcd(I a, I b)
	{
		I c;
		while (b != 0)
		{
			c = b;
			b = modulo(a, b);
			a = c;
		}
		return a;
	}

	template<typename I>
	I lcm(I a, I b)
	{
		return a*b / gcd(a, b);
	}

	template<typename I>
	I egcd(I a, I b, I& a_coeff, I& b_coeff)
	{
		a_coeff = 1;
		b_coeff = 0;
		I gcd = a;
		I next_a_coeff = 0;
		I next_b_coeff = 1;
		I next_gcd = b;
		while (next_gcd != 0)
		{
			I q = gcd / next_gcd;
			I temp = gcd - q * next_gcd;
			gcd = next_gcd;
			next_gcd = temp;
			temp = a_coeff - q * next_a_coeff;
			a_coeff = next_a_coeff;
			next_a_coeff = temp;
			temp = b_coeff - q * next_b_coeff;
			b_coeff = next_b_coeff;
			next_b_coeff = temp;
		}
		return gcd;
	}
}

