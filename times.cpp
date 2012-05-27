// The three contenders:
#include <vector>
#include <set>
#include <list>

#include <queue>
#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <algorithm>
#include <err.h>
#include <random>
#include <chrono>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Find first element in container greater than or equal to a given key.
// (Similar to std::lower_bound.)
///////////////////////////////////////////////////////////////////////////////
template <typename Container, typename T>
typename Container::iterator find(Container & c, const T & value);

// For vectors, we perform binary search.
template <typename T>
typename vector<T>::iterator find(vector<T> & c, const T & value) {
	return lower_bound(c.begin(), c.end(), value);
}

// For sets, we perform binary tree search.
template <typename T>
typename set<T>::iterator find(set<T> & c, const T & value) {
	return c.lower_bound(value);
}

// For lists, we perform linear search.
template <typename T>
typename list<T>::iterator find(list<T> & c, const T & value) {
	for (auto i = c.begin(); i != c.end(); ++i)
		if (!(*i < value)) return i;
	return c.end();
}

///////////////////////////////////////////////////////////////////////////////
// This is the algorithm we are timing.
//
// Given a sequence of keys to insert and a sequence of keys to erase:
//
// * First, we insert all the keys in the given insert order
// * Next, we iterate through the container to verify that the reported order
//   is actually sorted (penalises the red/black tree)
// * Finally, we erase keys in the erase order
//
// The function init_aux sets up insert_keys and erase_keys.
///////////////////////////////////////////////////////////////////////////////
template <typename Container>
inline void go(vector<int> & insert_keys, vector<int> & erase_keys, Container & data, size_t n) {
	// Insert elements in insert order
	for (size_t i = 0; i < n; ++i) {
		int el = insert_keys[i];
		typename Container::iterator j = find(data, el);
		data.insert(j, el);
	}

	// Verify that the reported sequence is sorted
	auto i = adjacent_find(data.begin(), data.end(), std::greater<typename Container::value_type>());
	if (i != data.end()) cout << "Oh noes!" << endl;

	// Erase elements in erase order
	for (size_t i = 0; i < n; ++i) {
		int el = erase_keys[i];
		typename Container::iterator j = find(data, el);
		data.erase(j);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Helpers for timed runs.
///////////////////////////////////////////////////////////////////////////////

enum datastructure {
	Set,
	List,
	Vector
};

typedef chrono::high_resolution_clock Clock;
typedef Clock::time_point Time;
typedef Clock::duration Duration;
typedef pair<Duration, pair<datastructure, size_t> > contestant;

vector<int> insert_keys;
vector<int> erase_keys;

void init_aux(size_t n) {
	if (insert_keys.size() == n) return;

	mt19937 r(n);

	insert_keys.resize(n);
	iota(insert_keys.begin(), insert_keys.end(), 0);
	erase_keys = insert_keys;

	shuffle(insert_keys.begin(), insert_keys.end(), r);
	shuffle(erase_keys.begin(), erase_keys.end(), r);
}

template <typename Container>
contestant go_timed(size_t n, Container & data, datastructure ds) {
	init_aux(n);
	Duration time;
	try {
		Time begin = Clock::now();
		go(insert_keys, erase_keys, data, n);
		Time end = Clock::now();
		time = end-begin;
	} catch (bad_alloc) {
		time = Duration(numeric_limits<Duration::rep>::max());
	}
	return make_pair(time, make_pair(ds, n));
}

contestant go_set(size_t n) {
	set<int> data;
	return go_timed(n, data, Set);
}

contestant go_vector(size_t n) {
	vector<int> data;
	data.reserve(n);
	return go_timed(n, data, Vector);
}

contestant go_list(size_t n) {
	list<int> data;
	return go_timed(n, data, List);
}


///////////////////////////////////////////////////////////////////////////////
/// Figure out how many decimals are needed to represent a duration with the
/// given period as a fixed precision decimal value.
///////////////////////////////////////////////////////////////////////////////

// Residue is the ratio between denominator and numerator
template <intmax_t Residue>
struct calc_decimals;

// Residue = 0 means the den. is greater than the num.
template <>
struct calc_decimals<0> {
	static const size_t value = 0;
};

// Residue = 1 means the den. is equal to the num.
template <>
struct calc_decimals<1> {
	static const size_t value = 0;
};

// Residue > 1 means we have decimals to represent
template <intmax_t Residue>
struct calc_decimals {
	static const size_t value = 1+calc_decimals<Residue/10>::value;
};

template <typename Period>
struct decimals {
	static const size_t value = calc_decimals<Period::den/Period::num>::value;
};


int main() {
	priority_queue<contestant, vector<contestant>, greater<contestant> > contestants;
	contestants.push(go_set(10));
	contestants.push(go_vector(10));
	contestants.push(go_list(10));

	while (!contestants.empty()) {
		contestant least = contestants.top();
		contestants.pop();
		size_t n = least.second.second*13/10+1;
		switch (least.second.first) {
			case Set:
				cout << "set     n = " << setw(9) << n << "  t = " << flush;
				least = go_set(n);
				break;
			case Vector:
				cout << "vector  n = " << setw(9) << n << "  t = " << flush;
				least = go_vector(n);
				break;
			case List:
				cout << "list    n = " << setw(9) << n << "  t = " << flush;
				least = go_list(n);
				break;
		}
		if (least.first.count() == numeric_limits<Duration::rep>::max()) {
			cout << "insufficient memory" << endl;
		} else {
			cout << fixed << setprecision(decimals<Duration::period>::value) << static_cast<double>(least.first.count())*Duration::period::num/Duration::period::den << endl;
			contestants.push(least);
		}
	}

	return 1;
}

// vim:set ts=4 sw=4 sts=4:
