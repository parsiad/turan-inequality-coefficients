/**
 * Computes the coefficients of the Turan-type power series.
 *
 * @author Parsiad Azimzadeh
 */

#include <stdlib.h>      // atoi

#include <iostream>      // cerr, cout
#include <memory>        // unique_ptr
#include <sstream>       // stringstream
#include <thread>        // thread
#include <utility>       // make_pair

#include <gmpxx.h>       // GNU Multiple Precision Arithmetic Library

#include "hash_combine.hpp"
#include "pochhammer.hpp"

using namespace std;

void task(unsigned start, unsigned step, unsigned end, bool *fail,
		stringstream *ss)
{
	// TODO: Store results in another format

	// Thread-local cache (TODO: explore asynchronous structure)
	pch_cache c;

	for(unsigned S = start; S <= end; S+=step)
	{
		for(unsigned m = 2; m < S; m++) // m = S is trivially zero
		{
			// Log
			*ss << "S=" << S << "\tm=" << m << "\t";

			// Calculate the d-th power series coefficient
			for(unsigned d = 1; d <= 2*m - 2; d++)
			{
				mpq_class sum;
				sum = 0;
				for(unsigned k = 0; k <= d; k++)
				{
					sum +=
						  (d-k+1) * (k+1) * pch_f(m-1,d-k,c) * pch_f(m-1,k,c)
								/ ( pch(2-m+S,d-k,c) * pch(2-m+S,k,c) )
						- (d-k+1) * (d-k+2) * pch_f(m-2,d-k,c) * pch_f(m,k,c)
								/ ( 2 * pch(1-m+S,k,c) * pch(3-m+S,d-k,c) )
					;
				}

				// Make sure the coefficient is nonnegative
				if(sum < 0)
				{
					*fail = true;
				}

				// Log
				*ss << sum << " ";
			}

			// Log
			*ss << endl;
		}
	}
}

int main(int argc, char **argv)
{
	// Bounds to check
	unsigned S_min, S_max;
	{
		int tmp_min, tmp_max;
		if( argc != 3 || (tmp_min = atoi(argv[1])) < 2
				|| (tmp_max = atoi(argv[2])) < tmp_min )
		{
			cerr << "usage: coefficients S_MIN S_MAX (where 2 <= S_MIN <= S_MAX)"
					<< endl;
			return 1;
		}
		S_min = (unsigned) tmp_min;
		S_max = (unsigned) tmp_max;
	}

	// Number of threads to make
	unsigned n = thread::hardware_concurrency();

	// Launch threads
	unique_ptr<bool[]> fails(new bool[n]);
	unique_ptr<stringstream[]> ss(new stringstream[n]);
	unique_ptr<unique_ptr<thread>[]> threads(new unique_ptr<thread>[n-1]);
	for(unsigned i = 0; i < n-1; i++)
	{
		fails[i] = false;
		threads[i] = unique_ptr<thread>(
				new thread {task, S_min+i, n, S_max, &fails[i],
				&ss[i]} );
	}

	// Main thread
	fails[n-1] = false;
	task(S_min+(n-1), n, S_max, &fails[n-1], &ss[n-1]);

	// TODO: Main thread should also poll every once in a while to see if
	//       execution should stop

	// Join and check
	bool fail = false;
	for(unsigned i = 0; i < n-1; i++)
	{
		threads[i]->join();
		if(fails[i])
		{
			fail = true;
			break;
		}
	}

	// Main thread check
	if(fails[n-1])
	{
		fail = true;
	}

	// Write to stdout
	cout << (fail ? "one or more coefficients negative" :
			"all coefficients nonnegative") << endl << endl;
	for(unsigned i = 0; i < n; i++)
	{
		cout << ss[i].str();
	}

	return 0;
}

