#pragma once

namespace griddle
{
	template<typename I>
	inline I modulo(I x, I m){ return ((x%m) + m) % m; }
}