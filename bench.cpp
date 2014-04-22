#include <iostream>
#include <vector>
#include <list>
#include <sys/time.h>
#include <math.h>

#include "map.h"

template <class Container>
void set(Container& ar, const size_t N)
{
	ar.resize(N);
	for (size_t i = 0; i < N; i++) {
		ar[i] = i;
	}
}


int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " n" << std::endl;
		return 1;
	}

	const size_t N = atoll(argv[1]);

	std::vector<float > ar;
	set(ar, N);

    struct timeval start,end;
    gettimeofday(&start, nullptr);

	std::vector<float> ret = map(
		[](int i) { return 2.0f*i+4.0f; },
		ar
	);

    gettimeofday(&end, nullptr);

    const size_t size_data = sizeof(typename decltype(ret)::value_type) * ret.size();
    const double time = (end.tv_sec+end.tv_usec/1000000.0)-(start.tv_sec+start.tv_usec/1000000.0);
    std::cout << *ret.begin() << std::endl;
    std::cout << "perf" << ": " << (time*1000.0) << " ms, BW: " << (2*size_data)/(time*1024.0*1024.0) << " MB/s" << std::endl;

#if 0
	for (float v: ret) {
		std::cout << v << ",";
	}
	std::cout << std::endl;
#endif

	return 0;
}
