#include <iostream>
#include <vector>
#include <list>
#include <sys/time.h>
#include <math.h>

#include "map.h"

template <class Container>
void __attribute__ ((noinline)) set(Container& ar, const size_t N)
{
	ar.resize(N);
	for (size_t i = 0; i < N; i++) {
		ar[i] = i;
	}
}

template <typename... Args>
void __attribute__ ((noinline)) bench(const char* name, Args&& ... args)
{
	struct timeval start,end;
	gettimeofday(&start, nullptr);
	auto ret = map(std::forward<Args>(args)...);
	gettimeofday(&end, nullptr);

	const size_t size_data = sizeof(typename decltype(ret)::value_type) * ret.size();
	const double time = (end.tv_sec+end.tv_usec/1000000.0)-(start.tv_sec+start.tv_usec/1000000.0);
	std::cout << *ret.begin() << std::endl;
	std::cout << name << ": " << (time*1000.0) << " ms, BW: " << (2*size_data)/(time*1024.0*1024.0) << " MB/s" << std::endl;
}

inline float compute(const float v)
{
	return sqrt(5.0f*v+1.0f);
}

template <class Container>
void bench_backends(Container const& ar)
{
	bench("seq_no_move",
		compute,
		ar,
		SeqBackendNoMove()
	);

	bench("seq",
		compute,
		ar,
		SeqBackend()
	);

	bench("omp",
		compute,
		ar,
		OMPParallelBackend()
	);

	bench("tbb",
		compute,
		ar,
		TBBParallelBackend()
	);
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
	std::cout << "vector" << std::endl;
	bench_backends(ar);

	std::cout << std::endl << "----" << std::endl << std::endl;

	std::list<float> ar2;
	ar2.resize(N);
	size_t i = 0;
	for (auto it = ar2.begin(); it != ar2.end(); it++) {
		*it = i++;
	}

	std::cout << "list" << std::endl;
	bench("seq_no_move",
		compute,
		ar2,
		SeqBackendNoMove()
	);

	bench("seq",
		compute,
		ar2,
		SeqBackend()
	);

	return 0;
}
