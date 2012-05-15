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

	set<int> a;
	{
		boost::timer::auto_cpu_timer t;
		for (size_t i = 0; i < n; ++i) {
			int el = elements[i];
			a.insert(el);
		}
		for (size_t i = 0; i < n; ++i) {
			int el = other_elements[i];
			a.erase(el);
		}
	}
}

// vim:set ts=4 sw=4 sts=4:
