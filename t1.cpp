#include <vector>
#include <iostream>
#include <set>
#include <sstream>
#include <algorithm>
#include <err.h>
#include <boost/random.hpp>
#include <boost/timer/timer.hpp>

using namespace std;

int main(int argc, char ** argv) {
	if (argc < 2) return errx(1, "Usage: %s n", argv[0]), 1;

	boost::mt19937 r;

	size_t n;
	stringstream(argv[1]) >> n;

	vector<int> elements(n);
	generate(elements.begin(), elements.end(), r);

	vector<int> other_elements(elements);
	shuffle(other_elements.begin(), other_elements.end(), r);

	int * a = new int[n];
	{
		boost::timer::auto_cpu_timer t;
		for (size_t i = 0; i < n; ++i) {
			int el = elements[i];
			int * j = lower_bound(a+0, a+i, el);
			// shift [j, i) elements to [j+1, i+1)
			copy_backward(j, a+i, a+i+1);
			*j = el;
		}
		for (size_t i = 0; i < n; ++i) {
			int el = other_elements[i];
			int * j = lower_bound(a+0, a+n-i, el);
			// shift [j+1, n-i) elements to [j, n-i-1)
			copy(j+1, a+n-i, j);
		}
	}
}

// vim:set ts=4 sw=4 sts=4:
