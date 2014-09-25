/**
 * Computes the coefficients of the Turan-type power series
 * $${}_2F_1(2,-m+1;S-m+2;-x){}^2 - {}_2F_1(1,-m;S-m+1;-x) {}_2F_1(3,-m+2;S-m+3;-x) = \sum_{n=0}^{\infty} \varphi_n x^n$$
 *
 * @author Parsiad Azimzadeh
 */

#include <stdlib.h>      // atoi

#include <iostream>      // cerr, cout
#include <memory>        // unique_ptr
#include <mutex>         // mutex
#include <sstream>       // stringstream
#include <string>        // string
#include <thread>        // thread
#include <utility>       // make_pair

#include <gmpxx.h>       // GNU Multiple Precision Arithmetic Library

#include "hash_combine.hpp"
#include "pochhammer.hpp"

using namespace std;

void task(unsigned start, unsigned step, unsigned end, bool *fail,
		ostream *os, mutex *os_mutex)
{
	typedef unsigned long Z;
	typedef mpq_class Q;

	// TODO: replace this with Cliff Click's hashtable and share it across
	//       threads
	pch_cache c;

	stringstream ss;

	for(Z S = start; S <= end; S+=step)
	{
		for(Z m = 2; m < S; m++) // m = S is trivially zero
		{
			// Log
			ss << "S=" << S << "\tm=" << m << "\t";

			// Calculate the d-th power series coefficient
			for(Z d = 1; d <= 2*m - 2; d++)
			{
				Q sum(0);

				// k = 0 (slow)
				//sum += (d+1) * (
				//	          pch_f(m-1,d,c) / (     pch(2-m+S,d,c) )
				//	- (d+2) * pch_f(m-2,d,c) / ( 2 * pch(3-m+S,d,c) )
				//);

				// k = 0 (fast)
				sum += (d+1) * (
					pch_f(m-2,d-1,c) / pch(3-m+S,d-1,c)
					* ( (m-1)/Q(2-m+S) - (d+2)*(m-1-d)/(2*Q(2-m+S+d)) )
				);

				// 0 < k < d
				for(Z k = 1; k < d; k++)
				{
					// (slow)
					//sum += (d-k+1) * (
					//	  (  k+1) * pch_f(m-1,d-k,c) * pch_f(m-1,k,c) / (     pch(2-m+S,d-k,c) * pch(2-m+S,k,c) )
					//	- (d-k+2) * pch_f(m-2,d-k,c) * pch_f(m  ,k,c) / ( 2 * pch(3-m+S,d-k,c) * pch(1-m+S,k,c) )
					//);

					// (fast)
					sum += (d-k+1) * (
						pch_f(m-2,d-k-1,c) * pch_f(m-1,k-1,c) / ( pch(3-m+S,d-k-1,c) * pch(2-m+S,k-1,c) )
						* ( (k+1)*(m-1)*(m-k)/Q((2-m+S)*(1-m+S+k)) - (d-k+2)*(m-1-d+k)*m/Q(2*(2-m+S+d-k)*(1-m+S)) )
					);
				}

				// k = d (slow)
				//sum +=
				//	(d+1) * pch_f(m-1,d,c) / pch(2-m+S,d,c)
				//	      - pch_f(m  ,d,c) / pch(1-m+S,d,c)
				//;

				// k = d (fast)
				sum +=
					pch_f(m-1,d-1,c) / pch(2-m+S,d-1,c)
					* ( (d+1)*(m-d)/Q(1-m+S+d) - m/Q(1-m+S) )
				;

				// Make sure the coefficient is nonnegative
				if(sum < 0) { *fail = true; }

				// Log
				ss << sum << " ";
			}

			// Log
			ss << endl;
		}

		// Try to flush
		if(os_mutex->try_lock())
		{
			(*os) << ss.str();
			os_mutex->unlock();
			ss.str(string());
		}
	}

	// Flush remainder
	os_mutex->lock();
	(*os) << ss.str();
	os_mutex->unlock();
}

int main(int argc, char **argv)
{
	// Bounds to check
	unsigned S_min, S_max;
	{
		int tmp_min, tmp_max;
		if(
			argc != 3
			|| (tmp_min = atoi(argv[1])) < 2
			|| (tmp_max = atoi(argv[2])) < tmp_min
		)
		{
			cerr << "usage: coefficients S_MIN S_MAX (where 2 <= S_MIN <= S_MAX)" << endl;
			return 1;
		}
		S_min = (unsigned) tmp_min;
		S_max = (unsigned) tmp_max;
	}

	// Number of threads to make
	unsigned n = thread::hardware_concurrency();

	// Mutex
	mutex os_mutex;

	// Launch threads
	unique_ptr<bool[]> fails(new bool[n]);
	unique_ptr<unique_ptr<thread>[]> threads(new unique_ptr<thread>[n-1]);
	for(unsigned i = 0; i < n-1; i++)
	{
		fails[i] = false;
		threads[i] = unique_ptr<thread>(new thread {
			task,
			S_min+i, n, S_max,
			&fails[i],
			&cout, &os_mutex
		});
	}

	// Main thread
	fails[n-1] = false;
	task(
		S_min+(n-1), n, S_max,
		&fails[n-1],
		&cout, &os_mutex
	);

	// Join
	for(unsigned i = 0; i < n-1; i++) { threads[i]->join(); }

	// Check
	cout << endl;
	for(unsigned i = 0; i < n; i++)
	{
		if(fails[i])
		{
			cout << "one or more coefficients negative" << endl;
			return 0;
		}
	}

	cout << "all coefficients nonnegative" << endl;
	return 0;
}

