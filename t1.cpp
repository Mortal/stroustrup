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
			size_t j = 0;
			int el = elements[i];
			while (j < i && a[j] < el) ++j;
			copy_backward(a+j, a+i, a+i+1);
			a[j] = el;
		}
		for (size_t i = 0; i < n; ++i) {
			int el = other_elements[i];
			size_t j = 0;
			while (j+i < n && a[j] != el) ++j;
			copy(a+j+1, a+n-i, a+j);
		}
	}
}

// vim:set ts=4 sw=4 sts=4:
