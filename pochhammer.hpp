/**
 * Computes the Pochhammer symbol with caching.
 *
 * @author Parsiad Azimzadeh.
 */

#include <tuple>         // tuple
#include <unordered_map> // unordered_map

#include <gmpxx.h>       // GNU Multiple Precision Arithmetic Library

typedef std::tuple<int, unsigned> pch_key;
typedef std::unordered_map<pch_key, mpq_class> pch_cache;

/**
 * Computes the rising factorial $(m)_n$.
 * @param c A cache.
 * @return The rising factorial in arbitrary precision.
 * @see pch_cache
 */
inline mpq_class pch(int m, unsigned n, pch_cache &c)
{
	// Base-case
	if(n == 0)
	{
		return 1;
	}

	// Query the cache
	{
		auto search = c.find( pch_key(m, n) );
		if(search != c.end()) {
			// Hit
			return search->second;
		}
	}

	// Miss (perform calculation)
	mpq_class p = m * pch(m+1, n-1, c);
	c.emplace( make_pair( pch_key(m, n), p) );
	return p;
}

/**
 * Computes the falling factorial $(-1)^n (-m)_n$.
 * @param c A cache.
 * @return The falling factorial in arbitrary precision.
 * @see pch_cache
 */
inline mpq_class pch_f(int m, unsigned n, pch_cache &c)
{
	return ((n % 2 == 0) ? -1 : 1) * pch(-m, n, c);
}

