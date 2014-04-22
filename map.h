#ifndef MAP_H
#define MAP_H

#include <type_traits>
#include <vector>
#include <array>
#include <tbb/parallel_for.h>

class Backend
{ };

class SeqBackendNoMove: Backend
{ };

class SeqBackend: Backend
{ };

class OMPParallelBackend: Backend
{ };

class TBBParallelBackend: Backend
{ };

template <class Container>
struct is_random_accessible_container
{
	static constexpr bool value = false;
	//static constexpr bool value = (std::iterator_traits<typename Container::const_iterator>::iterator_category == std::random_access_iterator_tag);
};


#if 0
struct ParallelBackend: Backend
{
	ParallelBackend(size_t ntokens):
		_ntokens(ntokens)
	{ }

public:
	size_t ntokens() const { return _ntokens; }

private:
	size_t _ntokens;
};

struct ParallelBackendGrain: public ParallelBackend
{
	using ParallelBackend::ParallelBackend;
};
#endif

template <class T>
struct known_backend
{
	static constexpr bool value = std::is_base_of<Backend, T>::value;
};

template <class Container, class Func, class Backend>
typename std::enable_if<known_backend<Backend>::value == false, Container>::type map(Func const& f, Container const& c, Backend const& backend)
{
	static_assert(known_backend<Backend>::value == true, "Unknown backend");
}

template<typename T>
struct HasReserve
{
	template<typename U, size_t (U::*)() const> struct SFINAE {};
	template<typename U> static char Test(SFINAE<U, &U::reserve>*);
	template<typename U> static int Test(...);
	static constexpr bool value = sizeof(Test<T>(0)) == sizeof(char);
};

template <class Container>
void map_reserve(Container& c, typename std::enable_if<HasReserve<Container>::value == true, size_t>::type s)
{
	c.reserve(s);
}

template <class Container>
void map_reserve(Container&, typename std::enable_if<HasReserve<Container>::value == false, size_t>::type)
{
}

template <class Container>
void map_resize(Container& c, size_t const s)
{
	c.resize(s);
}

template <class Func, class Container>
typename std::enable_if<is_random_accessible_container<Container>::value == false, Container>::type map(Func const& f, Container const& c, SeqBackend const&)
{
	Container ret;
	map_reserve(ret, c.size());
	for (auto const& v: c) {
		ret.emplace_back(std::move(f(v)));
	}
	return std::move(ret);
}


template <class Func, class Container>
typename std::enable_if<is_random_accessible_container<Container>::value == false, Container>::type map(Func const& f, Container const& c, SeqBackendNoMove const&)
{
	Container ret;
	map_reserve(ret, c.size());
	for (auto const& v: c) {
		ret.push_back(f(v));
	}
	return ret;
}

template <class T>
struct is_random_accessible_container<std::vector<T>>
{
	static constexpr bool value = true;
	//static constexpr bool value = (std::iterator_traits<typename Container::const_iterator>::iterator_category == std::random_access_iterator_tag);
};

template <class Func, class Container>
typename std::enable_if<is_random_accessible_container<Container>::value, Container>::type __attribute__ ((noinline)) map(Func const& f, Container const& c, SeqBackend const&)
{
	Container ret;
	map_resize(ret, c.size());
	for (size_t i = 0; i < c.size(); i++) {
		ret[i] = std::move(f(c[i]));
	}
	return std::move(ret);
}

template <class Func, class Container>
typename std::enable_if<is_random_accessible_container<Container>::value, Container>::type __attribute__ ((noinline)) map(Func const& f, Container const& c, SeqBackendNoMove const&)
{
	Container ret;
	map_resize(ret, c.size());
	for (size_t i = 0; i < c.size(); i++) {
		ret[i] = f(c[i]);
	}
	return ret;
}


// Parallel versions

