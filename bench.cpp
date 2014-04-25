#include <iostream>
#include <vector>
#include <list>
#include <numeric>
#include <chrono>
#include <cmath>

#include "map.h"
#include "string.h"

template <class Container>
void set(Container& ar, const size_t N)
{
	ar.clear();
	for (size_t i = 0; i < N; i++) {
		ar.push_back(i);
	}
}

template <typename... Args>
void __attribute__ ((noinline)) bench(const char* name, Args&& ... args)
{
	std::chrono::system_clock clock;
	auto start = clock.now();
	auto ret = map(std::forward<Args>(args)...);
	auto stop = clock.now();

	const size_t size_data = sizeof(typename decltype(ret)::value_type) * ret.size();
	const double time = (stop - start).count();

	std::cout << name << ": " << (time / 1000) << " ms, BW: " << (2*size_data * 10e6)/(time*1024.0*1024.0) << " MB/s" << std::endl;
}

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " n" << std::endl;
		return 1;
	}

	const size_t N = atoll(argv[1]);

	std::vector<float> ar1;
	set(ar1, N);
	bench("vector", [](float v) { return 2.0f*v+4.0f; }, ar1);

	std::list<float> ar2;
	set(ar2, N);
	bench("list", [](float v) { return 2.0f*v+4.0f; }, ar2);

	std::vector<String> strs;
	for (size_t i = 0; i < N; i++) {
		strs.emplace_back(String("my little poney"));
	}
	bench("str", [](String const& s) { return std::move(s.to_upper()); }, strs);

	return 0;
}