template <class Func, class Container>
typename std::enable_if<is_random_accessible_container<Container>::value, Container>::type __attribute__ ((noinline)) map(Func const& f, Container const& c, OMPParallelBackend const&)
{
	Container ret;
	map_resize(ret, c.size());
#pragma omp parallel for
	for (size_t i = 0; i < c.size(); i++) {
		ret[i] = std::move(f(c[i]));
	}
	return std::move(ret);
}

template <class Func, class Container>
typename std::enable_if<is_random_accessible_container<Container>::value, Container>::type __attribute__ ((noinline)) map(Func const& f, Container const& c, TBBParallelBackend const&)
{
	Container ret;
	map_resize(ret, c.size());
	tbb::parallel_for(
		tbb::blocked_range<size_t>(0, c.size()),
		[&ret,&c,&f](tbb::blocked_range<size_t> const& r)
		{
			for (size_t i = r.begin(); i != r.end(); i++) {
				ret[i] = std::move(f(c[i]));
			}
		});
	return std::move(ret);
}


#if 0
template <class Func, class Container>
Container map(Func const& f, Container const& c, ParallelBackend const& backend)
{
	typedef typename Container::const_pointer const_pointer;
	typedef typename Container::value_type value_type;

	Container ret;
	map_reserve(ret, c.size());

	auto cur_it = c.begin();
	tbb::parallel_pipeline(backend.ntokens(),
			tbb::make_filter<void, const_pointer>(
				tbb::filter::serial,
				[&c,&cur_it](tbb::flow_control& fc) -> const_pointer
				{
					if (cur_it == c.end()) {
						fc.stop();
						return nullptr;
					}
					const const_pointer ret = &(*cur_it);
					cur_it++;
					return ret;
				})

			&

			tbb::make_filter<const_pointer, value_type>(
				tbb::filter::parallel,
				[&f](const_pointer const p)
				{
					return std::move(f(*p));
				})

			&

			tbb::make_filter<value_type, void>(
				tbb::filter::serial,
				[&ret](value_type const& v)
				{
					ret.emplace_back(std::move(v));
				})
	);

	return ret;
}

template <class Func, class Container>
Container map(Func const& f, Container const& c, ParallelBackendGrain const& backend)
{
	typedef typename Container::const_pointer const_pointer;
	typedef typename Container::value_type value_type;
	typedef typename Container::const_iterator const_iterator;
	typedef std::array<value_type, 64*1024/sizeof(value_type)> tmp_type;
	static constexpr size_t ntmp = sizeof(tmp_type)/sizeof(value_type);

	Container ret;
	//ret.reserve(c.size());
	
	const_iterator cur_it = c.begin();
	tbb::parallel_pipeline(backend.ntokens(),
			tbb::make_filter<void, const_iterator>(
				tbb::filter::serial,
				[&c,&cur_it](tbb::flow_control& fc)
				{
					if (cur_it == c.end()) {
						fc.stop();
						return c.end();
					}
					const_iterator ret = cur_it;
					std::advance(cur_it, ntmp); 
					return ret;
				})

			&

			tbb::make_filter<const_iterator, tmp_type>(
				tbb::filter::parallel,
				[&f](const_iterator it)
				{
					tmp_type ret;
					for (size_t i = 0; i < ntmp; i++) {
						ret[i] = std::move(f(*it));
						it++;
					}
					return ret;
				})

			&

			tbb::make_filter<tmp_type, void>(
				tbb::filter::serial,
				[&ret](tmp_type const& ar)
				{
					for (auto && v: ar) {
						ret.emplace_back(std::move(v));
					}
				})
	);

	return ret;
}
#endif

#if 0
template <class Func, class T>
std::vector<T> map(Func const& f, std::vector<T> const& c, SeqBackend const& backend)
{
	std::vector<T> ret;
	const size_t len = c.size();
	ret.resize(len);
	for (size_t i = 0; i < len; i++) {
		ret[i] = std::move(f(c[i]));
	}
	return std::move(ret);
}
#endif

#endif
